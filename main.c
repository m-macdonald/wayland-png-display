#include "xdg-shell-protocol.h"
#include "png-loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <syscall.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <libpng16/png.h>

struct wl_compositor* compositor;
struct wl_shm* shm;
struct wl_surface* surface;
struct wl_buffer* buffer;
struct xdg_wm_base* shell;
struct xdg_toplevel* top;
struct wl_seat* seat;
struct wl_keyboard* kb;
uint64_t* pixel;
uint16_t width = 200;
uint16_t height = 100;
img image;
uint8_t c = 0;
uint8_t clse;

struct wl_client_state {
    
};

int32_t alloc_shm(uint64_t size) {
    int8_t name[8];
    name[0] = '/';
    name[7] = 0;
    for (uint8_t i = 1; i < 6; i++) {
        name[i] = (rand() & 23) + 97;
    }

    int32_t fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
    shm_unlink(name);
    ftruncate(fd, size);

    return fd;
}

void resize() {
    uint64_t size = image.width * image.height * 4;
    // multiply by 4 because each pixel is 4 bytes. RGBA
    int32_t fd = alloc_shm(size); 

    pixel = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct wl_shm_pool* pool = wl_shm_create_pool(shm, fd, size);
    buffer = wl_shm_pool_create_buffer(pool, 0, image.width, image.height, image.width * 4, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);
}

void draw() {
    unsigned int i;
    unsigned int j;
    unsigned int k;
    for (i = 0; i < image.height; i++) {
        for (j = 0; j < image.width; j++) {
            for (k = 0; k < 4; k++) {
                memset(pixel, image.row_pointers[(i * image.width) + j], (i * image.width) + j);
                memset(pixel, image.row_pointers[(i * image.width)])
                // printf("%c", image.row_pointers[(i * image.width) + j]);
                // memset(pixel, image.row_pointers[(i * j) * k], (i * j) * k);
                // printf("%i", image.row_pointers[i][j + k]);
                // memset(pixel, image.row_pointers[i][j + k], i * (j + k));
            }
            // memset(pixel, image.row_pointers[i][j], (i + j) * 4);
            // memset(pixel, image.row_pointers[i][j])
            // memset(pixel, c++, i * j * 4);
        }
    }

    // memset(pixel, 255, image.width * image.height * 4);

    // memset(pixel, image.row_pointers, image.width * image.height * 4);

    wl_surface_attach(surface, buffer, 0, 0);
    // Tell the compositor that you want it to draw within the bounds specified.
    // Here we are telling it to draw the entire size we've defined, but it could be a lesser portion of the total if we wanted
    wl_surface_damage_buffer(surface, 0, 0, image.width, image.height);
    // Tells the compositor that we are ready for the latest render to be displayed.
    wl_surface_commit(surface);
}

struct wl_callback_listener cb_listener;

void frame_new(void* data, struct wl_callback* cb, uint32_t a) {
    wl_callback_destroy(cb);
    cb = wl_surface_frame(surface);
    wl_callback_add_listener(cb, &cb_listener, 0);
    draw();
}

struct wl_callback_listener cb_listener = {
    .done = frame_new
};

void xdg_surface_conf(void* data, struct xdg_surface* xdg_surface, uint32_t serial) {
    xdg_surface_ack_configure(xdg_surface, serial);

    if (!pixel) {
        resize();
    }

    draw();
}

struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_conf
};

void top_conf(void *data, struct xdg_toplevel* top, int32_t new_width, int32_t new_height, struct wl_array* states) {
    if (!new_width && !new_height) {
        return;
    }

    // if (image.width != new_width || image.height != new_height) {
    //     munmap(pixel, image.width * image.height * 4);
    //     image.width = new_width;
    //     image.height = new_height;
    //
    //     resize();
    // }
}

void top_close(void* data, struct xdg_toplevel* top) {
    clse = 1;
}

struct xdg_toplevel_listener top_listener = {
    .configure = top_conf,
    .close = top_close
};

void shell_ping(void* data, struct xdg_wm_base* shell, uint32_t serial) {
    xdg_wm_base_pong(shell, serial);
}

struct xdg_wm_base_listener shell_listener = {
    .ping = shell_ping
};

void seat_cap(void* data, struct wl_seat* seat, uint32_t cap) {
    
}

void seat_name(void* data, struct wl_seat* seat, const char* name) {
    
}

struct wl_seat_listener seat_listener = {
    .capabilities = seat_cap,
    .name = seat_name
};

void registry_global_handler (
    void *data,
    struct wl_registry *registry,
    uint32_t name,
    const char *interface,
    uint32_t version
) {
    printf("interface: '%s', version: %u, name: %u\n", interface, version, name);

    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 6);
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        shell = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
    }
}

void registry_global_remove_handler(
    void *data,
    struct wl_registry *registry,
    uint32_t name
) { }

const struct wl_registry_listener registry_listener = {
    .global = registry_global_handler,
    .global_remove = registry_global_remove_handler
};

int main(int argc, char** argv) {
    // image = load_png("/home/maddux/Documents/projects/wayland-client/yorha.png");
    image = load_png(argv[1]);
    struct wl_display *display = wl_display_connect(NULL);
    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    // waits for the first set of globals. Is blocking
    wl_display_roundtrip(display);

    surface = wl_compositor_create_surface(compositor);
    struct wl_callback* cb = wl_surface_frame(surface);
    wl_callback_add_listener(cb, &cb_listener, 0);

    wl_seat_add_listener(seat, &seat_listener, 0);


    struct xdg_surface* xdg_surface = xdg_wm_base_get_xdg_surface(shell, surface);
    xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, 0);
    top = xdg_surface_get_toplevel(xdg_surface);
    xdg_toplevel_add_listener(top, &top_listener, 0);
    xdg_wm_base_add_listener(shell, &shell_listener, 0);
    xdg_toplevel_set_title(top, "wayland client");
    wl_surface_commit(surface);

    while (wl_display_dispatch(display)) {
        if (clse) break;
    }
    
    wl_seat_release(seat);
    if (buffer) {
        wl_buffer_destroy(buffer);
    }

    xdg_toplevel_destroy(top);
    xdg_surface_destroy(xdg_surface);

    wl_surface_destroy(surface);
    wl_display_disconnect(display);

    return 0;
}

typedef struct img{
    unsigned int width;
    unsigned int height;
    unsigned char* row_pointers; 
} img;

img load_png(char* file_name);

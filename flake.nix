{
    description = "A very basic flake";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
        flake-utils.url = "github:numtide/flake-utils";
        mini-compile-commands = {
            url = "github:danielbarter/mini_compile_commands";
            flake = false;
        };
    };

    outputs = { self, nixpkgs, flake-utils, mini-compile-commands}:
        flake-utils.lib.eachDefaultSystem(system:
            let
                pkgs = import nixpkgs {
                    inherit system;
                };

                mcc-env = with pkgs; (callPackage mini-compile-commands {}).wrap stdenv;

            in {
                devShells.default = (pkgs.mkShell.override {stdenv = mcc-env;}) {
                    packages = with pkgs; [
                        clang
                        cmake
                        gdb
                        wayland
                        libpng
                    ];
                };
            }
        );
}

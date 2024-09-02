# Setup environment
The WASM build uses ubuntu 24.04 with it's default clang version.

Simple steps to get an clean build environment:
1. Create a docker container image with the provided `Dockerfile`. This mainly installs ubuntu+clang
2. run a docker container with the created image, mounting the source directory (containing the `*.c` and `*.h` files) under `/work`

# Build steps
1. `make`; that will already produce the output `iptc.wasm` file.

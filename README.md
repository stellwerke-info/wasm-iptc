# Setup environment
The WASM build uses ubuntu 24.04 with its default clang version, installed from the repos (necessary packages are `make binutils clang lld libc-dev`).

Simple steps to get an clean build environment:
1. Create a docker container image with the provided `Dockerfile`. This mainly installs ubuntu+clang
2. run a docker container with the created image, mounting the source directory (containing the `*.c` and `*.h` files) under `/work`

# Build steps
1. `make`; that will already produce the output `iptc.wasm` file.

# Updating the C code

`image.c` and `iptc.c` are derived from upstream PHP source code. the remaining C code files are handwritten code to faciliate building as a WASM binary.

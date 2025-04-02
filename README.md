## Prerequisites

Install the following packages:
```
sudo apt install build-essential libyaml-dev gcc python3-pip pkg-config gcc-aarch64-linux-gnu g++-aarch64-linux-gnu binutils-aarch64-linux-gnu
sudo pip3 install meson ninja
```
You may also need to install libcsp, libparam and protobuf.

## Purpose
This module's purpose is to attach metadata about images from the GNSS module to the images, this includes location, time, altitude and speed. TODO: Add IR and visual camera's temperatures and the 2 core temperatures of the processors to the metadata.

More metadata about the images may be added later

## Building the Module

To build the module, run the `configure` script with either `build` or `test` as argument, as following (using build):
```sh
./configure build
```

Use `build` when you want to cross-compile to AArch64, and use `test` if you want to compile to your current machine for testing purposes.

Compiling with the `test` argument will produce the following files in the `builddir` directory:
 - `metadata_module-exec`: Can be executed to test the module (Will not utilize shared memory for the sake of simplicity).
 - `libmetadata_module.so`: Shared Oject library for use on the host architecture.

Compiling with the `build` argument will produce the following files in the `builddir` directory:
 - `libmetadata_module.so`: Shared Oject library for use on AArch64 machines.

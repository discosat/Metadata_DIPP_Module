## Prerequisites

Install the following packages:
```
sudo apt install build-essential libyaml-dev gcc python3-pip pkg-config gcc-aarch64-linux-gnu g++-aarch64-linux-gnu binutils-aarch64-linux-gnu
sudo pip3 install meson ninja
```

Rename the project inside the `meson.build` file, by editing the `project_name` variable and the project name. This name is used for the shared library files, and the test executable.

You may also need to install libcsp, libparam and protobuf.

## Purpose
This modules purpose is to attach metadata about images from the GNSS module to the images, this includes location, time, altitude and speed.

More metadata about the images may be added later

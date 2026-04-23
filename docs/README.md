# Swingby Developers Documentation

THIS file is not organized.

## Fedora

`wayland-scanner` command is in `wayland-devel` package.

The protocol files (in XML format) are in `wayland-protocols-devel` package.

`<xkbcommon/xkbcommon.h>` is in `libxkbcommon-devel` package.

### For Building Skia

```sh
$ sudo dnf install zlib-ng-compat-devel libjpeg-turbo-devel fontconfig-devel \
    expat-devel libglvnd-devel
```

### Fedora Linux Asahi Remix 43

When building Skia, `bin/gn` is not compatible with this system.
You have to build `gn` manually.

```sh
$ git clone https://gn.googlesource.com/gn
$ cd gn
$ python build/gen.py
$ ninja -C out
```

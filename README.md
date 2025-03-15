# Swingby

A minimal GUI library written in C, rendered with [Skia](https://skia.org/).

Swingby is the first cross-platform GUI library that prioritizes Wayland first.

Swingby is not a full-featured framework. It is a library for minimal GUI.


## Features

Swingby not provide a window directly. But you can build your own window (that
client-side decorated).

The event system makes easier to make the event driven GUI programs.

It is based on OpenGL 4.6 (the latest version) for easier buffer management.


## Build

Swingby is a Wayland based library. Therefore your system must suppport Wayland
before building the project.

We are using the latest version of OpenGL for rendering. The version 4.6 was
released in 2017.

Here is the details of the requirements.

1. Wayland and protocols. `wayland-scanner` command also required.
2. OpenGL and EGL. Wayland using EGL. ¹
3. CMake.

- ¹. EGL is similar to GLX in X11, or GLFW however it provides very small features.
It is unusual to use EGL in desktop development except Wayland client.
It seems this mainly used by Android with GLES programming. We are looking for
expert of this library.

### Build Steps

There are two scripts in the project's root. One is `gen-shaders.py`, it simply
converts shader source code to the C string literals. The generated code will
be committed so you don't need to use this script.

The other one is `gen-protocols.sh`. This script generate the Wayland interface
code which are not included in the core of Wayland protocol.
I'm not sure that these must be committed or not. Therefore you must generate
the C code with this script.

Additionally, since it uses Skia, you need to build Skia first.

To build Skia, follow the below commands.

```sh
$ git submodule update --init --recursive
$ make build-skia
```

The core is `libskia.a` file.

```sh
$ file skia/out/Static/libskia.a
```

You can now build Swingby using CMake.

```sh
$ mkdir build
$ cd build
$ cmake ..
$ make
```

You can test Swingby with the examples.

```sh
$ make example
```
The examples are in the `examples/` directory with begin of two digit numbers.


#### Debug Logs

If you want to show debug logs, pass `SWINGBY_DEBUG` macro to CMake.

```sh
$ cmake .. -DSWINGBY_DEBUG=ON
```


## Terms

### Application

An application is a global object that manage the event loop, the platform
dependent global objects and an application's lifecycle.

### Surface

A surface is a rectangular region that something to be drawn. It contains real
renderig code. But a surface alone does not just appear on the screen.

### Desktop Surface

A desktop surface (this name is from XDG surface) make the surface visible on the
compositor (maybe the desktop environment).

### View

The views are the elements that rendered in a surface. A view has it's parent (except
the root-view), and contain the children. A view can listen a pointer (mouse) enter,
leave, move, click event and etc.

Perhaps this could be called a widget. In Swingby however not privide real controls.
But you can make your own buttons, inputs and others with the views.


## Contribute

To make a Wayland client is a very difficult and challenging work.

Any advice, ideas, or suggestions are all welcome.

Please don't hesitate to open any issues, as they drive development forward.


## License

Swingby is developing under MIT License. For the detail, see the LICENSE file.


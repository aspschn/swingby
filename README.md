# Swingby

A GUI engine for Wayland written in C/C++, rendered
with [Skia](https://skia.org/).

Swingby is not a full-featured framework. It is an engine for minimal GUI.


## Features

- Basic drawing
- Event system
- Low-level windowing system
- Various common Wayland protocols support

Swingby not provide a window directly. But you can build your own window (that
client-side decorated).

The event system makes easier to make the event driven GUI programs.


## Goal

Wayland client programming is very complicated because its core library
is just a thin wrapper around a low-level protocol (socket) to communicate
with the server, commonly called a compositor.

Swingby is a bridge for GUI frameworks. It provides object-oriented APIs
for drawing primitives and handling events.

While you can build a GUI application directly with this library, but doing
so would be quite painful. The primary purpose of Swingby is to serve as a
backend that can be bound and wrapped by high-level languages.


## Build

Currently, development is based only on Arch Linux. There is no reason
you cannot build Swingby on other distributions as we use very basic
dependencies common to modern desktop environments. However, you may need to
install some packages manually for a development build.

Swingby is a Wayland based library. Therefore, your system must suppport Wayland
before building the project.

We are using the latest version of OpenGL for GPU-accelerated rendering.
For machines that do not support OpenGL version 4.6 (such as legacy GPUs,
VMs, ARM-based devices, etc.), you can try setting the environment
variable `MESA_GL_VERSION_OVERRIDE=4.6`.

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
I'm not sure that these must be committed or not. Therefore, you must generate
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
The examples are in the `examples/` directory with begin of two-digit numbers.


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

The views are the elements that rendered in a surface. A view has its parent (except
the root-view), and contain the children. A view can listen a pointer (mouse) enter,
leave, move, click event etc.

Perhaps this could be called a widget. In Swingby however does not privide
real controls. But you can make your own buttons, inputs and others with
the views.


## Contribute

To make a Wayland client is a very difficult and challenging work.

Any advice, ideas, or suggestions are all welcome.

Please don't hesitate to open any issues, as they drive development forward.


## License

Swingby is developing under MIT License. For the detail, see the LICENSE file.

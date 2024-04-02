# Foundation

A thin wrapper library for building Wayland client GUI programs.

It is derived from [Blusher](https://github.com/orbitrc/blusher2/tree/scroll-view) the
Wayland GUI framework. These two projects share much of codebase, however this is
not a direct fork.

Originally Blusher has too high-level APIs while Wayland protocol is low-level.
This gap finally made development more difficult.

So, I decided to make a less abstracted library works as a middle-ware.


## Features

Foundation not provide a window directly. But you can build your own window (that
client-side decorated).

The event system makes easier to make the event driven GUI programs.

It based on OpenGL 4.6 (the latest version of) for rendering performance.


## Build

Foundation is a Wayland based library. Therefore your system must suppport Wayland
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

Bulid is simple.

```sh
$ mkdir build
$ cd build
$ cmake ..
$ make
```

You can test Foundation with the examples.

```sh
$ make example
```
The examples are in the `examples/` directory with begin of two digit numbers.


## Terms

### Application

An application is a global object that manage the event loop, Wayland global objects
and an application's lifecycle.

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

Perhaps this could be called a widget. In Foundation however not privide real controls.
But you can make your own buttons, inputs and others with the views.


## Contribute

To make a Wayland client is a very difficult and challenging work.

Any advice, ideas, or suggestions are all welcome.

Please don't hesitate to open any issues, as they drive development forward.


## License

Foundation is developing under MIT License. For the detail, see the LICENSE file.


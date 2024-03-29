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


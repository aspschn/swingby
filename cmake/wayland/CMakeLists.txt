set(WAYLAND_PROTOCOLS_SOURCES
    include/wayland-protocols/stable/xdg-shell.c
    include/wayland-protocols/stable/tablet-v2.c
    include/wayland-protocols/stable/viewporter.c
    include/wayland-protocols/staging/fractional-scale-v1.c
    include/wayland-protocols/staging/cursor-shape-v1.c
    include/wayland-protocols/unstable/text-input-unstable-v3.c
)

set(SWINGBY_PLATFORM_SOURCES
    ${WAYLAND_PROTOCOLS_SOURCES}
    src/egl-context/egl-context.h
    src/egl-context/egl-context.c
    src/xkb/xkb-context.h
    src/xkb/xkb-context.c
    src/xcursor/xcursor.h
    src/xcursor/xcursor.c
    src/helpers/application.h
    src/helpers/application.c
    src/helpers/surface.c

    PARENT_SCOPE
)

set(SWINGBY_PLATFORM_LINK_OPTIONS
    PRIVATE ""

    PARENT_SCOPE
)

set(SWINGBY_PLATFORM_LINK_LIBRARIES
    # Skia static library
    PRIVATE ${SWINGBY_SKIA_STATIC_DIR}/libskia.a

    # Swingby dependencies
    PRIVATE wayland-client
    PRIVATE wayland-egl
    PRIVATE EGL
    PRIVATE OpenGL
    PRIVATE xkbcommon

    # Skia dependencies
    PRIVATE freetype
    PRIVATE z
    PRIVATE png16
    PRIVATE jpeg

    PARENT_SCOPE
)

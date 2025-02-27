set(WAYLAND_PROTOCOLS_SOURCES
    include/wayland-protocols/stable/xdg-shell.c
    include/wayland-protocols/stable/tablet-v2.c
    include/wayland-protocols/staging/cursor-shape-v1.c
)

set(SWINGBY_PLATFORM_SOURCES
    ${WAYLAND_PROTOCOLS_SOURCES}
    src/platform/wayland/application.c
    src/platform/wayland/surface.c
    src/platform/wayland/desktop-surface.c
    src/platform/wayland/egl-context/egl-context.h
    src/platform/wayland/egl-context/egl-context.c
    src/platform/wayland/xkb/xkb-context.h
    src/platform/wayland/xkb/xkb-context.c
    src/platform/wayland/xcursor/xcursor.h
    src/platform/wayland/xcursor/xcursor.c
    src/platform/wayland/helpers/application.h
    src/platform/wayland/helpers/application.c
    src/skia/gl-context.h
    src/skia/gl-context.cpp
    PARENT_SCOPE
)

set(SWINGBY_PLATFORM_LINK_OPTIONS
    PRIVATE -lwayland-client
    PRIVATE -lwayland-egl
    PRIVATE -lEGL
    PRIVATE -lOpenGL
    PRIVATE -lxkbcommon
    # Skia
    PRIVATE -lfreetype
    PRIVATE -lz
    PRIVATE -lpng16
    PRIVATE -ljpeg

    PARENT_SCOPE
)

set(SWINGBY_PLATFORM_LINK_LIBRARIES
    PRIVATE ${SWINGBY_SKIA_STATIC_DIR}/libskia.a

    PARENT_SCOPE
)

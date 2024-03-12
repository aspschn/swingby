cmake_minimum_required(VERSION 3.5)

project(foundation
    VERSION 0.1.0
    LANGUAGES C
)

set(WAYLAND_PROTOCOLS_SOURCES
    include/wayland-protocols/stable/xdg-shell.c
)

set(FOUNDATION_SOURCES
    include/foundation/log.h
    include/foundation/point.h
    include/foundation/size.h
    include/foundation/rect.h
    include/foundation/color.h
    include/foundation/application.h
    include/foundation/surface.h
    include/foundation/desktop-surface.h
    include/foundation/view.h
    include/foundation/egl-context.h
    include/foundation/event.h
    include/foundation/event-dispatcher.h
    src/log.c
    src/color.c
    src/application.c
    src/surface.c
    src/desktop-surface.c
    src/view.c
    src/egl-context.c
    src/event-dispatcher.c
    ${WAYLAND_PROTOCOLS_SOURCES}
)

add_library(foundation SHARED
    ${FOUNDATION_SOURCES}
)

target_include_directories(foundation
    PRIVATE ./include
)

string(REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

if(FOUNDATION_DEBUG)
    add_definitions(-DFOUNDATION_DEBUG)
endif()

target_link_options(foundation
    PRIVATE -lwayland-client
    PRIVATE -lwayland-egl
    PRIVATE -lEGL
    PRIVATE -lGL
)

set(FOUNDATION_EXAMPLES_BINARY_DIR ${CMAKE_BINARY_DIR}/examples)

file(MAKE_DIRECTORY ${FOUNDATION_EXAMPLES_BINARY_DIR})
add_subdirectory(examples)

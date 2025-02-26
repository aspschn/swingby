cmake_minimum_required(VERSION 3.5)

project(swingby
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(WAYLAND_PROTOCOLS_SOURCES
    include/wayland-protocols/stable/xdg-shell.c
    include/wayland-protocols/stable/tablet-v2.c
    include/wayland-protocols/staging/cursor-shape-v1.c
)

set(SWINGBY_HEADERS
    include/swingby/log.h
    include/swingby/bench.h
    include/swingby/list.h
    include/swingby/point.h
    include/swingby/size.h
    include/swingby/rect.h
    include/swingby/color.h
    include/swingby/filter.h
    include/swingby/application.h
    include/swingby/output.h
    include/swingby/surface.h
    include/swingby/desktop-surface.h
    include/swingby/view.h
    include/swingby/image.h
    include/swingby/egl-context.h
    include/swingby/input.h
    include/swingby/cursor.h
    include/swingby/event.h
    include/swingby/event-dispatcher.h
    include/swingby/swingby.h
)

set(SWINGBY_SOURCES
    ${SWINGBY_HEADERS}
    src/log.c
    src/bench.c
    src/list.c
    src/size.c
    src/rect.c
    src/color.c
    src/filter.c
    src/application.c
    src/output.c
    src/surface.c
    src/desktop-surface.c
    src/view.c
    src/image.c
    src/egl-context.c
    src/cursor.c
    src/event.c
    src/event-dispatcher.c
    src/skia/gl-context.h
    src/skia/gl-context.cpp
    src/skia/raster-context.h
    src/skia/raster-context.cpp
    src/skia/context.h
    src/skia/context.cpp
    src/skia/draw.h
    src/skia/draw.cpp
    src/skia/load-image.h
    src/skia/load-image.cpp
    src/skia/backend/gl.h
    src/skia/backend/gl.cpp
    src/skia/backend/raster.h
    src/skia/backend/raster.cpp
    src/xkb/xkb-context.h
    src/xkb/xkb-context.c
    src/xcursor/xcursor.h
    src/xcursor/xcursor.c
    src/helpers/application.h
    src/helpers/application.c
    ${WAYLAND_PROTOCOLS_SOURCES}
)

add_library(swingby SHARED
    ${SWINGBY_SOURCES}
)

target_include_directories(swingby
    PRIVATE ./include
    # Skia
    PRIVATE ./
    PRIVATE ./skia
)

target_compile_options(swingby PRIVATE -Wall)

string(REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

if(SWINGBY_DEBUG)
    add_definitions(-DSWINGBY_DEBUG)
endif()

target_link_options(swingby
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
)

set(SWINGBY_SKIA_STATIC_DIR ${CMAKE_SOURCE_DIR}/skia/out/Static)

target_link_libraries(swingby
    PRIVATE ${SWINGBY_SKIA_STATIC_DIR}/libskia.a
)

# Version info.
set_target_properties(swingby PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Install
include(GNUInstallDirs)

set_target_properties(swingby PROPERTIES
    PUBLIC_HEADER
    "${SWINGBY_HEADERS}"
)

install(TARGETS swingby
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/swingby
)

# Examples
set(FOUNDATION_EXAMPLES_BINARY_DIR ${CMAKE_BINARY_DIR}/examples)

file(MAKE_DIRECTORY ${FOUNDATION_EXAMPLES_BINARY_DIR})
add_subdirectory(examples)

# Tests
set(SWINGBY_TESTS_BINARY_DIR ${CMAKE_BINARY_DIR}/tests)

file(MAKE_DIRECTORY ${SWINGBY_TESTS_BINARY_DIR})
add_subdirectory(tests)


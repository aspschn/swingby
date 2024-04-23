cmake_minimum_required(VERSION 3.5)

project(swingby
    VERSION 0.1.0
    LANGUAGES C
)

set(WAYLAND_PROTOCOLS_SOURCES
    include/wayland-protocols/stable/xdg-shell.c
)

set(SWINGBY_HEADERS
    include/swingby/log.h
    include/swingby/bench.h
    include/swingby/list.h
    include/swingby/point.h
    include/swingby/size.h
    include/swingby/rect.h
    include/swingby/color.h
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
    ${WAYLAND_PROTOCOLS_SOURCES}
)

add_library(swingby SHARED
    ${SWINGBY_SOURCES}
)

target_include_directories(swingby
    PRIVATE ./include
)

string(REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

if(FOUNDATION_DEBUG)
    add_definitions(-DFOUNDATION_DEBUG)
endif()

target_link_options(swingby
    PRIVATE -lwayland-client
    PRIVATE -lwayland-egl
    PRIVATE -lEGL
    PRIVATE -lGL
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

set(FOUNDATION_EXAMPLES_BINARY_DIR ${CMAKE_BINARY_DIR}/examples)

file(MAKE_DIRECTORY ${FOUNDATION_EXAMPLES_BINARY_DIR})
add_subdirectory(examples)

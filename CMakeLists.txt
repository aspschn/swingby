cmake_minimum_required(VERSION 3.5)

project(swingby
    VERSION 0.5.0
    LANGUAGES C CXX
)

set(SWINGBY_HEADERS
    include/swingby/common.h
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
    include/swingby/pixmap.h
    include/swingby/font.h
    include/swingby/glyph.h
    include/swingby/input.h
    include/swingby/cursor.h
    include/swingby/event.h
    include/swingby/event-dispatcher.h
    include/swingby/swingby.h
)

set(SWINGBY_SKIA_STATIC_DIR ${CMAKE_SOURCE_DIR}/skia/out/Static)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_subdirectory(cmake/wayland)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
    set(CMAKE_CXX_FLAGS_DEBUG "/MT")
    add_subdirectory(cmake/win32)
endif()

set(SWINGBY_SOURCES
    ${SWINGBY_HEADERS}
    ${SWINGBY_PLATFORM_SOURCES}
    src/common.c
    src/log.c
    src/bench.c
    src/list.c
    src/point.c
    src/size.c
    src/rect.c
    src/color.c
    src/filter.c
    src/output.c
    src/view.c
    src/image.c
    src/pixmap.c
    src/glyph.c
    src/cursor.c
    src/event.c
    src/event-dispatcher.c
    src/skia/gl-renderer.h
    src/skia/gl-renderer.cpp
    src/skia/raster-renderer.cpp
    src/skia/renderer.h
    src/skia/renderer.cpp
    src/skia/draw.h
    src/skia/draw.cpp
    src/skia/load-image.h
    src/skia/load-image.cpp
    src/helpers/shared.h
    src/helpers/shared.c
    src/impl/image-impl.cpp
    src/impl/glyph-impl.h
    src/impl/glyph-impl.hpp
    src/impl/glyph-impl.cpp
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
    ${SWINGBY_PLATFORM_LINK_OPTIONS}
)

target_link_libraries(swingby
    ${SWINGBY_PLATFORM_LINK_LIBRARIES}
)

target_compile_definitions(swingby
    PRIVATE SB_BUILDING_SHARED_LIBRARY
)

# Platform macros.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions(SB_PLATFORM_WAYLAND)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(SB_PLATFORM_WIN32)
endif()

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


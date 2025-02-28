set(SWINGBY_PLATFORM_SOURCES
    src/platform/win32/application.c
    src/platform/win32/surface.c
    src/platform/win32/d3d-context/d3d-context.h
    src/platform/win32/d3d-context/d3d-context.cpp

    PARENT_SCOPE
)

set(SWINGBY_PLATFORM_LINK_OPTIONS
    PRIVATE ""

    PARENT_SCOPE
)

set(SWINGBY_PLATFORM_LINK_LIBRARIES
    PRIVATE ${SWINGBY_SKIA_STATIC_DIR}/skia.lib

    PARENT_SCOPE
)

set(SWINGBY_PLATFORM_SOURCES
    src/platform/cocoa/application.mm
    src/platform/cocoa/surface.mm
    src/platform/cocoa/desktop-surface.mm

    PARENT_SCOPE
)

set(SWINGBY_PLATFORM_LINK_OPTIONS
    PRIVATE ""

    PARENT_SCOPE
)

set(SWINGBY_PLATFORM_LINK_LIBRARIES
    PRIVATE ${SWINGBY_SKIA_STATIC_DIR}/libskia.a

    "-framework Cocoa"
    "-framework QuartzCore"
    "-framework Metal"

    PARENT_SCOPE
)

set_source_files_properties(${SWINGBY_PLATFORM_SOURCES} PROPERTIES
    COMPILE_FLAGS "-fobjc-arc"
)

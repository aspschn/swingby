set(EXAMPLE_NAME "glyphs")
set(EXAMPLE_NUM "16")

find_package(PkgConfig QUIET)

if(PKG_CONFIG_FOUND)
    pkg_check_modules(PANGO pango)
endif()

if(NOT PANGO_FOUND)
    add_custom_target(example_${EXAMPLE_NAME}
        COMMENT "Skipping glyphs example. Pango not found."
    )
    return()
endif()

pkg_check_modules(PANGO REQUIRED IMPORTED_TARGET pango)

add_executable(example_${EXAMPLE_NAME} EXCLUDE_FROM_ALL
    main.c
)
set_target_properties(example_${EXAMPLE_NAME} PROPERTIES
    OUTPUT_NAME ${EXAMPLE_NUM}-${EXAMPLE_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

target_include_directories(example_${EXAMPLE_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    PRIVATE PkgConfig::PANGO
)

target_link_libraries(example_${EXAMPLE_NAME}
    swingby
    PkgConfig::PANGO
    pangoft2-1.0
    fontconfig
)

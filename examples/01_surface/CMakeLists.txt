add_executable(example_surface EXCLUDE_FROM_ALL
    main.c
)
set_target_properties(example_surface PROPERTIES
    OUTPUT_NAME 01-surface
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

target_include_directories(example_surface
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(example_surface
    swingby
)

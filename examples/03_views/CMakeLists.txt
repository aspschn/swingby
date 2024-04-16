add_executable(example_views EXCLUDE_FROM_ALL
    main.c
)
set_target_properties(example_views PROPERTIES
    OUTPUT_NAME 03-views
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

target_include_directories(example_views
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(example_views
    swingby
)

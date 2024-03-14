add_executable(example_views EXCLUDE_FROM_ALL
    main.c
)
set_target_properties(example_views PROPERTIES OUTPUT_NAME views)

target_include_directories(example_views
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(example_views
    foundation
)

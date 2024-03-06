add_executable(example_surface EXCLUDE_FROM_ALL
    main.c
)
set_target_properties(example_surface PROPERTIES OUTPUT_NAME surface)

target_include_directories(example_surface
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(example_surface
    foundation
)

add_executable(example_desktop-surface EXCLUDE_FROM_ALL
    main.c
)
set_target_properties(example_desktop-surface PROPERTIES OUTPUT_NAME desktop-surface)

target_include_directories(example_desktop-surface
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(example_desktop-surface
    foundation
)

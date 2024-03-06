add_executable(example_application EXCLUDE_FROM_ALL
    main.c
)
set_target_properties(example_application PROPERTIES OUTPUT_NAME application)

target_include_directories(example_application
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(example_application
    foundation
)

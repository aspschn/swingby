add_executable(example_event-listener EXCLUDE_FROM_ALL
    main.c
)
set_target_properties(example_event-listener PROPERTIES
    OUTPUT_NAME 04-event-listener
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

target_include_directories(example_event-listener
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(example_event-listener
    swingby
)

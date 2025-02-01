set(EXAMPLE_NAME "clip")
set(EXAMPLE_NUM "11")

add_executable(example_${EXAMPLE_NAME} EXCLUDE_FROM_ALL
    main.c
)
set_target_properties(example_${EXAMPLE_NAME} PROPERTIES
    OUTPUT_NAME ${EXAMPLE_NUM}-${EXAMPLE_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

target_include_directories(example_${EXAMPLE_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(example_${EXAMPLE_NAME}
    swingby
)

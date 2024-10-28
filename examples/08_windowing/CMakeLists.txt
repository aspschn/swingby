set(EXAMPLE_NAME "windowing")
set(EXAMPLE_NUM "08")

add_executable(example_${EXAMPLE_NAME} EXCLUDE_FROM_ALL
    title-bar.h
    title-bar.c
    decoration.h
    decoration.c
    window.h
    window.c
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

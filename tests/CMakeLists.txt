add_executable(test-list
    list.c
)

target_include_directories(test-list
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(test-list
    swingby
)


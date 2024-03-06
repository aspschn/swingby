add_subdirectory(application)

add_custom_target(example)

add_dependencies(example
    example_application
)

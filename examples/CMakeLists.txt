add_subdirectory(application)
add_subdirectory(surface)

add_custom_target(example)

add_dependencies(example
    example_application
    example_surface
)

add_subdirectory(application)
add_subdirectory(surface)
add_subdirectory(desktop-surface)
add_subdirectory(views)
add_subdirectory(event-listener)

add_custom_target(example)

add_dependencies(example
    example_application
    example_surface
    example_desktop-surface
    example_views
    example_event-listener
)

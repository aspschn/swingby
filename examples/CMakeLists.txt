add_subdirectory(00_application)
add_subdirectory(01_surface)
add_subdirectory(02_desktop-surface)
add_subdirectory(03_views)
add_subdirectory(04_event-listener)
add_subdirectory(05_image-view)
add_subdirectory(06_window)

add_custom_target(example)

add_dependencies(example
    example_application
    example_surface
    example_desktop-surface
    example_views
    example_event-listener
    example_image-view
    example_window
)

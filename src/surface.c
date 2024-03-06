#include <foundation/surface.h>

#include <stdlib.h>

#include <wayland-client.h>
#include <wayland-egl.h>

#include <foundation/application.h>

struct ft_surface_t {
    struct wl_surface *_wl_surface;
    struct wl_egl_window *_wl_egl_window;
    ft_size_t _size;
};

ft_surface_t* ft_surface_new()
{
    ft_surface_t *surface = malloc(sizeof(ft_surface_t));

    surface->_size.width = 100.0f;
    surface->_size.height = 100.0f;

    ft_application_t *app = ft_application_instance();

    surface->_wl_surface = wl_compositor_create_surface(
        ft_application_wl_compositor(app));

    return surface;
}

void ft_surface_set_wl_surface(ft_surface_t *surface,
                               struct wl_surface *wl_surface)
{
    surface->_wl_surface = wl_surface;
}

const ft_size_t* ft_surface_size(ft_surface_t *surface)
{
    return &surface->_size;
}

void ft_surface_set_size(ft_surface_t *surface, const ft_size_t *size)
{
    surface->_size.width = size->width;
    surface->_size.height = size->height;
}

void ft_surface_commit(ft_surface_t *surface)
{
    wl_surface_commit(surface->_wl_surface);
}

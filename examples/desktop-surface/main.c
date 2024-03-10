#include <foundation/application.h>
#include <foundation/desktop-surface.h>

int main(int argc, char *argv[])
{
    ft_application_t *app = ft_application_new(argc, argv);

    ft_desktop_surface_t *surface = ft_desktop_surface_new(
        FT_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    // ft_surface_commit(surface);

    return ft_application_exec(app);
}


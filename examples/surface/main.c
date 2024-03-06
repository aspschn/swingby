#include <foundation/application.h>
#include <foundation/surface.h>

int main(int argc, char *argv[])
{
    ft_application_t *app = ft_application_new(argc, argv);

    ft_surface_t *surface = ft_surface_new();

    ft_surface_commit(surface);

    return ft_application_exec(app);
}


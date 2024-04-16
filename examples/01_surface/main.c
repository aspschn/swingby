#include <swingby/application.h>
#include <swingby/surface.h>

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_surface_t *surface = sb_surface_new();

    sb_surface_commit(surface);

    return sb_application_exec(app);
}


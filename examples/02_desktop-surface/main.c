#include <swingby/swingby.h>

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_desktop_surface_show(surface);

    sb_desktop_surface_hide(surface);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}


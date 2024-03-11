#ifndef _FOUNDATION_DESKTOP_SURFACE_H
#define _FOUNDATION_DESKTOP_SURFACE_H

typedef enum ft_desktop_surface_role {
    FT_DESKTOP_SURFACE_ROLE_TOPLEVEL,
    FT_DESKTOP_SURFACE_ROLE_POPUP,
} ft_desktop_surface_role;

typedef struct ft_desktop_surface_t ft_desktop_surface_t;

ft_desktop_surface_t* ft_desktop_surface_new(ft_desktop_surface_role role);

void ft_desktop_surface_show(ft_desktop_surface_t *desktop_surface);

void ft_desktop_surface_hide(ft_desktop_surface_t *desktop_surface);

#endif /* _FOUNDATION_DESKTOP_SURFACE_H */

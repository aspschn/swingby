#include "xcursor.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>

#include <swingby/log.h>

#define XCURSOR_DIR "/usr/share/icons"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//!<======================
//!< Helper Functions
//!<======================

static void _append_string(char *target, const char *str)
{
    uint64_t len = strlen(target);
    strcpy(target + len, str);
}

sb_xcursor_theme_manager_t* sb_xcursor_theme_manager_load()
{
    sb_xcursor_theme_manager_t *manager =
        malloc(sizeof(sb_xcursor_theme_manager_t));

    manager->ids = sb_list_new();

    // Iterate directories.
    DIR *dir = opendir(XCURSOR_DIR);
    if (!dir) {
        sb_log_warn("Directory not found - /usr/share/icons");
        return manager;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_DIR) {
            continue;
        }
        char icon_dirname[512];
        strcpy(icon_dirname, XCURSOR_DIR);
        _append_string(icon_dirname, "/");
        _append_string(icon_dirname, entry->d_name);

        DIR *icon_dir = opendir(icon_dirname);

        // Check the icon directory contains `cursors` directory.
        bool has_cursors = false;
        struct dirent *icon_entry;
        while ((icon_entry = readdir(icon_dir)) != NULL) {
            if (strcmp(icon_entry->d_name, "cursors") == 0) {
                has_cursors = true;
                break;
            }
        }
        closedir(icon_dir);

        if (has_cursors) {
            char *id = malloc(strlen(entry->d_name) + 1);
            strcpy(id, entry->d_name);
            sb_list_push(manager->ids, id);
        }
    }

    closedir(dir);

    return manager;
}

const sb_list_t* sb_xcursor_theme_manager_ids(
    sb_xcursor_theme_manager_t *manager)
{
    return manager->ids;
}

void sb_xcursor_theme_manager_free(sb_xcursor_theme_manager_t *manager)
{
    for (uint64_t i = 0; i < sb_list_length(manager->ids); ++i) {
        char *id = sb_list_at(manager->ids, i);
        free(id);
    }

    free(manager);
}

sb_xcursor_theme_t* sb_xcursor_theme_new(const char *id)
{
    sb_xcursor_theme_t *theme = malloc(sizeof(sb_xcursor_theme_t));

    theme->id = malloc(sizeof(char) * strlen(id) + 1);
    strcpy((char*)theme->id, id);

    theme->files = sb_list_new();

    return theme;
}

void sb_xcursor_theme_scan_files(sb_xcursor_theme_t *theme)
{
    char dirname[512];
    strcpy(dirname, XCURSOR_DIR);
    _append_string(dirname, "/");
    _append_string(dirname, theme->id);
    _append_string(dirname, "/cusors");

    DIR *dir = opendir(dirname);
    // TODO: Error check.

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char *filename = malloc(sizeof(char) * strlen(entry->d_name));
        sb_list_push(theme->files, filename);
    }

    closedir(dir);
}

void sb_xcursor_theme_free(sb_xcursor_theme_t *theme)
{
    free((void*)theme->id);

    for (uint64_t i = 0; i < sb_list_length(theme->files); ++i) {
        free(sb_list_at(theme->files, i));
    }

    free(theme);
}

#ifdef __cplusplus
}
#endif // __cplusplus

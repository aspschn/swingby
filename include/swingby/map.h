#ifndef SWINGBY_MAP_H
#define SWINGBY_MAP_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/// \struct sb_map_t
typedef struct sb_map_t sb_map_t;

sb_map_t* sb_map_new();

void sb_map_set(sb_map_t *map, void *key, void *value);

void* sb_map_get(sb_map_t *map, void *key);

void sb_map_free(sb_map_t *map);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* SWINGBY_MAP_H */

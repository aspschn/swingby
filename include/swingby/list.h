#ifndef _FOUNDATION_LIST_H
#define _FOUNDATION_LIST_H

#include <stdint.h>

#include <swingby/common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_list_t sb_list_t;

SB_EXPORT
sb_list_t* sb_list_new();

SB_EXPORT
void sb_list_push(sb_list_t *list, void *data);

SB_EXPORT
void* sb_list_at(const sb_list_t *list, uint64_t index);

SB_EXPORT
uint64_t sb_list_length(const sb_list_t *list);

SB_EXPORT
void* sb_list_remove(sb_list_t *list, uint64_t index);

SB_EXPORT
void sb_list_free(sb_list_t *list);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_LIST_H */

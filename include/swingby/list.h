#ifndef _FOUNDATION_LIST_H
#define _FOUNDATION_LIST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_list_t sb_list_t;

sb_list_t* sb_list_new();

void sb_list_push(sb_list_t *list, void *data);

void* sb_list_at(sb_list_t *list, uint64_t index);

uint64_t sb_list_length(sb_list_t *list);

void* sb_list_remove(sb_list_t *list, uint64_t index);

void sb_list_free(sb_list_t *list);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_LIST_H */

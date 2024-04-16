#ifndef _FOUNDATION_LIST_H
#define _FOUNDATION_LIST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ft_list_t ft_list_t;

ft_list_t* ft_list_new();

void ft_list_push(ft_list_t *list, void *data);

void* ft_list_at(ft_list_t *list, uint64_t index);

uint64_t ft_list_length(ft_list_t *list);

void* ft_list_remove(ft_list_t *list, uint64_t index);

void ft_list_free(ft_list_t *list);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_LIST_H */

#include <foundation/list.h>

#include <stdint.h>
#include <stdlib.h>

#include <foundation/log.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ft_list_t {
    uint64_t capacity;
    uint64_t length;
    void **data;
};

ft_list_t* ft_list_new()
{
    ft_list_t *list = malloc(sizeof(ft_list_t));

    list->capacity = 16;
    list->length = 0;
    list->data = malloc(sizeof(void*));
    *list->data = malloc(sizeof(void**) * list->capacity);

    return list;
}

void ft_list_push(ft_list_t *list, void *data)
{
    if (list->capacity == list->length) {
        // Increase capacity.
        void **old_data = list->data;
        list->capacity = list->capacity * 2;
        void **new_data = malloc(sizeof(void**) * list->capacity);
        for (int i = 0; i < list->length; ++i) {
            new_data[i] = old_data[i];
        }
        free(old_data);
        list->data = new_data;
    }

    list->data[list->length] = data;
    list->length += 1;
}

void* ft_list_at(ft_list_t *list, uint64_t index)
{
    return list->data[index];
}

uint64_t ft_list_length(ft_list_t *list)
{
    return list->length;
}

void* ft_list_remove(ft_list_t *list, uint64_t index)
{
    void *item = list->data[index];

    for (uint64_t i = index; i < list->length - 1; ++i) {
        list->data[i] = list->data[i + 1];
    }

    list->length -= 1;

    return item;
}

#ifdef __cplusplus
}
#endif

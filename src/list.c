#include <swingby/list.h>

#include <stdint.h>
#include <stdlib.h>

#include <swingby/log.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sb_list_t {
    uint64_t capacity;
    uint64_t length;
    void **data;
};

sb_list_t* sb_list_new()
{
    sb_list_t *list = malloc(sizeof(sb_list_t));

    list->capacity = 16;
    list->length = 0;
    // list->data = malloc(sizeof(void*));
    list->data = malloc(sizeof(void**) * list->capacity);

    return list;
}

void sb_list_push(sb_list_t *list, void *data)
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

void* sb_list_at(sb_list_t *list, uint64_t index)
{
    return list->data[index];
}

uint64_t sb_list_length(sb_list_t *list)
{
    return list->length;
}

void* sb_list_remove(sb_list_t *list, uint64_t index)
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

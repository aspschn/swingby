#include <stdlib.h>
#include <stdio.h>

#include <swingby/list.h>

int main(int argc, char *argv[])
{
    sb_list_t *list = sb_list_new();

    // Push elements.
    for (int i = 0; i < 1000; ++i) {
        int *val = malloc(sizeof(int));
        *val = i;
        sb_list_push(list, val);
    }

    // Length check.
    if (sb_list_length(list) != 1000) {
        return 1;
    }

    // Iterate elements.
    for (int i = 0; i < 1000; ++i) {
        int *val = (int*)sb_list_at(list, i);
        if (*val != i) {
            return 1;
        }
    }

    // Remove elements.
    for (int i = 999; i >= 0; --i) {
        int *val = (int*)sb_list_remove(list, i);
        printf("%d\n", *val);
        free(val);
    }

    return 0;
}


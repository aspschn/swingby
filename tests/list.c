#include <stdlib.h>
#include <stdio.h>

#include <swingby/list.h>

static void print_list(sb_list_t *list)
{
    for (int i = 0; i < sb_list_length(list); ++i) {
        printf("%d, ", *(int*)sb_list_at(list, i));
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    sb_list_t *list = sb_list_new();

    // Push elements.
    for (int i = 0; i < 100; ++i) {
        int *val = malloc(sizeof(int));
        *val = i;
        sb_list_push(list, val);
        print_list(list);
    }

    // Length check.
    if (sb_list_length(list) != 100) {
        return 1;
    }

    // Iterate elements.
    for (int i = 0; i < 100; ++i) {
        int *val = (int*)sb_list_at(list, i);
        if (*val != i) {
            return 1;
        }
    }

    // Remove elements.
    for (int i = 99; i >= 0; --i) {
        int *val = (int*)sb_list_remove(list, i);
        printf("%d\n", *val);
        free(val);
    }

    return 0;
}


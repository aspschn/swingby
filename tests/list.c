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

static void list_reverse()
{
    int n1 = 1;
    int n2 = 2;
    int n3 = 3;
    int n4 = 4;
    int n5 = 5;

    sb_list_t *even_list = sb_list_new();
    sb_list_push(even_list, &n1);
    sb_list_push(even_list, &n2);
    sb_list_push(even_list, &n3);
    sb_list_push(even_list, &n4);
    sb_list_reverse(even_list);
    if (*(int*)sb_list_at(even_list, 0) != 4) { exit(1); }
    if (*(int*)sb_list_at(even_list, 1) != 3) { exit(1); }
    if (*(int*)sb_list_at(even_list, 2) != 2) { exit(1); }
    if (*(int*)sb_list_at(even_list, 3) != 1) { exit(1); }
    sb_list_free(even_list);

    sb_list_t *odd_list = sb_list_new();
    sb_list_push(odd_list, &n1);
    sb_list_push(odd_list, &n2);
    sb_list_push(odd_list, &n3);
    sb_list_push(odd_list, &n4);
    sb_list_push(odd_list, &n5);
    sb_list_reverse(odd_list);
    if (*(int*)sb_list_at(odd_list, 0) != 5) { exit(1); }
    if (*(int*)sb_list_at(odd_list, 1) != 4) { exit(1); }
    if (*(int*)sb_list_at(odd_list, 2) != 3) { exit(1); }
    if (*(int*)sb_list_at(odd_list, 3) != 2) { exit(1); }
    if (*(int*)sb_list_at(odd_list, 4) != 1) { exit(1); }
    sb_list_free(odd_list);
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

    // Free list.
    sb_list_free(list);

    // Reverse test.
    list_reverse();

    return 0;
}


#include <stdlib.h>

#include <swingby/swingby.h>

static void rect_intersects()
{
    // Test case 1.
    {
        sb_rect_t r1;
        r1.pos.x = 0.0f;
        r1.pos.y = 0.0f;
        r1.size.width = 50.0f;
        r1.size.height = 50.0f;
        sb_rect_t r2;
        r2.pos.x = 100.0f;
        r2.pos.y = 100.0f;
        r2.size.width = 50.0f;
        r2.size.height = 59.0f;

        bool res = sb_rect_intersects(&r1, &r2);
        if (res == true) {
            exit(1);
        }
    }

    // Test case 2.
    {
        sb_rect_t r1;
        r1.pos.x = 0.0f;
        r1.pos.y = 0.0f;
        r1.size.width = 50.0f;
        r1.size.height = 50.0f;
        sb_rect_t r2;
        r2.pos.x = 49.0f;
        r2.pos.y = 0.0f;
        r2.size.width = 50.0f;
        r2.size.height = 50.0f;

        bool res = sb_rect_intersects(&r1, &r2);
        if (res == false) {
            exit(2);
        }
    }

    // Test case 3.
    {
        sb_rect_t r1 = {{0.0f, 0.0f}, {100.0f, 100.0f}};
        sb_rect_t r2 = {{25.0f, 25.0f}, {50.0f, 50.0f}};

        bool res = sb_rect_intersects(&r1, &r2);
        if (res == false) {
            exit(3);
        }
    }

    // Test case 4.
    {
        sb_rect_t r1 = {{0.0f, 0.0f}, {50.0f, 50.0f}};
        sb_rect_t r2 = {{50.0f, 0.0f}, {50.0f, 50.0f}};

        bool res = sb_rect_intersects(&r1, &r2);
        if (res == true) {
            exit(4);
        }
    }

    // Test case 5.
    {
        sb_rect_t r1 = {{10.0f, 10.0f}, {50.0f, 50.0f}};
        sb_rect_t r2 = {{10.0f, 10.0f}, {50.0f, 50.0f}};

        bool res = sb_rect_intersects(&r1, &r2);
        if (res == false) {
            exit(5);
        }
    }

    // Test case 6.
    {
        sb_rect_t r1 = {{50.0f, 50.0f}, {10.0f, 10.0f}};
        sb_rect_t r2 = {{55.0f, 55.0f}, {20.0f, 20.0f}};

        bool res = sb_rect_intersects(&r1, &r2);
        if (res == false) {
            exit(6);
        }
    }

    // Test case 7.
    {
        sb_rect_t r1 = {{0.0f, 0.0f}, {50.0f, 50.0f}};
        sb_rect_t r2 = {{0.0f, 50.0f}, {50.0f, 50.0f}};

        bool res = sb_rect_intersects(&r1, &r2);
        if (res == true) {
            exit(7);
        }
    }

    // Test case 8.
    {
        sb_rect_t r1 = {{10.0f, 10.0f}, {30.0f, 30.0f}};
        sb_rect_t r2 = {{35.0f, 10.0f}, {30.0f, 30.0f}};

        bool res = sb_rect_intersects(&r1, &r2);
        if (res == false) {
            exit(8);
        }
    }
}

int main(int argc, char *argv[])
{
    rect_intersects();

    return 0;
}

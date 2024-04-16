#include <swingby/application.h>

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    return sb_application_exec(app);
}


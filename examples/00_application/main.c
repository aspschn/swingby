#include <swingby/application.h>

int main(int argc, char *argv[])
{
    ft_application_t *app = ft_application_new(argc, argv);

    return ft_application_exec(app);
}


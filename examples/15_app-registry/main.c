#include <stdio.h>

#include <swingby/swingby.h>

void on_registry(sb_event_t *event)
{
    fprintf(stderr, " * <%s, %d> %d\n", event->registry.interface,
            event->registry.version, event->registry.name);
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new_with_registry_listener(argc,
        argv,
        on_registry,
        NULL);

    return sb_application_exec(app);
}


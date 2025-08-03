#include <swingby/application.h>

#include <stdlib.h>

#import <Cocoa/Cocoa.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sb_application_t {
    NSApplication *ns_application;
};

// Singleton object.
static sb_application_t *_sb_application_instance = NULL;

sb_application_t *sb_application_new(int argc, char *argv[]) {
    sb_application_t *app = (sb_application_t*)malloc(sizeof(sb_application_t));

    app->ns_application = [NSApplication sharedApplication];

    _sb_application_instance = app;

    return app;
}

sb_application_t *sb_application_instance() {
    return _sb_application_instance;
}

void sb_application_post_event(sb_application_t *application,
                               sb_event_t *event)
{
    // TODO.
}

int sb_application_exec(sb_application_t *application)
{
    // TODO.

    return 0;
}

#ifdef __cplusplus
}
#endif

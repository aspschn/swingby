#define _GNU_SOURCE // mkostemp

#include "surface.h"

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>

#include <linux/limits.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int _create_tmpfile(int size)
{
    const char template[] = "swingby-XXXXXX";
    char path[PATH_MAX];

    const char *runtime_dir = getenv("XDG_RUNTIME_DIR");
    if (runtime_dir == NULL) {
        return -1;
    }

    snprintf(path, sizeof(path), "%s/%s", runtime_dir, template);

    int fd = mkostemp(path, O_CLOEXEC);
    if (fd < 0) {
        return -1;
    }

    unlink(path);

    if (ftruncate(fd, size) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#include <swingby/common.h>

#include <stddef.h>
#include <time.h>

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// TODO: Use `int64_t` rather than `uint64_t`. 
uint64_t sb_time_now_milliseconds()
{
    uint64_t now = 0;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    now = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);

    return now;
}

int64_t sb_time_milliseconds_to_nanoseconds(int64_t ms)
{
    return ms * 1000000;
}

struct timespec sb_time_milliseconds_to_timespec(int64_t ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;

    return ts;
}

#ifdef __cplusplus
}
#endif // __cplusplus

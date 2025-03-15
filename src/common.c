#include <swingby/common.h>

#include <stddef.h>

#if defined(SB_PLATFORM_WAYLAND)
#include <sys/time.h>
#elif defined(SB_PLATFORM_WIN32)
#include <Windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint64_t sb_time_now_milliseconds()
{
    uint64_t now = 0;

#if defined(SB_PLATFORM_WAYLAND)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    now = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#elif defined(SB_PLATFORM_WIN32)
    FILETIME ft;
    ULARGE_INTEGER li;

    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;

    // Convert to Unix time.
    now = (li.QuadPart - 116444736000000000LL) / 10000;
#endif

    return now;
}

#ifdef __cplusplus
}
#endif // __cplusplus

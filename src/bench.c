#include <swingby/bench.h>

#include <stdlib.h>
#include <stdio.h>

#if defined(SB_PLATFORM_WAYLAND) || defined(SB_PLATFORM_COCOA)
#include <sys/time.h>
#elif defined(SB_PLATFORM_WIN32)
#include <Windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct sb_bench_t {
    const char *message;
#if defined(SB_PLATFORM_WAYLAND) || defined(SB_PLATFORM_COCOA)
    struct timeval start;
    struct timeval end;
#elif defined(SB_PLATFORM_WIN32)
    FILETIME start;
    FILETIME end;
#endif
};

sb_bench_t* sb_bench_new_F(const char *message)
{
    sb_bench_t *bench = malloc(sizeof(sb_bench_t));

    bench->message = message;
#if defined(SB_PLATFORM_WAYLAND) || defined(SB_PLATFORM_COCOA)
    gettimeofday(&bench->start, NULL);
#elif defined(SB_PLATFORM_WIN32)
    GetSystemTimeAsFileTime(&bench->start);
#endif

    return bench;
}

void sb_bench_end_F(sb_bench_t *bench)
{
#if defined(SB_PLATFORM_WAYLAND) || defined(SB_PLATFORM_COCOA)
    gettimeofday(&bench->end, NULL);

    double elapsed = (bench->end.tv_sec - bench->start.tv_sec)
        + (bench->end.tv_usec - bench->start.tv_usec) / 1000000.0;
#elif defined(SB_PLATFORM_WIN32)
    GetSystemTimeAsFileTime(&bench->end);
    ULARGE_INTEGER time_int;
    time_int.LowPart = bench->end.dwLowDateTime;
    time_int.HighPart = bench->end.dwHighDateTime;
    double elapsed = (double)time_int.QuadPart / 1000000.0;
#endif

    fprintf(stderr, "[%f] - %s\n", elapsed, bench->message);

    free(bench);
}

#ifdef __cplusplus
}
#endif

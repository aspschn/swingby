#include <swingby/bench.h>

#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ft_bench_t {
    const char *message;
    struct timeval start;
    struct timeval end;
};

ft_bench_t* ft_bench_new_F(const char *message)
{
    ft_bench_t *bench = malloc(sizeof(ft_bench_t));

    bench->message = message;
    gettimeofday(&bench->start, NULL);

    return bench;
}

void ft_bench_end_F(ft_bench_t *bench)
{
    gettimeofday(&bench->end, NULL);

    double elapsed = (bench->end.tv_sec - bench->start.tv_sec)
        + (bench->end.tv_usec - bench->start.tv_usec) / 1000000.0;

    fprintf(stderr, "[%f] - %s\n", elapsed, bench->message);

    free(bench);
}

#ifdef __cplusplus
}
#endif

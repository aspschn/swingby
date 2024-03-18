#ifndef _FOUNDATION_BENCH_H
#define _FOUNDATION_BENCH_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ft_bench_t ft_bench_t;

ft_bench_t* ft_bench_new_F(const char *message);

void ft_bench_end_F(ft_bench_t *bench);

#ifdef FOUNDATION_DEBUG
#define ft_bench_new(message) ft_bench_new_F(message)
#define ft_bench_end(bench) ft_bench_end_F(bench)
#else
#define ft_bench_new() NULL
#define ft_bench_end(bench)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_BENCH_H */

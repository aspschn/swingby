#ifndef _FOUNDATION_BENCH_H
#define _FOUNDATION_BENCH_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_bench_t sb_bench_t;

sb_bench_t* sb_bench_new_F(const char *message);

void sb_bench_end_F(sb_bench_t *bench);

#ifdef FOUNDATION_DEBUG
#define sb_bench_new(message) sb_bench_new_F(message)
#define sb_bench_end(bench) sb_bench_end_F(bench)
#else
#define sb_bench_new() NULL
#define sb_bench_end(bench)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_BENCH_H */

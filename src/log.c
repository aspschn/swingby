#include <foundation/log.h>

#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void ft_log(ft_log_level level, const char *format, va_list args)
{
    if (level == FT_LOG_LEVEL_DEBUG) {
#ifdef FOUNDATION_DEBUG
        fprintf(stdout, "\033[1;32m[DEBUG]\033[0m ");
        vfprintf(stdout, format, args);
        fflush(stdout);
#endif
    } else if (level == FT_LOG_LEVEL_WARN) {
        fprintf(stderr, "\033[1;33m[WARN]\033[0m ");
        vfprintf(stderr, format, args);
    } else if (level == FT_LOG_LEVEL_ERROR) {
        fprintf(stderr, "\033[1;33m[ERROR]\033[0m ");
        vfprintf(stderr, format, args);
    }
}

void ft_log_debug(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    ft_log(FT_LOG_LEVEL_DEBUG, format, args);

    va_end(args);
}

void ft_log_warn(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    ft_log(FT_LOG_LEVEL_WARN, format, args);

    va_end(args);
}

void ft_log_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    ft_log(FT_LOG_LEVEL_ERROR, format, args);

    va_end(args);
}

#ifdef __cplusplus
}
#endif

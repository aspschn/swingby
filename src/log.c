#include <foundation/log.h>

#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void ft_log(ft_log_level level, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    if (level == FT_LOG_LEVEL_DEBUG) {
#ifdef FOUNDATION_DEBUG
        fprintf(stdout, "\033[1;32m[DEBUG]\033[0m ");
        vfprintf(stdout, format, args);
        fflush(stdout);
#endif
    } else if (level == FT_LOG_LEVEL_WARN) {
        //
    } else if (level == FT_LOG_LEVEL_ERROR) {
        //
    }

    va_end(args);
}

void ft_log_debug(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    ft_log(FT_LOG_LEVEL_DEBUG, format, args);

    va_end(args);
}

#ifdef __cplusplus
}
#endif

#ifndef _FOUNDATION_LOG_H
#define _FOUNDATION_LOG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ft_log_level {
    FT_LOG_LEVEL_DEBUG,
    FT_LOG_LEVEL_WARN,
    FT_LOG_LEVEL_ERROR,
} ft_log_level;

/// \brief Print log with log level.
void ft_log(ft_log_level level, const char *format, va_list args);

void ft_log_debug(const char *format, ...);

void ft_log_warn(const char *format, ...);

void ft_log_error(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_LOG_H */

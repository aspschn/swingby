#ifndef _FOUNDATION_LOG_H
#define _FOUNDATION_LOG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum sb_log_level {
    SB_LOG_LEVEL_DEBUG,
    SB_LOG_LEVEL_WARN,
    SB_LOG_LEVEL_ERROR,
} sb_log_level;

/// \brief Print log with log level.
void sb_log(sb_log_level level, const char *format, va_list args);

void sb_log_debug(const char *format, ...);

void sb_log_warn(const char *format, ...);

void sb_log_error(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_LOG_H */

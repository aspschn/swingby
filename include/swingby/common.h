#ifndef _SWINGBY_COMMON_H
#define _SWINGBY_COMMON_H

#include <stdint.h>

#ifdef SB_PLATFORM_WIN32
#ifdef SB_BUILDING_SHARED_LIBRARY
#define SB_EXPORT __declspec(dllexport)
#else
#define SB_EXPORT __declspec(dllimport)
#endif
#else
#define SB_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint64_t sb_time_now_milliseconds();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_COMMON_H */

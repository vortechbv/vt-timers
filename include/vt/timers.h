#ifndef VT_TIMERS_H
#define VT_TIMERS_H

#include <stddef.h>

#ifndef VT_TIMERS_ATTR
#define VT_TIMERS_ATTR
#endif

#ifdef __cplusplus
#define VT_C_NAME_MANGLING extern "C"
#else
#define VT_C_NAME_MANGLING
#endif

#ifdef _MSC_VER
#define VT_C_CALLCONV __cdecl
#else
#define VT_C_CALLCONV
#endif

#define VT_C_API VT_C_NAME_MANGLING VT_TIMERS_ATTR


typedef enum vtErrorCodes {
    vtOK = 0,
    vtWARNING = 1,
    vtERROR = 2
} vtErrorCode;


VT_C_API vtErrorCode VT_C_CALLCONV vt_timer_tic(const char* name);

VT_C_API vtErrorCode VT_C_CALLCONV vt_timer_toc(const char* name);

VT_C_API vtErrorCode VT_C_CALLCONV vt_timers_to_cstring(char* cstring, const size_t n);

VT_C_API vtErrorCode VT_C_CALLCONV vt_timers_to_stdout();

VT_C_API vtErrorCode VT_C_CALLCONV vt_timers_reset();

#endif  // VT_TIMERS_H

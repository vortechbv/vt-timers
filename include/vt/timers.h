// Copyright (c) 2019 VORtech b.v.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

VT_C_API void VT_C_CALLCONV vt_last_error_message(char* cstring, const size_t n);

VT_C_API vtErrorCode VT_C_CALLCONV vt_last_error_code();


VT_C_API vtErrorCode VT_C_CALLCONV vt_timer_tic(const char* name);

VT_C_API vtErrorCode VT_C_CALLCONV vt_timer_toc(const char* name);

VT_C_API vtErrorCode VT_C_CALLCONV vt_timers_to_cstring(char* cstring, const size_t n);

VT_C_API vtErrorCode VT_C_CALLCONV vt_timers_to_stdout();

VT_C_API vtErrorCode VT_C_CALLCONV vt_timers_reset();

#endif  /* VT_TIMERS_H */

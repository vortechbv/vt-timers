#ifndef VT_TIMERS_H
#define VT_TIMERS_H

#ifndef VT_TIMERS_ATTR
#define VT_TIMERS_ATTR
#endif

VT_TIMERS_ATTR void vt_timer_tic(const char* name);

VT_TIMERS_ATTR void vt_timer_toc(const char* name);

VT_TIMERS_ATTR void vt_timers_to_cstring(char* cstring, const size_t n);

VT_TIMERS_ATTR void vt_timers_to_stdout();

VT_TIMERS_ATTR void vt_timers_reset();

#endif  // VT_TIMERS_H

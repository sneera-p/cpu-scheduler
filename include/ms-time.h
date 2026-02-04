#ifndef SCHEDULER__MS_TIME__H
#define SCHEDULER__MS_TIME__H

#include <stdint.h>
#include <inttypes.h>


typedef uint64_t ms_timer_s;
typedef ms_timer_s *const restrict MS_TIMER_;

typedef uint64_t ms_delta_s;

#define PRImst PRIu64
#define PRImsd PRIu64


static inline void incr_mstimer(MS_TIMER_ timer, const ms_delta_s delta)
{
   *timer += delta;
}


#endif /* SCHEDULER__MS_TIME__H */
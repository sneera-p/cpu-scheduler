#ifndef SCHEDULER__PROC__H
#define SCHEDULER__PROC__H

#include <stdint.h>
#include "ms-time.h"
#include "config.h"


struct proc
{
   ms_delta_s cpu_remaining;  // CPU time still needed to finish
   ms_delta_s cpu_total;      // Total CPU time requested
   ms_timer_s first_exec;     // Timestamp of the first execution
   ms_timer_s last_exec;      // Timestamp of the most recent execution
   ms_timer_s arrival_time;   // Timestamp when process entered the scheduler
   uint32_t pid;
   proc_state_e state;
   priority_e priority;
};

typedef struct proc proc_s;
typedef proc_s *const restrict PROC_;


/* --- derived properties --- */
extern ms_timer_s proc_completion_time(PROC_ proc);
extern ms_delta_s proc_turnaround_time(PROC_ proc);
extern ms_delta_s proc_active_time(PROC_ proc);
extern ms_delta_s proc_response_time(PROC_ proc);
extern ms_delta_s proc_work_done(PROC_ proc);
extern ms_delta_s proc_wait_time(PROC_ proc);


// Compares 2 processes and returns
//    -1: less than
//    0: equal,
//    1: greater than
[[nodiscard]] int64_t proc_cmp(const PROC_ proc1, const PROC_ proc2) [[reproducible]];

// Initializes a process struct and returns the time taken for creation
void proc_init(PROC_ proc, MS_TIMER_ timer, const priority_e priority);

// Runs the process for a given duration. Returns the the time taken.
void proc_run(PROC_ proc, MS_TIMER_ timer, const ms_delta_s quantum);

// Display process details
void proc_display(PROC_ proc, MS_TIMER_ timer);


/* --- HELPER MACROS --- */

#define PROC_NEW(proc)     ((proc) && (proc)->state == NEW)
#define PROC_READY(proc)   ((proc) && (proc)->state == READY)
#define PROC_RUNNING(proc) ((proc) && (proc)->state == RUNNING)
#define PROC_HASRUN(proc)  ((proc) && (proc)->last_exec != 0)
#define PROC_EXIT(proc)    ((proc) && (proc)->state == EXIT)


/* --- SANITY CHECKS --- */

// Ensures proc_s can be allocted in contiguous blocks
static_assert(sizeof(proc_s) % alignof(proc_s) == 0, "proc_s size must be a multiple of its alignment for safe array indexing");


#endif /* SCHEDULER__PROC__H */
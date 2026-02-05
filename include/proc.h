#ifndef SCHEDULER__PROC__H
#define SCHEDULER__PROC__H

#include <stdint.h>
#include <stdio.h>
#include "ms-time.h"
#include "config.h"

extern FILE *logstream;

/* --- PCB struct --- */

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


// Compares 2 processes and returns
//    -1: less than
//    0: equal,
//    1: greater than
[[nodiscard]] int8_t proc_cmp(const PROC_ proc1, const PROC_ proc2) [[reproducible]];

// Initializes a process struct and returns the time taken for creation
void proc_init(PROC_ proc, MS_TIMER_ timer, const priority_e priority);

// Runs the process for a given duration. Returns the time taken.
[[nodiscard]] ms_delta_s proc_run(PROC_ proc, MS_TIMER_ timer, const ms_delta_s quantum);


/* --- Process metrics --- */

struct proc_metrics
{
   ms_delta_s turnaround_time;
   ms_delta_s response_time;
   ms_delta_s wait_time;
   uint32_t pid;
};

typedef struct proc_metrics proc_metrics_s;

[[nodiscard]] proc_metrics_s proc_get_metrics(PROC_ proc) [[reproducible]];


// Display process details
void proc_snapshot(const PROC_ proc, MS_TIMER_ timer);
void proc_display(const PROC_ proc, const proc_metrics_s metrics);
void proc_print_table(const proc_s procs[], const proc_metrics_s metrics[], const size_t len);


/* --- HELPER MACROS --- */

#define PROC_NEW(proc)        ((proc) && (proc)->state == NEW)
#define PROC_READY(proc)      ((proc) && (proc)->state == READY)
#define PROC_RUNNING(proc)    ((proc) && (proc)->state == RUNNING)
#define PROC_HASRUN(proc)     ((proc) && (proc)->last_exec != 0)
#define PROC_COMPLETE(proc)   ((proc) && (proc)->state == COMPLETE)
#define PROC_EXIT(proc)       ((proc) && (proc)->state == EXIT)


/* --- SANITY CHECKS --- */

// Ensures proc_s can be allocted in contiguous blocks
static_assert(sizeof(proc_s) % alignof(proc_s) == 0, "proc_s size must be a multiple of its alignment for safe array indexing");


#endif /* SCHEDULER__PROC__H */

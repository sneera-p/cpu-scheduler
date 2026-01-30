#ifndef __PROC__H
#define __PROC__H

#include <stddef.h>
#include <stdint.h>
#include "config.h"


/*
 * Process Control Block (PCB)
 * ---------------------------
 *
 * Tracks the 
 *    - identity
 *    - requirements
 *    - current state
 */
struct proc
{
   /* --- EXECUTION STATE --- */
   ptime_delta_t remaining_time; // CPU time still needed to finish
   ptimer_t first_run_at;        // Timestamp of the first execution
   ptimer_t last_run_at;         // Timestamp of the most recent execution

   /* --- LOAD STATE --- */
   ptimer_t load_at;         // Timestamp the process was loaded to short-term-scheduler

   /* --- INITIALIZATION DATA --- */
   uint32_t pid;            // Unique Process Identifier
   ptime_delta_t burst_time; // Total CPU time requested
   ptimer_t arrival_time;    // Timestamp when process entered the system (long-term-scheduler)
   priority_e priority;     // Target queue (Q0–Q3) based on task type
};

typedef struct proc proc_s;
typedef proc_s *const restrict PROC_;

extern bool proc_is_complete(PROC_ proc);
extern bool proc_is_started(PROC_ proc);
extern bool proc_is_loaded(PROC_ proc);
extern bool proc_is_dormant(PROC_ proc);
extern ptimer_t proc_completion_time(PROC_ proc);
extern ptime_delta_t proc_turnaround_time(PROC_ proc);
extern ptime_delta_t proc_active_time(PROC_ proc);
extern ptime_delta_t proc_work_done(PROC_ proc);
extern ptime_delta_t proc_wait_time(PROC_ proc);

// Compares 2 processes and returns [ -1: less than, 0: equal, 1: greater than ]
[[nodiscard]] int64_t proc_cmp(const PROC_ proc1, const PROC_ proc2) [[reproducible]];

// Initializes a process struct and returns the time taken for creation
[[nodiscard]] ptimer_t proc_init(PROC_ proc, const ptimer_t timer, const ptime_delta_t burst_time, const priority_e priority) [[reproducible]];

// Loads the process (emulates it) as it moves from dormant to running
[[nodiscard]] ptimer_t proc_load(PROC_ proc, const ptimer_t timer);

// Runs the process for a given duration. Returns the updated global timer.
[[nodiscard]] ptimer_t proc_run(PROC_ proc, const ptimer_t timer, const ptime_delta_t slice_duration);


/* --- SANITY CHECKS --- */

// Ensures proc_s can be allocted in contiguous blocks
static_assert(sizeof(proc_s) % alignof(proc_s) == 0, "proc_s size must be a multiple of its alignment for safe array indexing");


#endif /* __PROC__H */

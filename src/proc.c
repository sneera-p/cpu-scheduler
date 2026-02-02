#include <assert.h>
#include <time.h>
#include <stdio.h>
#include "pcg/pcg_basic.h"
#include "utils/minmax.h"
#include "utils/xmacro.h"
#include "config.h"
#include "proc.h"


/* --- derived properties --- */

[[nodiscard]] inline ms_timer_s proc_completion_time(PROC_ proc) [[reproducible]]
{
   assert(PROC_COMPLETE(proc) || PROC_EXIT(proc));
   return proc->last_exec;
}

[[nodiscard]] inline ms_delta_s proc_turnaround_time(PROC_ proc) [[reproducible]]
{
   assert(PROC_COMPLETE(proc) || PROC_EXIT(proc));
   return (proc->last_exec - proc->arrival_time);
}

[[nodiscard]] inline ms_delta_s proc_active_time(PROC_ proc) [[reproducible]]
{
   assert(PROC_HASRUN(proc));
   return (proc->last_exec - proc->arrival_time);
}

[[nodiscard]] inline ms_delta_s proc_response_time(PROC_ proc) [[reproducible]]
{
   assert(PROC_HASRUN(proc));
   return (proc->first_exec - proc->arrival_time);
}

[[nodiscard]] inline ms_delta_s proc_work_done(PROC_ proc) [[reproducible]]
{
   assert(proc);
   return (proc->cpu_total - proc->cpu_remaining);
}

[[nodiscard]] inline ms_delta_s proc_wait_time(PROC_ proc) [[reproducible]]
{
   return proc_active_time(proc) - proc_work_done(proc);
}



int8_t proc_cmp(const PROC_ proc1, const PROC_ proc2)
{
   assert(proc1);
   assert(proc2);

   if (proc1->cpu_remaining < proc2->cpu_remaining)
      return -1;
   if (proc1->cpu_remaining > proc2->cpu_remaining)
      return 1;

   if (proc1->arrival_time < proc2->arrival_time)
      return -1;
   if (proc1->arrival_time > proc2->arrival_time)
      return 1;

   return 0;
}

void proc_init(PROC_ proc, MS_TIMER_ timer, const priority_e priority)
{
   static uint32_t pid_counter = 1;
   static pcg32_random_t rng;

   if (pid_counter == 1)
      pcg32_srandom_r(&rng, (uint64_t)time(nullptr), (uint64_t)clock());


   assert(proc);
   assert(timer);

   const ms_delta_s cpu_time = 
      ((ms_delta_s)pcg32_boundedrand_r(&rng, (1ul << 27)) + (1ul << 3));
      // | ((ms_delta_s)pcg32_boundedrand_r(&rng, (1ul << 9)) << 32);

   *proc = (proc_s) {
      // execution state
      .cpu_remaining = cpu_time,
      .first_exec = 0,
      .last_exec = 0,
      .state = NEW,
      // initialization state
      .priority = priority,
      .pid = pid_counter,
      .cpu_total = cpu_time,
      .arrival_time = *timer,   
   };


   pid_counter++;
   *timer += TIME_PROC_INIT;
}

void proc_run(PROC_ proc, MS_TIMER_ timer, const ms_delta_s quantum)
{
   assert(PROC_READY(proc) || PROC_RUNNING(proc));
   assert(quantum > 0);

   // process start (first time)
   if (PROC_READY(proc))
   {
      proc->first_exec = *timer;
      proc->state = RUNNING;
   }

   ms_delta_s work_done = min(proc->cpu_remaining, quantum);

   *timer += work_done;

   proc->cpu_remaining -= work_done;
   proc->last_exec = *timer;

   // process exit (termination)
   if (proc->cpu_remaining == 0)
   {
      proc->state = COMPLETE;
      // *timer += TIME_PROC_INIT;
   }
}


void proc_snapshot(const PROC_ proc, MS_TIMER_ timer)
{
   static char buf[256];
   int len = snprintf(buf, sizeof buf, 
      "timer: %12" PRImst "\t"
      "pid: %4u\t"
      " [%s] "
      "cpu_remaining: %10" PRImsd "\t"
      "status: %s\n", 
      *timer, 
      proc->pid, 
      priority_desc[proc->priority], 
      proc->cpu_remaining, 
      proc_state_desc[proc->state]
   );
   fwrite(buf, 1, len, logstream);
}

void proc_display(const PROC_ proc)
{
   static char buf[256];
   int len = snprintf(buf, sizeof(buf), 
      "Process (pid:%u)\n"
      IND "priority: %s\n"
      IND "Status  : %s\n"
      IND "CPU\n"
      IND2 "Work done:       %10" PRImsd "\n"
      IND2 "Response time:   %10" PRImsd "\n"
      IND2 "Wait time:       %10" PRImsd "\n"
      IND2 "Turnaround time: %10" PRImsd "\n"
      IND "Start time:  %10" PRImsd "\n"
      IND "Finish time: %10" PRImsd "\n",
      proc->pid, 
      priority_desc[proc->priority], 
      proc_state_desc[proc->state],
      proc->cpu_total,
      proc_response_time(proc),
      proc_wait_time(proc),
      proc_turnaround_time(proc),
      proc->first_exec,
      proc_completion_time(proc)
   );
   fwrite(buf, 1, len, logstream);
}

#include <assert.h>
#include <time.h>
#include <stdio.h>
#include "pcg/pcg_basic.h"
#include "utils/minmax.h"
#include "utils/xmacro.h"
#include "config.h"
#include "proc.h"


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
      pcg32_srandom_r(&rng, (uint64_t)time(NULL), (uint64_t)clock());


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

ms_delta_s proc_run(PROC_ proc, MS_TIMER_ timer, const ms_delta_s quantum)
{
   assert(proc);
   assert(PROC_READY(proc) || PROC_RUNNING(proc));
   assert(quantum > 0);

   // process start (first time)
   if (PROC_READY(proc))
   {
      proc->first_exec = *timer;
      proc->state = RUNNING;
   }

   const ms_delta_s work_done = min(proc->cpu_remaining, quantum);

   proc->cpu_remaining -= work_done;
   proc->last_exec = *timer + work_done;

   *timer += work_done;

   // process exit (termination)
   if (proc->cpu_remaining == 0)
      proc->state = COMPLETE;
   return work_done;
}


void proc_snapshot(const PROC_ proc, MS_TIMER_ timer)
{
   fprintf(
      logstream,
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
}

void proc_display(const PROC_ proc, const proc_metrics_s metrics)
{
   fprintf(
      logstream,
      "\nProcess (pid:%u)\n"
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
      metrics.response_time,
      metrics.wait_time,
      metrics.turnaround_time,
      proc->first_exec,
      proc->last_exec
   );
}

void proc_print_table(const proc_s procs[], const proc_metrics_s metrics[], const size_t len)
{
   assert(procs);
   assert(metrics);
   assert(len > 0);

   printf(
      "\n\n| %-4s | %-8s | %-16s | %-16s | %-16s | %-16s |\n",
      "proc",
      "priority",
      "work done",
      "response time",
      "wait time",
      "turnaround time"
   );

   for (size_t i = 0; i < len; i++)
   {
      printf(
         "| %4u | %-8s | %16"PRImsd" | %16"PRImsd" | %16"PRImsd" | %16"PRImsd" |\n",
         procs[i].pid,
         priority_desc[procs[i].priority],
         procs[i].cpu_total,
         metrics[i].response_time,
         metrics[i].wait_time,
         metrics[i].turnaround_time
      );
   }
}


proc_metrics_s proc_get_metrics(PROC_ proc)
{
   assert(PROC_COMPLETE(proc) || PROC_EXIT(proc));
   proc_metrics_s m;
   m.pid = proc->pid;
   m.turnaround_time = proc->last_exec - proc->arrival_time;
   m.response_time = proc->first_exec - proc->arrival_time;
   m.wait_time = m.turnaround_time - proc->cpu_total;
   return m;
}

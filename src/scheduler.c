#include <assert.h>
// #include <stdbit.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "input.h"
#include "linear-alloc.h"
#include "scheduler.h"

/*
 * Quick Fix for MVSC(windows) not supporting <stdbit.h> yet...
 * Remove this and uncomment line #2 (#include <stdbit.h>) after support arrives
 */
#ifdef _MSC_VER
   #include <intrin.h>
   #include <stdint.h>
   // MSVC doesn't have stdbit.h yet, so we define the C23 function manually
   static inline int stdc_first_trailing_one_ui(unsigned int value) {
      if (value == 0) return 0; // C23 spec: return 0 if no bits are set
      unsigned long index;
      // _BitScanForward finds the first set bit (1) from LSB to MSB
      _BitScanForward(&index, (unsigned long)value);
      return (int)(index + 1); // C23 uses 1-based indexing for "first" functions
   }

   // Alias it to the generic name if you want to keep your code clean
   #define stdc_first_trailing_one(x) stdc_first_trailing_one_ui(x)
#else
   #include <stdbit.h>
#endif


void scheduler_init(SCHEDULER_ scheduler, LINEAR_ALLOC_ allocator, const size_t nproc, MS_TIMER_ timer)
{
   assert(scheduler);
   assert(allocator);

   #define _queues_(i) (scheduler->queues[i])
   #define _procs_(i) (&scheduler->procs[i])

   for (size_t i = 0; i < N_PRIORITY; i++)
   {
      _queues_(i) = linear_alloc_type(allocator, proc_queue_s, 1);
      void *const buf = linear_alloc_type(allocator, proc_s*, nproc);
      proc_queue_init(_queues_(i), nproc, buf, proc_algo_queue_mode[priority_algo[i]]);
   }

   puts("Enter process priority: ");
   for (size_t i = 0; i < nproc; i++)
   {
      char buf[64];
      snprintf(buf, sizeof(buf), IND "Process (pid:%zu): ", i);

      const priority_e p = input_priority_stdin(buf);
      if (p >= N_PRIORITY)
         exit(EXIT_FAILURE);

      proc_init(_procs_(i), timer, p);
      proc_snapshot(_procs_(i), timer);

      proc_queue_insert(_queues_(p), _procs_(i));
      proc_snapshot(_procs_(i), timer);
   }

   #undef _procs_
   #undef _queues_

   scheduler->nproc = nproc;
}

void scheduler_exit(SCHEDULER_ scheduler)
{
   assert(scheduler);
   for (size_t i = 0; i < N_PRIORITY; i++)
   {
      if (proc_queue_isempty(scheduler->queues[i]))
         continue;
      printf("Warning: Process queue [%s] is not empty @ scheduler exit\n", priority_desc[i]);
   }

   proc_metrics_s metrics[scheduler->nproc];
   proc_queue_metrics_s sums[N_PRIORITY] = {0};
   for (size_t i = 0; i < scheduler->nproc; i++)
   {
      PROC_ proc = &scheduler->procs[i];
      metrics[i] = proc_get_metrics(proc);
      proc_display(proc, metrics[i]);
      sums[proc->priority].work_time += proc->cpu_total;
      sums[proc->priority].turnaround_time += metrics[i].turnaround_time;
      sums[proc->priority].response_time += metrics[i].response_time;
      sums[proc->priority].wait_time += metrics[i].wait_time;
      sums[proc->priority].len++;
   }
   proc_print_table(scheduler->procs, metrics, scheduler->nproc);
   proc_queue_print_table(sums);
}

[[nodiscard]] static inline priority_e _scheduler_next(const uint64_t mask, const priority_e cur) [[unsequenced]]
{
   if (mask == 0)
      return cur;

   typeof(cur) next = (cur + 1) % N_PRIORITY;
   uint64_t higher_bits = mask & (~0ull << next);
   return stdc_first_trailing_one((higher_bits != 0) ? higher_bits : mask) - 1;
}

void scheduler_run(SCHEDULER_ scheduler, MS_TIMER_ timer)
{
   assert(scheduler);
   assert(timer);

   uint64_t queue_mask = 0;
   for (priority_e i = 0; i < N_PRIORITY; i++)
      if (!proc_queue_isempty(scheduler->queues[i]))
         queue_mask |= (1ull << i);

   priority_e i = (priority_e)(stdc_first_trailing_one(queue_mask) - 1);
   while (queue_mask != 0)
   {
      PROC_QUE_ queue = scheduler->queues[i];
      queue_runners[priority_algo[i]](queue, timer);

      if (proc_queue_isempty(queue))
         queue_mask &= ~(1ull << i);

      if (queue_mask == 0)
         break;

      i = _scheduler_next(queue_mask, i);
   }
}

#include <assert.h>
#include <stdbit.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "input.h"
#include "linear-alloc.h"
#include "scheduler.h"


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
   puts("TODO: Statistical analysis");
   for (size_t i = 0; i < scheduler->nproc; i++)
      proc_display(&scheduler->procs[i]);
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

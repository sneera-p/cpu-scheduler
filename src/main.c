#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils/minmax.h"
#include "utils/swap.h"
#include "input.h"
#include "ms-time.h"
#include "config.h"
#include "proc.h"
#include "proc-queue.h"
#include "linear-alloc.h"

const char *proc_state_desc[N_PROC_STATE] = {
   PROC_STATE(X_DESC)
};

const char *priority_desc[N_PRIORITY] = {
   PRIORITY(X_DESC)
};

const proc_algo_e priority_algo[N_PRIORITY] = {
   [Q0] = RR,
   [Q1] = SJF,
   [Q2] = SJF,
   [Q3] = FIFO,
};


void test()
{
   // ms_timer_s a = 3;
   // ms_timer_s b = 4;
   // printf("max: %" PRImst ", min: %" PRImst "\n", max(a, b), min(a, b));

   // printf("a: %" PRImst ", b: %" PRImst "\n", a, b);
   // SWAP(a, b);
   // printf("a: %" PRImst ", b: %" PRImst "\n", a, b);

   // const priority_e p = Q1;
   // printf("%s\n", priority_desc[p]);

   // const proc_state_e s = NEW;
   // printf("%s\n", proc_state_desc[s]);

   printf("proc_s size:%zu alignment:%zu\n", sizeof(proc_s), alignof(proc_s));


   ms_timer_s timer = 1;
   const size_t len = input_size_stdin("Enter no of processes: ");
   LINEAR_ALLOC_ allocator = linear_alloc_create(len * 2 * sizeof(proc_s) + 2 * sizeof(proc_queue_s));

   proc_s *const procs = linear_alloc_type(allocator, proc_s, len);

   for (size_t i = 0; i < len; i++)
   {
      const priority_e priority = input_priority_stdin(IND "Enter process priority: ");
      proc_init(&procs[i], &timer, priority);
      proc_snapshot(&procs[i], &timer);
   }

   proc_queue_s *const queue = linear_alloc_type(allocator, proc_queue_s, 1);
   void *const buf = linear_alloc_type(allocator, proc_s*, len);

   proc_queue_init(queue, len, buf, UNSORTED);
   for (size_t i = 0; i < len; i++)
   {
      proc_queue_insert(queue, &procs[i]);
      proc_snapshot(&procs[i], &timer);
   }

   while (!proc_queue_isempty(queue))
   {
      PROC_ p = proc_queue_peek(queue);
      while (!PROC_COMPLETE(p))
      {
         proc_run(p, &timer, TIME_QUANTUM * 100);
         proc_snapshot(p, &timer);
      }
      proc_queue_remove(queue);
      proc_display(p);
   }

   linear_alloc_reset(allocator);
   linear_alloc_delete(allocator);


   // const priority_e p = input_priority_stdin(IND "Enter process 1 priority: ");
   // printf("%s\n", priority_desc[p]);

}

FILE *logstream = nullptr;

int main(int argc, char *argv[])
{
   logstream = (argc < 2) ? stdout : fopen(argv[1], "w");
   test();
   fclose(logstream);
   return 0;
}
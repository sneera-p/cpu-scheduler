#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils/minmax.h"
#include "utils/swap.h"
#include "input.h"
#include "ms-time.h"
#include "config.h"
#include "proc.h"
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
   proc_s proc[2];

   proc_init(&proc[0], &timer, Q0);
   proc_snapshot(&proc[0], &timer);

   proc_init(&proc[1], &timer, Q0);
   proc_snapshot(&proc[1], &timer);

   proc[0].state = READY;
   proc[1].state = READY;
   
   while (!PROC_EXIT(&proc[0]))
   {   
      proc_run(&proc[0], &timer, TIME_QUANTUM * 100);
      proc_snapshot(&proc[0], &timer);
   }

   while (!PROC_EXIT(&proc[1]))
   {   
      proc_run(&proc[1], &timer, TIME_QUANTUM * 100);
      proc_snapshot(&proc[1], &timer);
   }

   proc_display(&proc[0]);
   proc_display(&proc[1]);


   // const size_t len = input_size_stdin("Enter no of processes: ");
   // LINEAR_ALLOC_ allocator = linear_alloc_create(len * 2 * sizeof(size_t));

   // // int *arr = linear_alloc(allocator, alignof(int), sizeof(int) * 5);
   // size_t *arr = linear_alloc_type(allocator, size_t, len);
   // for (size_t i = 0; i < len; i++)
   //    arr[i] = i + 1;
   // for (size_t i = 0; i < len; i++)
   //    printf("%zu\n", arr[i]);

   // linear_alloc_delete(allocator);


   // const priority_e p = input_priority_stdin(IND "Enter process 1 priority: ");
   // printf("%s\n", priority_desc[p]);
}

int main()
{
   test();
   return 0;
}
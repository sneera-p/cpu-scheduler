#include <stdio.h>
#include <stdlib.h>
#include "utils/minmax.h"
#include "utils/swap.h"
#include "ms-time.h"
#include "config.h"
#include "proc.h"


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
   proc_display(&proc[0], &timer);

   proc_init(&proc[1], &timer, Q0);
   proc_display(&proc[1], &timer);

   proc[0].state = READY;
   proc[1].state = READY;
   
   while (!PROC_EXIT(&proc[0]))
   {   
      proc_run(&proc[0], &timer, TIME_QUANTUM * 100);
      proc_display(&proc[0], &timer);
   }

   while (!PROC_EXIT(&proc[1]))
   {   
      proc_run(&proc[1], &timer, TIME_QUANTUM * 100);
      proc_display(&proc[1], &timer);
   }
}

int main()
{
   test();
   return 0;
}
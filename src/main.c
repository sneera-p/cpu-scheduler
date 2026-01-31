#include <stdio.h>
#include "utils/minmax.h"
#include "utils/swap.h"
#include "ms-time.h"
#include "config.h"


int main()
{
   ms_timer_s a = 3;
   ms_timer_s b = 4;
   printf("max: %" PRImst ", min: %" PRImst "\n", max(a, b), min(a, b));

   printf("a: %" PRImst ", b: %" PRImst "\n", a, b);
   SWAP(a, b);
   printf("a: %" PRImst ", b: %" PRImst "\n", a, b);

   const priority_e p = Q1;
   printf("%s\n", priority_desc[p]);

   const proc_state_e s = NEW;
   printf("%s\n", proc_state_desc[s]);

   return 0;
}
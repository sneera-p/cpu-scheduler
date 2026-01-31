#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include "utils/minmax.h"
#include "utils/swap.h"

typedef uint64_t ms_timer_s;
#define PRImstimer PRIu64

int main()
{
   ms_timer_s a = 3;
   ms_timer_s b = 4;
   printf("max: %" PRImstimer ", min: %" PRImstimer "\n", max(a, b), min(a, b));

   printf("a: %" PRImstimer ", b: %" PRImstimer "\n", a, b);
   SWAP(a, b);
   printf("a: %" PRImstimer ", b: %" PRImstimer "\n", a, b);

   return 0;
}
#ifndef SCHEDULER__SCHEDULER__H
#define SCHEDULER__SCHEDULER__H

#include <stddef.h>
#include "proc-queue.h"
#include "config.h"


struct scheduler
{
   proc_queue_s *queues[N_PRIORITY];
   size_t nproc;
   proc_s procs[];
};

typedef struct scheduler scheduler_s;
typedef scheduler_s *const restrict SCHEDULER_;

void scheduler_init(SCHEDULER_ scheduler, LINEAR_ALLOC_ allocator, const size_t nproc, MS_TIMER_ timer);
void scheduler_exit(SCHEDULER_ scheduler);
void scheduler_run(SCHEDULER_ scheduler, MS_TIMER_ timer);

#endif /* SCHEDULER__SCHEDULER__H */

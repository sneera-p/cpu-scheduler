#ifndef __SCHEDULER__H
#define __SCHEDULER__H

#include <stddef.h>
#include "utils.h"
#include "config.h"
#include "proc-queue.h"


struct scheduler
{
   ptimer_t timer;
   proc_queue_s *queues[N_PRIORITY];
   size_t nproc;
   proc_s procs[];
};

typedef struct scheduler scheduler_s;
typedef scheduler_s *const restrict SCHEDULER_;

// Allocate
[[nodiscard]] scheduler_s *scheduler_alloc(const size_t nproc);
void scheduler_free(SCHEDULER_ scheduler);

// Init
void scheduler_init(SCHEDULER_ scheduler, const size_t nproc, const toml_datum_t *const toml_procs);
void scheduler_clear(SCHEDULER_ scheduler);
void scheduler_run(SCHEDULER_ scheduler);


#endif /* __SCHEDULER__H */
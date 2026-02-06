#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "utils/minmax.h"
#include "utils/swap.h"
#include "input.h"
#include "ms-time.h"
#include "config.h"
#include "proc.h"
#include "proc-queue.h"
#include "linear-alloc.h"
#include "scheduler.h"

FILE *logstream = NULL;

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

const bool proc_algo_queue_mode[N_PROC_ALGO] = {
   [RR] = UNSORTED,
   [SJF] = SORTED,
   [FIFO] = UNSORTED,
};


[[noreturn]] void sigint_cb(int revents);
[[noreturn]] void sigterm_cb(int revents);
[[nodiscard]] size_t __mem_size(size_t n) [[unsequenced]];

int main(int argc, char *argv[])
{
   signal(SIGINT, sigint_cb);
   signal(SIGTERM, sigterm_cb);

   logstream = (argc < 2) ? stdout : fopen(argv[1], "w");
   if (!logstream)
      logstream = stdout;

   const size_t len = input_size_stdin("Enter no of processes: ");
   if (len == 0)
      exit(EXIT_FAILURE);

   LINEAR_ALLOC_ allocator = linear_alloc_create(__mem_size(len));
   if (!allocator)
      exit(EXIT_FAILURE);

   ms_timer_s timer = 1;
   SCHEDULER_ scheduler = linear_alloc(allocator, alignof(scheduler_s), sizeof(scheduler_s) + len * sizeof(proc_s));
   scheduler_init(scheduler, allocator, len, &timer);
   scheduler_run(scheduler, &timer);
   scheduler_exit(scheduler);

   linear_alloc_delete(allocator);
   if (logstream != stdout)
      fclose(logstream);

   return 0;
}

void sigint_cb(int /* revents */)
{
   puts("\nScheduler interupted\nTerminating...");
   exit(EXIT_FAILURE);
}

void sigterm_cb(int /* revents */)
{
   puts("Scheduler Terminating...");
   exit(EXIT_FAILURE);
}

size_t __mem_size(size_t n)
{
   return (
      sizeof(scheduler_s) * 2
      + sizeof(proc_s) * ((n) + 10)
      + sizeof(proc_s*) * ((n) * (N_PRIORITY + 4))
      + sizeof(proc_queue_s) * (N_PRIORITY + 4)
      + sizeof(proc_queue_s*) * (N_PRIORITY + 4)
   );
}

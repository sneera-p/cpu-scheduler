#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "scheduler.h"

[[nodiscard]] static priority_e parse_priority(char *restrict s) [[reproducible]];
static void parse_procs_stdin(SCHEDULER_ scheduler);
static void parse_procs_toml(SCHEDULER_ scheduler, const toml_datum_t *const toml_procs);

static ptime_delta_t run_rr(PROC_QUE_ queue, ptimer_t timer);
static ptime_delta_t run_sjf(PROC_QUE_ queue, ptimer_t timer);
static ptime_delta_t run_fifo(PROC_QUE_ queue, ptimer_t timer);


scheduler_s *scheduler_alloc(const size_t nproc)
{
   assert(nproc > 0);
   const size_t size = sizeof(scheduler_s) + sizeof(proc_s) * nproc;

   void *tmp = malloc(size);
   if (tmp == nullptr)
      exit(EXIT_FAILURE);

	return tmp;
}

void scheduler_free(SCHEDULER_ scheduler)
{
   assert(scheduler);
   free(scheduler);
}

static priority_e parse_priority(char *restrict s)
{
   assert(s);
   while(' ' == *s || '\t' == *s)
      s++; // skip whitespace / tabs
   
   priority_e v = 0;
   for (; *s >= '0' && *s <= '9'; s++)
      v = v * 10 + (*s - '0');

   if (v < N_PRIORITY)
      return v;

   return N_PRIORITY;
}

static void parse_procs_stdin(SCHEDULER_ scheduler)
{
	assert(scheduler);
	char buf[256];
	buf[0] = '\0';

	#define _timer_ (scheduler->timer)
	#define _procs_(e) (scheduler->procs[e])
	#define _queue_(e) (scheduler->queues[priority])

	fputs("Enter process priority (0..3):\n", stdout);
	for (size_t i = 0; i < scheduler->nproc; i++)
	{
		priority_e priority = N_PRIORITY;
		while (N_PRIORITY >= priority)
		{
			printf(IND "Process %zu: ", i);
			if (nullptr == fgets(buf, sizeof(buf), stdin))
			{
				fputs("Error: read failed\n", stderr);
				exit(EXIT_FAILURE);
			}

			priority = parse_priority(buf);
		}

		_timer_ += proc_init(&_procs_(i), _timer_, priority);

		if (proc_queue_isfull(_queue_(priority)))
			_queue_(priority) = proc_queue_grow(_queue_(priority));
		proc_queue_insert(_queue_(priority), &_procs_(i));

		_procs_(i).state = READY;
	}

	#undef _queue_
	#undef _procs_
	#undef _timer_
}

static void parse_procs_toml(SCHEDULER_ scheduler, const toml_datum_t *const toml_procs)
{
	assert(scheduler);
	assert(toml_procs);
	assert(toml_procs->type == TOML_ARRAY);

	#define _timer_ (scheduler->timer)
	#define _procs_(e) (scheduler->procs[e])
	#define _queue_(e) (scheduler->queues[priority])

	for (size_t i = 0; i < scheduler->nproc; i++)
	{
		toml_datum_t elem = toml_procs->u.arr.elem[i];
		assert(elem.type != TOML_INT64);

		const priority_e priority = elem.u.int64;
		assert(priority < N_PRIORITY);

		_timer_ += proc_init(&_procs_(i), _timer_, priority);

		if (proc_queue_isfull(_queue_(priority)))
			_queue_(priority) = proc_queue_grow(_queue_(priority));
		proc_queue_insert(_queue_(priority), &_procs_(i));

		_procs_(i).state = READY;
	}

	#undef _queue_
	#undef _procs_
	#undef _timer_
}


void scheduler_init(SCHEDULER_ scheduler, const size_t nproc, const toml_datum_t *const toml_procs)
{
   assert(scheduler);

   const size_t queue_cap = nproc / 4 + 1;

   scheduler->timer = 1;
   scheduler->nproc = nproc;

   for (uint8_t i = 0; i < N_PRIORITY; i++)
	{
		scheduler->queues[i] = proc_queue_alloc(queue_cap);

		if (i == Q1 || i == Q2)
			proc_queue_init(scheduler->queues[i], queue_cap, SORTED_LINEAR);
		else /* (i == Q0 || i == Q3) */
			proc_queue_init(scheduler->queues[i], queue_cap, UNSORTED_CIRCULAR);
	}

   if (nullptr == toml_procs)
   	parse_procs_stdin(scheduler);
   else
   	parse_procs_toml(scheduler, toml_procs);
}

void scheduler_clear(SCHEDULER_ scheduler)
{
	assert(scheduler);
   for (uint8_t i = 0; i < N_PRIORITY; i++)
		proc_queue_free(scheduler->queues[i]);
}



static ptime_delta_t run_rr(PROC_QUE_ queue, ptimer_t timer)
{
	assert(!proc_queue_isempty(queue));

   ptime_delta_t total_work = 0;
   ptime_delta_t turn_quota = TIME_QUANTUM;

   while (!proc_queue_isempty(queue) && turn_quota > 0)
   {
      PROC_ proc = proc_queue_peek(queue);
      // RR uses a smaller internal quantum
      ptime_delta_t slice = min(turn_quota, Q0_TIME_QUANTUM);
      ptime_delta_t work = proc_run(proc, timer + total_work, slice);
		printf("Process %u [%s] ran for %ums\n", proc->pid, priority_desc[proc->priority], work);

      total_work += work;
      turn_quota -= work;

      if (proc->state == EXIT)
      {
         proc_queue_remove_head(queue);
      }
      else
      {
         proc_queue_rotate(queue);
         break; // Q0 gives up the CPU after its internal quantum
      }
   }
   return total_work;
}

static ptime_delta_t run_sjf(PROC_QUE_ queue, ptimer_t timer)
{
	assert(!proc_queue_isempty(queue));

	ptime_delta_t slice_duration = TIME_QUANTUM;
	while (!proc_queue_isempty(queue) && slice_duration > 0)
	{
		PROC_ proc = proc_queue_peek(queue);

		ptime_delta_t t = proc_run(proc, timer, slice_duration);
		printf("Process %u [%s] ran for %ums\n", proc->pid, priority_desc[proc->priority], t);
		slice_duration -= t;

		if (EXIT != proc->state)
			break;
		else
			proc_queue_remove_sorted(queue);
	}

	return slice_duration;
}

static ptime_delta_t run_fifo(PROC_QUE_ queue, ptimer_t timer)
{
	assert(!proc_queue_isempty(queue));

	ptime_delta_t slice_duration = TIME_QUANTUM;
	while (!proc_queue_isempty(queue) && slice_duration > 0)
	{
		PROC_ proc = proc_queue_peek(queue);

		ptime_delta_t t = proc_run(proc, timer, slice_duration);
		printf("Process %u [%s] ran for %ums\n", proc->pid, priority_desc[proc->priority], t);
		slice_duration -= t;

		if (EXIT != proc->state)
			break;
		else
			proc_queue_remove_head(queue);
	}

	return slice_duration;
}


void scheduler_run(SCHEDULER_ scheduler)
{
	assert(scheduler);
    
   // Jump table for algorithms
   typedef ptime_delta_t (*sched_algo_f)(PROC_QUE_, ptimer_t);
   static const sched_algo_f run_algo[N_PRIORITY] = {
      run_rr, run_sjf, run_sjf, run_fifo
   };

   priority_e queue_cur = Q0;
   while (true) 
   {
      // 1. Find the next non-empty queue (State Transition)
      bool found = false;
      for (uint8_t i = 0; i < N_PRIORITY; i++)
      {
         priority_e idx = (queue_cur + i) % N_PRIORITY;
         if (!proc_queue_isempty(scheduler->queues[idx]))
         {
            queue_cur = idx;
            found = true;
            break;
         }
      }

      if (!found) break; // All queues empty, simulation finished

      // 2. Execute the state's algorithm
      PROC_QUE_ queue = scheduler->queues[queue_cur];
        
      // Add context switch overhead before running
      scheduler->timer += TIME_PROC_SWITCH; 
        
      // Execute and advance global time
      ptime_delta_t work_done = run_algo[queue_cur](queue, scheduler->timer);
      scheduler->timer += work_done;

      // 3. Move cursor to next queue for Round Robin behavior between levels
      queue_cur = (queue_cur + 1) % N_PRIORITY;
   }
}

#include <assert.h>
#include <stdlib.h>
#include "pcg/pcg_basic.h"
#include "utils.h"
#include "proc.h"

[[nodiscard]] inline ptimer_t proc_completion_time(PROC_ proc) [[reproducible]]
{
	assert(proc && proc->state == EXIT);
	return proc->last_exec;
}

[[nodiscard]] inline ptime_delta_t proc_turnaround_time(PROC_ proc) [[reproducible]]
{
	assert(proc && proc->state == EXIT);
	return (proc->last_exec - proc->arrival_time);
}

[[nodiscard]] inline ptime_delta_t proc_active_time(PROC_ proc) [[reproducible]]
{
	assert(proc && proc->last_exec != 0);
	return (proc->last_exec - proc->arrival_time);
}

[[nodiscard]] inline ptime_delta_t proc_response_time(PROC_ proc) [[reproducible]]
{
	assert(proc && proc->last_exec != 0);
    return (proc->first_exec - proc->arrival_time);
}

[[nodiscard]] inline ptime_delta_t proc_work_done(PROC_ proc) [[reproducible]]
{
	assert(proc);
	return (proc->cpu_total - proc->cpu_remaining);
}

[[nodiscard]] inline ptime_delta_t proc_wait_time(PROC_ proc) [[reproducible]]
{
	return proc_active_time(proc) - proc_work_done(proc);
}


int64_t proc_cmp(const PROC_ proc1, const PROC_ proc2)
{
	assert(proc1);
	assert(proc2);

	if (proc1->cpu_remaining != proc2->cpu_remaining)
		return proc1->cpu_remaining - proc2->cpu_remaining;
	else
		return proc1->arrival_time - proc2->arrival_time;
}

ptime_delta_t proc_init(PROC_ proc, const ptimer_t timer, const priority_e priority)
{
	static uint32_t pid_counter = 1;
	static pcg32_random_t proc_rng;

	// Seed RNG on first call
	if (pid_counter == 1)
		pcg32_srandom_r(&proc_rng, (uint64_t)&pid_counter, (uint64_t)&proc_rng);

	assert(proc);
	uint32_t cpu_time = pcg32_boundedrand_r(&proc_rng, (1ul << 27)) + (1ul << 7);

	*proc = (proc_s) {
   	// execution state
		.cpu_remaining = cpu_time,
		.first_exec = 0,
		.last_exec = 0,
		.state = NEW,
   	// initialization state
		.priority = priority,
		.pid = pid_counter,
		.cpu_total = cpu_time,
		.arrival_time = timer,
	};

	pid_counter++;
	return TIME_PROC_INIT;
}


ptime_delta_t proc_run(PROC_ proc, const ptimer_t timer, const ptime_delta_t slice_duration)
{
	assert(proc && proc->state == READY);
	assert(slice_duration > 0);

	// the very first time process runs
   if (0 == proc->first_exec)
      proc->first_exec = timer;

	ptime_delta_t work_done = min(proc->cpu_remaining, slice_duration);

	proc->cpu_remaining -= work_done;
	proc->last_exec = timer + work_done;

	// process exit (termination)
	if (0 == proc->cpu_remaining)
		proc->state = EXIT;

	return work_done;
}

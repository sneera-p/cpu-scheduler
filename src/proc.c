#include <assert.h>
#include <stdlib.h>
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

ptimer_t proc_init(PROC_ proc, const ptimer_t timer, const ptime_delta_t cpu_total, const priority_e priority)
{
	static uint32_t pid_counter = 1;

	assert(proc);
	assert(cpu_total > 0);

	*proc = (proc_s) {
		.cpu_remaining = cpu_total,
		.first_exec = 0,
		.last_exec = 0,
		.state = NEW,
		.priority = priority,
		.pid = pid_counter,
		.cpu_total = cpu_total,
		.arrival_time = timer,
	};

	pid_counter++;
	return timer + TIME_PROC_INIT;
}


ptimer_t proc_run(PROC_ proc, const ptimer_t timer, const ptime_delta_t slice_duration)
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

	return proc->last_exec;
}


// #ifndef __TOML__IMPL
// 	#define __TOML__IMPL
// 	#include <toml-c.h>
// #endif /* __TOML__IMPL */

// [[nodiscard]] static inline size_t parse_ulen(char *restrict s) [[reproducible]]
// {
// 	assert(s);
// 	while(' ' == *s || '\t' == *s)
// 		s++; // skip whitespace / tabs

// 	size_t v = 0;
//    for (; *s >= '0' && *s <= '9'; s++)
//       v = v * 10 + (*s - '0');

//    if (0 == v)
//    	strcpy(s, "Error: Length must be a number greater than 0\n");
//    return v;
// }

// [[nodiscard]] static inline priority_e parse_priority(char *restrict s) [[reproducible]]
// {
// 	assert(s);
// 	while(' ' == *s || '\t' == *s)
// 		s++; // skip whitespace / tabs
   
// 	priority_e v = 0;
//    for (; *s >= '0' && *s <= '9'; s++)
//       v = v * 10 + (*s - '0');

//    if (v < N_PRIORITY)
//    	return v;

//    strcpy(s, "Error: Invalid priority\n");
//    return N_PRIORITY;
// }

// proc_arr_s parse_procs_stdin(void)
// {
// 	proc_s *restrict procs = nullptr;
// 	char buf[256];
// 	buf[0] = '\0';

// 	fputs("Enter process count: ", stdout);
// 	if (nullptr == fgets(buf, sizeof(buf), stdin))
// 		goto read_fail;

// 	const size_t len = parse_ulen(buf);
// 	if (0 == len)
// 		goto return_empty;

// 	procs = (proc_s*)aligned_alloc(alignof(proc_s), sizeof(proc_s) * len);

// 	fputs("Enter process data:\n", stdout);
// 	for (size_t i = 0; i < len; i++)
// 	{
// 		printf(IND "Process %zu:\n", i);

// 		uint32_t time = 0;
// 		while (0 == time)
// 		{
// 			fputs(IND2 "Burst time: ", stdout);
// 			if (nullptr == fgets(buf, sizeof(buf), stdin))
// 				goto read_fail;
// 			time = parse_ulen(buf);
// 		}

// 		priority_e priority = N_PRIORITY;
// 		while (N_PRIORITY == priority)
// 		{
// 			fputs(IND2 "Priority: ", stdout);
// 			if (nullptr == fgets(buf, sizeof(buf), stdin))
// 				goto read_fail;
// 			priority = parse_priority(buf);
// 		}

// 		TIMER_ t = proc_init(&procs[i], i * TIME_PROC_INIT, time, priority);
// 		assert(t == (i + 1) * TIME_PROC_INIT);
// 	}
// 	return proc_arr_create(procs, len);

// read_fail:
// 	fputs("Error: file read failed\n", stderr);
// 	if (nullptr != procs)
// 		free(procs);	
// return_empty:
// 	fputs(buf, stderr);
// 	return proc_arr_create(nullptr, 0);
// }


// proc_arr_s parse_procs_toml(FILE *const restrict file)
// {
// 	assert(file);
// 	char errbuf[256];
// 	errbuf[0] = '\0';

// 	toml_table_t *const table = toml_parse_file(file, errbuf, sizeof(errbuf));
// 	if (nullptr == table)
// 		goto return_empty;

// 	const toml_array_t *const toml_procs = toml_table_array(table, "tasks");
// 	if (nullptr == toml_procs)
// 		goto no_tasks;

// 	const size_t len = toml_array_len(toml_procs);
// 	proc_s *const restrict procs = (proc_s*)aligned_alloc(alignof(proc_s), sizeof(proc_s) * len);

// 	for (size_t i = 0; i < len; ++i)
// 	{
// 		const toml_table_t *const toml_proc = toml_array_table(toml_procs, i);
// 		if (nullptr == toml_proc)
// 			continue;

// 		toml_value_t v_prio = toml_table_int(toml_proc, "priority");
// 		toml_value_t v_time = toml_table_int(toml_proc, "time");

// 		if (!v_prio.ok || !v_time.ok)
// 		   continue; 

// 		TIMER_ t = proc_init(&procs[i], i * TIME_PROC_INIT, (uint32_t)v_time.u.i, (priority_e)v_prio.u.i);
// 		assert(t == (i + 1) * TIME_PROC_INIT);
// 	}

// 	toml_free(table);
// 	return proc_arr_create(procs, len);

// no_tasks:
// 	strcpy(errbuf, "No tasks available\n");
// return_empty:
// 	fputs(errbuf, stderr);
// 	toml_free(table);
// 	return proc_arr_create(nullptr, 0);
// }
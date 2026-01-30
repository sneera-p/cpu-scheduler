#include <assert.h>
#include <stdlib.h>
#include "utils.h"
#include "proc.h"

[[nodiscard]] inline bool proc_is_complete(PROC_ proc) [[reproducible]]
{
	assert(proc);
	return (0 == proc->remaining_time);
}

[[nodiscard]] inline bool proc_is_started(PROC_ proc) [[reproducible]]
{
	assert(proc);
	return (0 != proc->first_run_at);
}

[[nodiscard]] inline bool proc_is_loaded(PROC_ proc) [[reproducible]]
{
	assert(proc);
	return (0 != proc->load_at);
}

[[nodiscard]] inline bool proc_is_dormant(PROC_ proc) [[reproducible]]
{
	assert(proc);
	return ((0 == proc->first_run_at) && (0 == proc->load_at));
}

[[nodiscard]] inline ptimer_t proc_completion_time(PROC_ proc) [[reproducible]]
{
	assert(proc_is_complete(proc));
	return proc->last_run_at;
}

[[nodiscard]] inline ptime_delta_t proc_turnaround_time(PROC_ proc) [[reproducible]]
{
	assert(proc_is_complete(proc));
	return (proc->last_run_at - proc->arrival_time);
}

[[nodiscard]] inline ptime_delta_t proc_active_time(PROC_ proc) [[reproducible]]
{
	assert(proc_is_started(proc));
	return (proc->last_run_at - proc->arrival_time);
}

[[nodiscard]] inline ptime_delta_t proc_response_time(PROC_ proc) [[reproducible]]
{
	assert(proc_is_started(proc));
    return (proc->first_run_at - proc->arrival_time);
}

[[nodiscard]] inline ptime_delta_t proc_work_done(PROC_ proc) [[reproducible]]
{
	assert(proc);
	return (proc->burst_time - proc->remaining_time);
}

[[nodiscard]] inline ptime_delta_t proc_wait_time(PROC_ proc) [[reproducible]]
{
	return proc_active_time(proc) - proc_work_done(proc);
}


int64_t proc_cmp(const PROC_ proc1, const PROC_ proc2)
{
	assert(proc1);
	assert(proc2);

	if (proc1->remaining_time != proc2->remaining_time)
		return proc1->remaining_time - proc2->remaining_time;
	else
		return proc1->arrival_time - proc2->arrival_time;
}

ptimer_t proc_init(PROC_ proc, const ptimer_t timer, const ptime_delta_t burst_time, const priority_e priority)
{
	static uint32_t pid_counter = 1;

	assert(proc);
	assert(burst_time > 0);

	*proc = (proc_s) {
		.pid = pid_counter,
		.burst_time = burst_time,
		.arrival_time = timer,
		.priority = priority,
		.load_at = 0,
		.remaining_time = burst_time,
		.first_run_at = 0,
		.last_run_at = 0,
	};

	pid_counter++;
	return timer + TIME_PROC_INIT;
}

ptimer_t proc_load(PROC_ proc, const ptimer_t timer)
{
	assert(proc_is_dormant(proc));
	proc->load_at = timer;
	return timer + TIME_PROC_LOAD;
}


ptimer_t proc_run(PROC_ proc, const ptimer_t timer, const ptime_delta_t slice_duration)
{
	assert(proc_is_loaded(proc));
	assert(slice_duration > 0);

	// the very first time process runs
   if (0 == proc->first_run_at)
      proc->first_run_at = timer;

	ptime_delta_t work_done = min(proc->remaining_time, slice_duration);

	proc->remaining_time -= work_done;
	proc->last_run_at = timer + work_done;

	return proc->last_run_at;
}

// #define IND "   "
// #define IND2 IND IND

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
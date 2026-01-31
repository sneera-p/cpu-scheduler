#include <assert.h>
#include <stdlib.h>
#include "utils.h"
#include "proc-queue.h"


proc_queue_s *proc_queue_alloc(const uint32_t cap)
{
	assert(cap > 0);
	const size_t size = sizeof(proc_queue_s) + sizeof(proc_s*) * cap;

	void *tmp = malloc(size);
	if (tmp == nullptr)
		exit(EXIT_FAILURE);

	return tmp;
}

proc_queue_s *proc_queue_grow(PROC_QUE_ queue)
{
	assert(queue);

	const size_t cap = queue->cap * 3 / 2;
	const size_t size = sizeof(proc_queue_s) + sizeof(proc_s*) * cap;

	PROC_QUE_ tmp = realloc(queue, size);
	if (tmp == nullptr)
		exit(EXIT_FAILURE);

	tmp->cap = cap;
	return tmp;
}

void proc_queue_free(PROC_QUE_ queue)
{
	assert(queue);
	free(queue);
}


void proc_queue_init(PROC_QUE_ queue, const size_t cap, const proc_queue_mode_e mode)
{
	assert(queue);
	*queue = (proc_queue_s) {
		.head = 0,
		.len = 0,
		.cap = cap,
		.mode = mode,
		// No assignment to .data needed, it's part of the struct allocation
	};
}


bool proc_queue_isempty(PROC_QUE_ queue)
{
	assert(queue);
	return (0 == queue->len);
}

bool proc_queue_isfull(PROC_QUE_ queue)
{
	assert(queue);
	return (queue->cap == queue->len);
}


void proc_queue_bubble_up(PROC_QUE_ queue)
{
	assert(queue);
	assert(SORTED_LINEAR == queue->mode);

	if (queue->len <= 1)
		return; 

	uint32_t index = queue->len - 1;
	while (index > 0)
	{
		uint32_t parent = (index - 1) / 2;
		if (0 <= proc_cmp(queue->data[index], queue->data[parent]))
			break;

		_swap(queue->data[index], queue->data[parent]);
		index = parent;
	}
}

void proc_queue_bubble_down(PROC_QUE_ queue)
{
	assert(queue);
	assert(SORTED_LINEAR == queue->mode);

	if (queue->len <= 1)
		return;

	#define valid_i(i) ((i) < queue->len)

	uint32_t index = 0;
	while (true)
	{
		uint32_t left = index * 2 + 1;
		uint32_t right = index * 2 + 2;
		uint32_t min = index;

		if (valid_i(left) && (0 > proc_cmp(queue->data[left], queue->data[min])))
			min = left;

		if (valid_i(right) && (0 > proc_cmp(queue->data[right], queue->data[min])))
			min = right;

		if (min == index)
			break;

		_swap(queue->data[min], queue->data[index]);
		index = min;
	}

	#undef valid_i
}

void proc_queue_insert_back(PROC_QUE_ queue, PROC_ proc)
{
	assert(!proc_queue_isfull(queue));
	assert(proc);
	const uint32_t pos = (queue->head + queue->len) % queue->cap;
	queue->data[pos] = proc;
	queue->len++;
}

void proc_queue_insert_sorted(PROC_QUE_ queue, PROC_ proc)
{
	assert(!proc_queue_isfull(queue));
	assert(proc);
	assert(0 == queue->head);
	queue->data[queue->len] = proc;
	queue->len++;
	proc_queue_bubble_up(queue);
}


void proc_queue_insert(PROC_QUE_ queue, PROC_ proc)
{
	assert(queue);
	switch (queue->mode)
	{
		case SORTED_LINEAR:
			proc_queue_insert_sorted(queue, proc);
			break;

		case UNSORTED_CIRCULAR:
			proc_queue_insert_back(queue, proc);
			break;

		default:
			unreachable();
	}
}

void proc_queue_remove_head(PROC_QUE_ queue)
{
	assert(!proc_queue_isempty(queue));
	queue->head = (queue->head + 1) % queue->cap;
	queue->len--;
}

void proc_queue_remove_sorted(PROC_QUE_ queue)
{
	assert(!proc_queue_isempty(queue));
	assert(0 == queue->head);
	_swap(queue->data[0], queue->data[queue->len - 1]);
	queue->len--;
	proc_queue_bubble_down(queue);
}

void proc_queue_remove(PROC_QUE_ queue)
{
	assert(queue);
	switch (queue->mode)
	{
		case SORTED_LINEAR:
			proc_queue_remove_sorted(queue);
			break;

		case UNSORTED_CIRCULAR:
			proc_queue_remove_head(queue);
			break;

		default:
			unreachable();
	}
}


proc_s *proc_queue_peek(PROC_QUE_ queue)
{
	assert(!proc_queue_isempty(queue));
	return queue->data[queue->head];
}

void proc_queue_rotate(PROC_QUE_ queue)
{
   assert(queue->mode == UNSORTED_CIRCULAR);
   
   if (queue->len <= 1) 
   	return;

   PROC_ tmp = proc_queue_peek(queue);
   proc_queue_remove(queue);
   proc_queue_insert(queue, tmp);
}

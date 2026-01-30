#ifndef __PROC_QUEUE__H
#define __PROC_QUEUE__H

#include <stddef.h>
#include <stdint.h>
#include "proc.h"

enum proc_queue_mode : uint8_t
{
	SORTED_LINEAR, // for SJF
	UNSORTED_CIRCULAR, // for RR, FIFO
};

typedef enum proc_queue_mode proc_queue_mode_e;


struct proc_queue
{
	uint32_t head; // Circular queue head
	uint32_t len;
	uint32_t cap;
	proc_queue_mode_e mode;
	proc_s *data[];
};

typedef struct proc_queue proc_queue_s;
typedef proc_queue_s *const restrict PROC_QUE_;

// Allocation
[[nodiscard]] proc_queue_s *proc_queue_alloc(const uint32_t cap);
void proc_queue_free(PROC_QUE_ queue);

// Initialises a process queue
void proc_queue_init(PROC_QUE_ queue, const size_t cap, const proc_queue_mode_e mode) [[reproducible]];

[[nodiscard]] bool proc_queue_isempty(PROC_QUE_ queue) [[reproducible]];
[[nodiscard]] bool proc_queue_isfull(PROC_QUE_ queue) [[reproducible]];

// minheap-like behaivor (for SJF)
void proc_queue_bubble_up(PROC_QUE_ queue) [[reproducible]];
void proc_queue_bubble_down(PROC_QUE_ queue) [[reproducible]];

// queue insert/remove
void proc_queue_insert(PROC_QUE_ queue, PROC_ proc);
void proc_queue_remove(PROC_QUE_ queue);

// view
[[nodiscard]] proc_s *proc_queue_peek(PROC_QUE_ queue) [[reproducible]];


#endif /* __PROC_QUEUE__H */
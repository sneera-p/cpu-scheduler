#ifndef SCHEDULER__PROC_QUEUE__H
#define SCHEDULER__PROC_QUEUE__H

#include <stddef.h>
#include "proc.h"

#define SORTED (true)
#define UNSORTED (false)


/*
 * proc_queues_s
 * --------------
 * 
 * Process Queue Structure
 * The queue has 2 modes, which is set at creation and is fixed
 * 
 * UNSORTED (circular queue): 
 *    - front: index of first (earliest) inserted element
 *    - FIFO,
 *    - O(n) insert / remove
 *
 * SORTED (min-heap):
 *    - front: arbitary value guaranteed (front > cap)
 *    - ordered by 'proc_cmp()' function
 *    - O(log(n)) insert / remove
 */
struct proc_queue
{
   size_t cap;
   size_t len;
   size_t front; // (front > cap mean queue is sorted)
   proc_s **data;
};

typedef struct proc_queue proc_queue_s;
typedef proc_queue_s *const restrict PROC_QUE_;

extern bool proc_queue_isempty(PROC_QUE_ queue);
extern bool proc_queue_isfull(PROC_QUE_ queue);
extern bool proc_queue_issorted(PROC_QUE_ queue);

// Initialise queue
void proc_queue_init(PROC_QUE_ queue, const size_t cap, void *const buffer, const bool is_sorted) [[reproducible]];

// circular queue properties
void proc_queue_rotate(PROC_QUE_ queue);

// min-heap properties
void proc_queue_bubble_up(PROC_QUE_ queue);
void proc_queue_bubble_down(PROC_QUE_ queue);

// insert
void proc_queue_insert_back(PROC_QUE_ queue, PROC_ proc);   /* queue is UNSORTED */
void proc_queue_insert_sorted(PROC_QUE_ queue, PROC_ proc); /* queue is SORTED */
extern void proc_queue_insert(PROC_QUE_ queue, PROC_ proc); /* generic */

// remove
void proc_queue_remove_front(PROC_QUE_ queue);  /* queue is UNSORTED */
void proc_queue_remove_sorted(PROC_QUE_ queue); /* queue is SORTED */
extern void proc_queue_remove(PROC_QUE_ queue); /* generic */

// view
proc_s *proc_queue_peek_circular(PROC_QUE_ queue); /* queue is UNSORTED */
proc_s *proc_queue_peek_sorted(PROC_QUE_ queue);   /* queue is SORTED */
extern proc_s *proc_queue_peek(PROC_QUE_ queue);   /* generic */


/* --- HELPER MACROS --- */

#define QUEUE_CIRC_BACK(queue)       ((queue->front + queue->len - 1) % queue->cap)
#define QUEUE_CIRC_BACK_NEXT(queue)  ((queue->front + queue->len) % queue->cap)
#define QUEUE_CIRC_FRONT_NEXT(queue) ((queue->front + 1) % queue->cap)


#endif /* SCHEDULER__PROC_QUEUE__H */
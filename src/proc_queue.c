#include <assert.h>
#include <stdint.h>
#include "utils/swap.h"
#include "proc-queue.h"

[[nodiscard]] inline bool proc_queue_isempty(PROC_QUE_ queue) [[reproducible]]
{
   assert(queue);
   return (queue->len == 0);
}

[[nodiscard]] inline bool proc_queue_isfull(PROC_QUE_ queue) [[reproducible]]
{
   assert(queue);
   return (queue->len == queue->cap);
}

[[nodiscard]] inline bool proc_queue_issorted(PROC_QUE_ queue) [[reproducible]]
{
   assert(queue);
   return (queue->front > queue->cap);
}


// Initialise queue
void proc_queue_init(PROC_QUE_ queue, const size_t cap, void *const buffer, const bool is_sorted)
{
   assert(queue);
   assert(cap > 0);

   *queue = (proc_queue_s) {
      .cap = cap,
      .len = 0,
      .front = (is_sorted) ? 0 : cap + 1,
      .data = (proc_s**)buffer,
   };
}

void proc_queue_rotate(PROC_QUE_ queue)
{
   assert(!proc_queue_issorted(queue));

   if (queue->len <= 1)
      return;

   queue->data[QUEUE_CIRC_BACK_NEXT(queue)] = queue->data[queue->front];
   queue->front = QUEUE_CIRC_FRONT_NEXT(queue);
}

void proc_queue_bubble_up(PROC_QUE_ queue)
{
   assert(proc_queue_issorted(queue));

   if (queue->len <= 1)
      return; 

   uint32_t index = queue->len - 1;
   while (index > 0)
   {
      uint32_t parent = (index - 1) / 2;
      if (0 <= proc_cmp(queue->data[index], queue->data[parent]))
         break;

      SWAP(queue->data[index], queue->data[parent]);
      index = parent;
   }
}

void proc_queue_bubble_down(PROC_QUE_ queue)
{
   assert(proc_queue_issorted(queue));

      if (queue->len <= 1)
      return;

   #define VALID_IDX(i) ((i) < queue->len)

   uint32_t index = 0;
   while (true)
   {
      uint32_t left = index * 2 + 1;
      uint32_t right = index * 2 + 2;
      uint32_t min = index;

      if (VALID_IDX(left) && (0 > proc_cmp(queue->data[left], queue->data[min])))
         min = left;

      if (VALID_IDX(right) && (0 > proc_cmp(queue->data[right], queue->data[min])))
         min = right;

      if (min == index)
         break;

      SWAP(queue->data[min], queue->data[index]);
      index = min;
   }

   #undef VALID_IDX
}

// insert
void proc_queue_insert_back(PROC_QUE_ queue, PROC_ proc)
{
   assert(!proc_queue_issorted(queue));
   assert(!proc_queue_isfull(queue));
   assert(proc);

   proc->state = READY;
   queue->data[QUEUE_CIRC_BACK_NEXT(queue)] = proc;
   queue->len++;
}

void proc_queue_insert_sorted(PROC_QUE_ queue, PROC_ proc)
{
   assert(proc_queue_issorted(queue));
   assert(!proc_queue_isfull(queue));
   assert(proc);

   proc->state = READY;
   queue->data[queue->len] = proc;
   queue->len++;
   proc_queue_bubble_up(queue);
}

inline void proc_queue_insert(PROC_QUE_ queue, PROC_ proc)
{
   assert(queue);
   if (proc_queue_issorted(queue))
      proc_queue_insert_sorted(queue, proc);
   else
      proc_queue_insert_back(queue, proc);
}


// remove
void proc_queue_remove_front(PROC_QUE_ queue)
{
   assert(!proc_queue_issorted(queue));
   assert(!proc_queue_isempty(queue));

   queue->data[queue->front]->state = EXIT;
   queue->front = QUEUE_CIRC_FRONT_NEXT(queue);
   queue->len--;
}

void proc_queue_remove_sorted(PROC_QUE_ queue)
{
   assert(proc_queue_issorted(queue));
   assert(!proc_queue_isempty(queue));

   queue->data[0]->state = EXIT;
   queue->len--;
   SWAP(queue->data[0], queue->data[queue->len]);
   proc_queue_bubble_down(queue);
}

inline void proc_queue_remove(PROC_QUE_ queue)
{
   assert(queue);
   if (proc_queue_issorted(queue))
      proc_queue_remove_sorted(queue);
   else
      proc_queue_remove_front(queue);
}

proc_s *proc_queue_peek_circular(PROC_QUE_ queue)
{
   assert(!proc_queue_isempty(queue));
   return queue->data[queue->front];
}

proc_s *proc_queue_peek_sorted(PROC_QUE_ queue)
{
   assert(!proc_queue_isempty(queue));
   return queue->data[0];
}

proc_s *proc_queue_peek(PROC_QUE_ queue)
{
   assert(queue);
   if (proc_queue_issorted(queue))
      return proc_queue_peek_sorted(queue);
   else
      return proc_queue_peek_circular(queue);
}
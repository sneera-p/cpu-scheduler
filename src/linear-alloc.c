#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "linear-alloc.h"

linear_alloc_s *linear_alloc_create(const size_t size)
{
   assert(size > 0);

   linear_alloc_s *alloc = malloc(sizeof(linear_alloc_s) + size);
   if (!alloc)
   {
      puts("Error: linear allocator failed to initialise");
      return nullptr;
   }

   *alloc = (linear_alloc_s) {
      .size = size,
      .offset = 0,
   };

   return alloc;
}

void linear_alloc_delete(LINEAR_ALLOC_ alloc)
{
   assert(alloc);
   free(alloc);
}


static inline byte_t *align_up(const byte_t *const cur, const size_t align)
{
   assert(cur);
   register const uintptr_t ptr = (uintptr_t)cur;
   register const uintptr_t aligned = (ptr + (align - 1)) & ~(align - 1);
   return (byte_t*)aligned;
}


void *linear_alloc(LINEAR_ALLOC_ alloc, const size_t align, const size_t size)
{
   assert(alloc);
   assert(align > 0);

   const byte_t *const cur = align_up(alloc->buffer + alloc->offset, align);
   const size_t offset = (size_t)(cur - alloc->buffer) + size;

   if (offset > alloc->size)
      return nullptr;

   alloc->offset = offset;
   return (void*)cur;
}

void linear_alloc_reset(LINEAR_ALLOC_ alloc)
{
   assert(alloc);
   alloc->offset = 0;
}

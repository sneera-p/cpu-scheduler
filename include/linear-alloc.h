#ifndef SCHEDULER__LINEAR_ALLOC__H
#define SCHEDULER__LINEAR_ALLOC__H

#include <stddef.h>
#include <stdint.h>


typedef uint8_t byte_t;

struct linear_alloc
{
   size_t size;
   size_t offset;
   byte_t buffer[];
};

typedef struct linear_alloc linear_alloc_s;
typedef linear_alloc_s *const restrict LINEAR_ALLOC_;


[[nodiscard]] linear_alloc_s *linear_alloc_create(const size_t size);
void linear_alloc_delete(LINEAR_ALLOC_ alloc);

[[nodiscard]] void *linear_alloc(LINEAR_ALLOC_ alloc, const size_t align, const size_t size);
void linear_alloc_reset(LINEAR_ALLOC_ alloc);


#define linear_alloc_type(alloc, type, len) \
   (linear_alloc((alloc), alignof(type), sizeof(type) * (len)))


#endif /* SCHEDULER__LINEAR_ALLOC__H */
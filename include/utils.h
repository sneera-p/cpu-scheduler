#ifndef __UTILS__H
#define __UTILS__H

#include <stdint.h>

/* --- indentation --- */

#define IND "   "
#define IND2 IND IND
#define IND3 IND2 IND
#define IND4 IND3 IND



/* --- x macro --- */

#define X_STR(elem) #elem,
#define X_ENUM(elem) elem,



/* --- aliases for timestamp vs. time --- */

typedef uint32_t ptimer_t;
typedef uint32_t ptime_delta_t;



/* --- max / min --- */

/*
 * WARNING: These are raw macros. 
 *    Do NOT pass expressions with side effects (e.g., i++, func()) 
 *    as arguments, as they will be evaluated twice.
 */
#define _MIN_RAW(a, b) (((a) < (b)) ? (a) : (b))
#define _MAX_RAW(a, b) (((a) > (b)) ? (a) : (b))

#define _GENERATE_MIN_MAX(type) \
[[nodiscard]] static inline type _min_##type(const type a, const type b) [[unsequenced]] { return _MIN_RAW(a, b); } \
[[nodiscard]] static inline type _max_##type(const type a, const type b) [[unsequenced]] { return _MAX_RAW(a, b); }

#define _min(type, a, b) \
   _min_##type(a, b)

#define _max(type, a, b) \
   _max_##type(a, b)


_GENERATE_MIN_MAX(uint32_t)
_GENERATE_MIN_MAX(uint64_t)
// _GENERATE_MIN_MAX(ptimer_t)
// _GENERATE_MIN_MAX(ptime_delta_t)

#define min(a, b) _Generic((a) + (b), \
   uint32_t: _min_uint32_t, \
   uint64_t: _min_uint64_t \
)(a, b)

#define max(a, b) _Generic((a) + (b), \
   uint32_t: _max_uint32_t, \
   uint64_t: _max_uint64_t \
)(a, b)



/* --- swap --- */

#define _swap(a, b) \
   do { \
      typeof(b) _tmp = (a); \
      (a) = (b); \
      (b) = _tmp; \
   } while(0)


#endif /* __UTILS__H */
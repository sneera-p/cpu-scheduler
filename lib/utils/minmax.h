#ifndef UTILS__MINMAX__H
#define UTILS__MINMAX__H


#include <stdint.h>

/*
 * WARNING: These are raw macros. 
 *    Do NOT pass expressions with side effects (e.g., i++, func()) 
 *    as arguments, as they will be evaluated twice.
 */
#define _MIN_RAW(a, b) (((a) < (b)) ? (a) : (b))
#define _MAX_RAW(a, b) (((a) > (b)) ? (a) : (b))


#define _GENERATE_MIN_MAX_FUNC(type) \
[[nodiscard]] static inline type _min_##type(const type a, const type b) [[unsequenced]] { return _MIN_RAW(a, b); } \
[[nodiscard]] static inline type _max_##type(const type a, const type b) [[unsequenced]] { return _MAX_RAW(a, b); }


#define _min(type, a, b) \
   _min_##type(a, b)

#define _max(type, a, b) \
   _max_##type(a, b)



/* _GENERATE_MIN_MAX_FUNC(int8_t)   */
/* _GENERATE_MIN_MAX_FUNC(int16_t)  */
/* _GENERATE_MIN_MAX_FUNC(int32_t)  */
/* _GENERATE_MIN_MAX_FUNC(int64_t)  */

/* _GENERATE_MIN_MAX_FUNC(uint8_t)  */
/* _GENERATE_MIN_MAX_FUNC(uint16_t) */
_GENERATE_MIN_MAX_FUNC(uint32_t)
_GENERATE_MIN_MAX_FUNC(uint64_t)


#define min(a, b) _Generic((a), \
   /* int8_t:   _min_int8_t,   */ \
   /* int16_t:  _min_int16_t,  */ \
   /* int32_t:  _min_int32_t,  */ \
   /* int64_t:  _min_int64_t,  */ \
   /* uint8_t:  _min_uint8_t,  */ \
   /* uint16_t: _min_uint16_t, */ \
   uint32_t: _min_uint32_t, \
   uint64_t: _min_uint64_t  \
)(a, b)

#define max(a, b) _Generic((a), \
   /* int8_t:   _max_int8_t,   */ \
   /* int16_t:  _max_int16_t,  */ \
   /* int32_t:  _max_int32_t,  */ \
   /* int64_t:  _max_int64_t,  */ \
   /* uint8_t:  _max_uint8_t,  */ \
   /* uint16_t: _max_uint16_t, */ \
   uint32_t: _max_uint32_t, \
   uint64_t: _max_uint64_t  \
)(a, b)


#endif /* UTILS__MINMAX__H */
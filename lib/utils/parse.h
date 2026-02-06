#ifndef UTILS__PARSE__H
#define UTILS__PARSE__H

#include <stddef.h>
#include <stdint.h>

static inline void skip_whitespace(const char *restrict *const s)
{
   while (**s == ' ' || **s == '\t')
      (*s)++;
}

// Returns UINT8_MAX if input overflows
[[nodiscard]] static inline uint8_t parse_u8(const char *restrict *const s)
{
   uint8_t v = 0;
   skip_whitespace(s);

   while (**s >= '0' && **s <= '9')
   {
      const unsigned char d = **s - '0';
      if (v > (UINT8_MAX - d) / 10)
         return UINT8_MAX;

      v = v * 10 + d;
      (*s)++;
   }

   skip_whitespace(s);
   return v;
}

// Returns SIZE_MAX if input overflows
[[nodiscard]] static inline size_t parse_usize(const char *restrict *const s)
{
   size_t v = 0;
   skip_whitespace(s);

   while (**s >= '0' && **s <= '9')
   {
      const unsigned char d = **s - '0';
      if (v > (SIZE_MAX - d) / 10)
         return SIZE_MAX;

      v = v * 10 + d;
      (*s)++;
   }

   skip_whitespace(s);
   return v;
}

#endif /* UTILS__PARSE__H */

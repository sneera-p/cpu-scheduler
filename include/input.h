#ifndef SCHEDULER__INPUT__H
#define SCHEDULER__INPUT__H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "utils/parse.h"
#include "config.h"

[[nodiscard]] static inline size_t input_size_stdin(const char *const msg)
{
   char buf[16];
   size_t v = 0;
   const char *restrict s;

   while (true)
   {
      fputs(msg, stdout);
      if (!fgets(buf, sizeof(buf), stdin))
      {
         fputs("Error: read failed\n", stderr);
         return 0;
      }

      s = buf;
      v = parse_usize(&s);

      if (*s == '\n' && v > 0)
         break;

      fputs("Error: input invalid\n", stderr);
   }

   return v;
}

[[nodiscard]] static inline priority_e input_priority_stdin(const uint32_t pid, const char *const msg)
{
   char buf[16];
   priority_e v = 0;
   const char *restrict s;

   while (true)
   {
      // fputs(IND msg, stdout);
      printf(msg, pid);
      if (!fgets(buf, sizeof(buf), stdin))
      {
         fputs("Error: read failed\n", stderr);
         return N_PRIORITY;
      }

      s = buf;
      v = parse_u8(&s);

      if (*s == '\n' && s != buf && v < N_PRIORITY)
         break;

      fputs(IND "Error: input invalid\n", stderr);
   }

   return v;
}

#endif /* SCHEDULER__INPUT__H */
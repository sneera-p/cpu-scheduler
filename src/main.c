#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "scheduler.h"

[[nodiscard]] static inline size_t parse_ulen(char *restrict s) [[reproducible]]
{
	assert(s);
	while(' ' == *s || '\t' == *s)
		s++; // skip whitespace / tabs

	size_t v = 0;
   for (; *s >= '0' && *s <= '9'; s++)
      v = v * 10 + (*s - '0');

   if (0 == v)
   	strcpy(s, "Error: Length must be a number greater than 0\n");
   return v;
}

int main(int argc, char *argv[])
{
	char buf[256];

	fputs("Enter no of processes: ", stdout);
	if (nullptr == fgets(buf, sizeof(buf), stdin))
	{
		fputs("Error: read failed\n", stderr);
		exit(EXIT_FAILURE);
	}

	const size_t len = parse_ulen(buf);
	if (0 == len)
	{
		fputs(buf, stderr);
		exit(EXIT_FAILURE);
	}

	SCHEDULER_ scheduler = scheduler_alloc(len);
	scheduler_init(scheduler, len, nullptr);
	scheduler_run(scheduler);
	scheduler_clear(scheduler);
	scheduler_free(scheduler);

	return 0;
}

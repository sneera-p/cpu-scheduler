#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "proc.h"


// int _main(int argc, char *argv[])
// {
// 	proc_arr_s procs;
// 	if (argc < 2)
// 	{
// 		procs = parse_procs_stdin();
// 	}
// 	else
// 	{
// 		FILE *stream = fopen(argv[1], "r");
// 		procs = parse_procs_toml(stream);
// 		fclose(stream);
// 	}

// 	if (0 == procs.len)
// 		return 1;
	
// 	for (size_t i = 0; i < procs.len; i++)
// 	{
// 		proc_s proc = procs.data[i];
//       	printf("Process %02u: [%s] takes %ums\n", proc.pid, priority_desc[proc.priority], proc.burst_time);
// 	}

// 	free(procs.data);
// 	return 0;
// }

int main()
{
	return 0;
}

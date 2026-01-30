#ifndef __CONFIG__H
#define __CONFIG__H

#include <stdint.h>
#include "utils.h"


/* --- TIME SETTINGS (ms) --- */

// aliases to differentiate timestamp from time (defined in utils.h)
// typedef uint32_t ptimer_t;
// typedef uint32_t ptime_delta_t;

// time per queue in the short-term scheduler before swapping
#define TIME_QUANTUM 20'000

// time per process in Q0 (Round Robin) before swapping
#define Q0_TIME_QUANTUM 1'250 

// Estimated overhead time to create & initialize a new process
#define TIME_PROC_INIT 200

// Estimated overhead time to load a process from long-scheduler to short-scheduler
#define TIME_PROC_LOAD 50


/* --- PROCESS STATE --- */

#define PSTATE(x) \
   x(NEW) /* creating / waiting to start running (in LTS) */ \
   x(READY) /* ready (in STS) */ \
   x(RUN) /* running (in CPU) */ \
   x(IO) /* waiting for I/O */ \
   x(TERM) /* finished running */

enum pstate : uint8_t
{
   PSTATE(X_ENUM) 
   N_PSTATE
};

typedef enum pstate pstate_e;
static const char *pstate_desc[N_PSTATE] = {
   PSTATE(X_STR)
};


/* --- PRIORITY LEVELS --- */

#define PRIORITY(x) \
   x(Q0) /* RR */ \
   x(Q1) /* SJF */ \
   x(Q2) /* SJF */ \
   x(Q3) /* FIFO */

enum priority : uint8_t
{ 
   PRIORITY(X_ENUM) 
   N_PRIORITY 
};

typedef enum priority priority_e;
static const char *priority_desc[N_PRIORITY] = {
	PRIORITY(X_STR)
};


/* --- QUEUE CAPACITY LIMITS --- */

#define Q0_NPROC 32
#define Q1_NPROC 128
#define Q2_NPROC 512
#define Q3_NPROC 2048

// Maximum number of processes allowed in each queue
static const uint32_t NPROC[N_PRIORITY] = {
   [Q0] = Q0_NPROC, 
   [Q1] = Q1_NPROC, 
   [Q2] = Q2_NPROC, 
   [Q3] = Q3_NPROC,
};


/* --- SANITY CHECKS --- */

// Ensures the Round Robin slices fit perfectly into the 20s window
static_assert(TIME_QUANTUM % Q0_TIME_QUANTUM == 0, "Q0 slice must divide evenly into total window");

// Ensures 2 process dont have the same arrival time
static_assert(TIME_PROC_INIT != 0, "proocess initialization cost cannot be 0");


#endif /* __CONFIG__H */
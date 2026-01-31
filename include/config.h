#ifndef SCHEDULER__CONFIG__H
#define SCHEDULER__CONFIG__H

#include "utils/xmacro.h"


/* --- TIME SETTINGS (ms) --- */

// time per queue in the short-term scheduler before swapping
#define TIME_QUANTUM 20'000

// time per process in Q0 (Round Robin) before swapping
#define Q0_TIME_QUANTUM 1'250 

// Estimated overhead time to create & initialize a new process
#define TIME_PROC_INIT 200

// Estimated overhead time to switch process
#define TIME_PROC_SWITCH 20


// indentation gap for stdout
#define IND "   "
#define IND2 IND IND
#define IND3 IND2 IND
#define IND4 IND3 IND


/* --- PROCESS STATE --- */

#define PROC_STATE(x) \
   x(NEW) /* creating / waiting to start running */ \
   x(READY) /* ready (in STS) */ \
   x(RUNNING) /* running (in CPU) */ \
   x(EXIT) /* finished running */

enum proc_state : uint8_t
{
   PROC_STATE(X_ENUM) 
   N_PROC_STATE
};

typedef enum proc_state proc_state_e;
static const char *proc_state_desc[N_PROC_STATE] = {
   PROC_STATE(X_DESC)
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
   PRIORITY(X_DESC)
};



/* --- SANITY CHECKS --- */

#ifndef TIME_QUANTUM
   #error "TIME_QUANTUM must be defined to switch process"
#endif

#ifndef Q0_TIME_QUANTUM
   #error "Q0_TIME_QUANTUM must be defined to switch RR queues"
#endif

#ifndef TIME_PROC_INIT
   #error "TIME_PROC_INIT must be defined to avoid process arrival_time collisions"
#endif

#ifndef TIME_PROC_SWITCH
   #error "TIME_PROC_SWITCH must be defined"
#endif

// Ensures the Round Robin slices fit perfectly into the 20s window
static_assert(TIME_QUANTUM % Q0_TIME_QUANTUM == 0, "Q0 slice must divide evenly into total window");

// Ensures 2 process dont have the same arrival time
static_assert(TIME_PROC_INIT != 0, "proocess initialization cost cannot be 0");


#endif /* SCHEDULER__CONFIG__H */
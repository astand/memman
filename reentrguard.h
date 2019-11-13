#pragma once

#define NO_LOCK
// #define RTOS_LOCK
// #define CORE_LOCK


#if defined (NO_LOCK)
#define setLOCK() { while(0); }
#define setUNLOCK() { while(0); }

#elif defined (RTOS_LOCK)
#include "rtos-api.h"

#define setLOCK() { stop_task_switching(); }
#define setUNLOCK() { resume_task_switching(); }

#elif defined (CORE_LOCK)
#include "core-mcu-api.h"

#define setLOCK() { disable_glo_interrupts(); }
#define setUNLOCK() { enable_glo_interrupts(); }
#endif

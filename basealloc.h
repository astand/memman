#pragma once

#include <stdint.h>

#if !defined NULL
#define NULL ((void*)0)
#endif

// namespace for ability to provide a few versions of allocator
// in one project (or library)
namespace basealloc
{

// return pointer to allocated block or 0 if allocation failed
extern void* getmem();
// frees memory under pointer
extern void freemem(void* p);
// for tests - resets the state of allocator to initial
extern void resetmem();

}


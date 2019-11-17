#pragma once

namespace basealloc
{

// return pointer to allocated block or 0 if allocation failed
void* getmem();
// frees memory under pointer
void freemem(void* p);

}

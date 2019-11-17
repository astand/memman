#include "pch.h"
#include "reentrguard.h"
#include "baseapi.h"
#include "basealloc.h"

namespace basealloc
{

void* getmem()
{
  blocknum_t blocknum;
  void* retp = NULL;
  setLOCK();

  if (try_get_next_block(blocknum))
  {
    // block was provided
    retp = get_memp_on_block(blocknum);
  }

  setUNLOCK();
  return retp;
}

void freemem(void* p)
{
  uint32_t address_to_free;
  blocknum_t blockid;
  address_to_free = (uint32_t)p;

  if (try_get_block_for_addr(address_to_free, blockid))
  {
    setLOCK();
    free_block(blockid);
    setUNLOCK();
  }

  return;
}

}

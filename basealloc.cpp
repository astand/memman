#include "pch.h"
#include <iostream>
#include "basealloc.h"
#include "baseallocconfig.h"
#include "reentrguard.h"

namespace basealloc
{

// this typedef can be usefull to minimize memory consumption
// when number of blocks is low (e.g. under 255 it can be uint8_t)
typedef uint32_t blocknum_t;

// for type clarification
const uint32_t kBlocksCount = BLOCKS_COUNT;
const uint32_t kBlockSize = ONE_BLOCK_SIZE;
const uint32_t kPoolSize = (kBlockSize* kBlocksCount);

// base vars
static volatile uint8_t memory_alloc_area[kPoolSize] = { 0 };
static volatile uint32_t pool_start_address = (uint32_t)& memory_alloc_area;

// structure to provide and handle O(1) performance for new memory allocation
static volatile struct
{
  blocknum_t cells[kBlocksCount];
  uint32_t head;
  uint32_t tail;
  int32_t size;
} freefifo;

// array for marking busy / not busy
static volatile uint8_t busy_flags[kBlocksCount] = { 0 };

static inline void shiftindex(volatile uint32_t& index)
{
  index = ((index + 1) % kBlocksCount);
}

static inline blocknum_t read_tail()
{
  blocknum_t ret = freefifo.cells[freefifo.tail];
  shiftindex(freefifo.tail);
  freefifo.size--;
  return ret;
}

static inline void put_head(blocknum_t num)
{
  shiftindex(freefifo.head);
  freefifo.cells[freefifo.head] = num;
  freefifo.size++;
}

// if returns 0 the block_num_out has valid value
static int32_t pick_next_free_block(blocknum_t& block_num_out)
{
  int32_t ret = 0;
  static bool mem_init = false;

  if (mem_init == false)
  {
    resetmem();
    mem_init = true;
    block_num_out = read_tail();
  }
  else if (freefifo.size > 0 && freefifo.size <= kBlocksCount)
  {
    // this cast is unsafe only for uint32_t type when blocks
    block_num_out = read_tail();
  }
  else
  {
    ret = -1;
  }

  return ret;
}

void* getmem()
{
  blocknum_t blocknum;
  void* retp = NULL;
  setLOCK();
  int32_t ret = pick_next_free_block(blocknum);

  if (ret >= 0)
  {
    busy_flags[blocknum] = 1;
    retp = (void*)(pool_start_address + (blocknum * kBlockSize));
  }

  setUNLOCK();
  return retp;
}

void freemem(void* p)
{
  uint32_t address_to_free;
  address_to_free = (uint32_t)p;
  address_to_free = (address_to_free - pool_start_address);

  if (address_to_free >= kPoolSize || (address_to_free % kBlockSize) != 0)
  {
    // address is invalid - do nothing
    return;
  }

  // get block number
  int32_t cell_id = address_to_free / kBlockSize;
  setLOCK();

  if (busy_flags[cell_id] == 1)
  {
    // non-free block goes to free state, put the number to freefifo
    put_head(cell_id);
    busy_flags[cell_id] = 0;
  }

  setUNLOCK();
  return;
}


void resetmem()
{
  freefifo.head = kBlocksCount - 1;
  freefifo.tail = 0;
  freefifo.size = kBlocksCount;

  for (blocknum_t i = 0; i < kBlocksCount; i++)
  {
    busy_flags[i] = 0;
    freefifo.cells[i] = i;
  }
}

}

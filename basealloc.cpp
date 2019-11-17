#include "pch.h"
#include <iostream>
#include "basealloc.h"
#include "baseallocconfig.h"

namespace basealloc
{

// for type clarification
const uint32_t kBlocksCount = BLOCKS_COUNT;
const uint32_t kBlockSize = ONE_BLOCK_SIZE;
const uint32_t kPoolSize = (kBlockSize* kBlocksCount);

// base vars
static volatile uint8_t memory_alloc_area[kPoolSize] = { 0 };
static volatile uint32_t pool_start_address = (uint32_t)& memory_alloc_area;

// stack for handling freeblock's numbers
static struct
{
  blocknum_t cells[kBlocksCount];
  uint32_t shead;
  uint32_t size;

  bool is_empty() {
    return (shead == 0);
  }

  bool is_full() {
    return (shead == size);
  }

  uint32_t count() {
    return shead;
  }

  blocknum_t peek() {
    if (!is_empty()) {
      return cells[shead - 1];
    }
  }

  blocknum_t pop() {
    if (!is_empty()) {
      shead--;
      return cells[shead];
    }
  }

  void push(blocknum_t value) {
    if (!is_full()) {
      cells[shead] = value;
      shead++;
    }
  }
} freestack;

// array for marking busy / not busy
static volatile uint8_t busy_flags[kBlocksCount] = { 0 };

bool try_get_next_block(blocknum_t& bid)
{
  static bool mem_init = false;
  bool ret = true;

  if (mem_init == false)
  {
    test_reset_mem();
    mem_init = true;
    bid = freestack.pop();
    busy_flags[bid] = 1;
  }
  else if (!freestack.is_empty())
  {
    // this cast is unsafe only for uint32_t type when blocks
    bid = freestack.pop();
    busy_flags[bid] = 1;
  }
  else
  {
    ret = false;
  }

  return ret;
}


void* get_memp_on_block(blocknum_t block)
{
  return (void*)(pool_start_address + (block * kBlockSize));
}


// if the true is returned the bid has the valid block number to free
bool try_get_block_for_addr(uint32_t mem, blocknum_t& bid)
{
  mem = (mem - pool_start_address);

  if (mem >= kPoolSize || (mem % kBlockSize) != 0)
  {
    // address is invalid - do nothing
    return false;
  }

  // get block number
  bid = static_cast<blocknum_t>(mem / kBlockSize);
  return true;
}


void free_block(blocknum_t bid)
{
  if (bid < kBlocksCount)
  {
    freestack.push(bid);
    busy_flags[bid] = 0;
  }
}


void test_reset_mem()
{
  freestack.shead = 0;
  freestack.size = kBlocksCount;

  for (blocknum_t i = (kBlocksCount); i != 0; i--)
  {
    busy_flags[i - 1] = 0;
    freestack.push(i - 1);
  }
}

}

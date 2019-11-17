#pragma once

#include <stdint.h>

#if !defined NULL
#define NULL ((void*)0)
#endif

namespace basealloc
{
// this typedef can be usefull to minimize memory consumption
// when number of blocks is low (e.g. under 255 it can be uint8_t)
typedef uint32_t blocknum_t;

// return true if the new block of memory is releasing
bool try_get_next_block(blocknum_t& bid);

// returns pointer on the start of memory  of the block.
// the @block param is not checked inside,
// it must strongly used the result of
// @try_get_next_block() function as the value
void* get_memp_on_block(blocknum_t block);

// checkk if address is valid and if so returns true and
// appropriate block number
bool try_get_block_for_addr(uint32_t mem, blocknum_t& bid);

// frees block
void free_block(blocknum_t bid);

// for tests - resets the state of allocator to initial
void test_reset_mem();

}


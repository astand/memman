#include "pch.h"
#include <iostream>
#include "baseapi.h"
#include "baseallocconfig.h"
#include "basealloc.h"
#include "basealloc_tests.h"

using namespace basealloc;

typedef enum
{
  PASSED = 0,
  NotAlloc,
  AllocedOut,
  InvlFree,
  FailedMix1,
  FailedMix2,
  FailedMemAddress,
  MaxTestResultNum
} testresult_t;

static const char* testmessages[MaxTestResultNum] =
{
  "PASSED",
  "Failed: memory was not allocated when it must be",
  "Failed: memory allocated when it cannot be",
  "Failed: no allocation after invalid memory releasing",
  "Failed: reallocation returned 0 when must not",
  "Failed: reallocation returned pointer when must 0",
  "Failed: memory address is incorrect"
};


// all the tests must return 0 if test passed
testresult_t OutOfRequest_Test()
{
  void* p;
  test_reset_mem();

  for (int32_t i = 0; i < (BLOCKS_COUNT); i++)
  {
    p = getmem();

    if (p == (NULL))
    {
      return NotAlloc;
    }
  }

  p = getmem();

  if (p != NULL)
  {
    return AllocedOut;
  }

  return PASSED;
}

testresult_t MultiBlockFree_Test()
{
  void* p;
  test_reset_mem();
  p = getmem();
  freemem(p);
  freemem(p);
  p = getmem();

  if (p == 0)
  {
    return InvlFree;
  }

  return PASSED;
}

testresult_t WrongBlockFree_Test()
{
  void* p;
  test_reset_mem();
  p = getmem();
  freemem((void*)((uint32_t)p + 1));
  freemem((void*)((uint32_t)p + 2));
  freemem((void*)((uint32_t)p + 3));
  freemem((void*)((uint32_t)p + 4));
  freemem((void*)((uint32_t)p + 5));
  freemem((void*)((uint32_t)p - 5));
  freemem((void*)((uint32_t)p - 4));
  freemem((void*)((uint32_t)p - 3));
  freemem((void*)((uint32_t)p - 2));
  freemem((void*)((uint32_t)p - 1));
  freemem(p);
  return PASSED;
}

testresult_t ZeroBlockFree_Test()
{
  void* p;
  test_reset_mem();
  p = getmem();
  freemem(p);
  freemem(NULL);
  p = getmem();

  if (p == 0)
  {
    return InvlFree;
  }

  return PASSED;
}

testresult_t MixedAllocAndFree_Test()
{
  void* p;
  test_reset_mem();

  for (int32_t i = 0; i < BLOCKS_COUNT; i++)
  {
    p = getmem();

    if ((i % 2) == 0)
    {
      // free each even block
      freemem(p);
    }
  }

  // get the count of availible block
  int32_t left_free_count = ((BLOCKS_COUNT & 1) != 0) ? ((BLOCKS_COUNT / 2) + 1) : (BLOCKS_COUNT / 2);

  for (int32_t i = 0; i < left_free_count; i++)
  {
    p = getmem();

    if (p == NULL)
    {
      return FailedMix1;
    }
  }

  p = getmem();

  if (p != NULL)
  {
    return FailedMix2;
  }

  return PASSED;
}


testresult_t MemoryAddressInv_Test()
{
  void* p;
  test_reset_mem();
  static uint32_t prevaddress;
  uint32_t address;

  for (int32_t i = 0; i < BLOCKS_COUNT; i++)
  {
    p = getmem();
    address = (uint32_t)p;

    if (i > 0)
    {
      if (address != (prevaddress + ONE_BLOCK_SIZE))
        return FailedMemAddress;
    }

    prevaddress = address;
  }

  return PASSED;
}


void make_tests()
{
  std::cout << "Test 1 : " << testmessages[OutOfRequest_Test()] << '\n';
  std::cout << "Test 2 : " << testmessages[MultiBlockFree_Test()] << '\n';
  std::cout << "Test 3 : " << testmessages[WrongBlockFree_Test()] << '\n';
  std::cout << "Test 4 : " << testmessages[ZeroBlockFree_Test()] << '\n';
  std::cout << "Test 5 : " << testmessages[MixedAllocAndFree_Test()] << '\n';
  std::cout << "Test 6 : " << testmessages[MemoryAddressInv_Test()] << '\n';
  test_reset_mem();
}

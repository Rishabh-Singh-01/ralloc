#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

#include "../core/ralloc.c"
#include "../utils/stack.c"

int generateRandomNum(int s, int n) { return s + (rand() % n); }

int main() {
  Stack testStack;
  initStack(&testStack);

  srand(time(NULL));
  initialize();

  int maxCount = 0;

  int cycles = generateRandomNum(1, 30);
  int numOccupy = generateRandomNum(1, 50);
  int numFree = generateRandomNum(1, numOccupy);
  /*int prevNumOccupy = numOccupy;*/
  /*int prevNumFree = numFree;*/

  int *rallocPtrListPtr[1000000];
  int expectedValueListPtr[1000000];

  int cycleItr = cycles;
  while (cycleItr > 0) {
    if (cycleItr == cycles) {
      // Running the alloc cycle
      int tempOccupy = 0;
      while (tempOccupy < numOccupy) {
        void *testPtr = rallocate(sizeof(int));
        LOG_INFO("Testing Ptr: %p", testPtr);
        rallocPtrListPtr[tempOccupy] = testPtr;
        int num = generateRandomNum(1, 100);
        expectedValueListPtr[tempOccupy] = num;
        *(rallocPtrListPtr[tempOccupy]) = num;
        tempOccupy++;
        maxCount++;
      }

      // Running the free cycle
      tempOccupy = 0;
      while (tempOccupy < numFree) {
        int num = generateRandomNum(0, numOccupy);
        LOG_DEBUG("This is the free num: '%d'", num);
        LOG_DEBUG("This is the free ptr: '%p'", rallocPtrListPtr[num]);
        expectedValueListPtr[num] = -1;
        ralloc_free(rallocPtrListPtr[num]);
        push(&testStack, num);
        tempOccupy++;
      }

      // Running the assertions
      /*tempOccupy = 0;*/
      /*while (tempOccupy < numOccupy) {*/
      /*  assert(expectedValueListPtr[tempOccupy] ==*/
      /*             *(rallocPtrListPtr[tempOccupy]) &&*/
      /*         "Unexpected Value Mismatch");*/
      /*  LOG_INFO("Test Passed: Expected '%d', Actual '%d'",*/
      /*           expectedValueListPtr[tempOccupy],*/
      /*           *(rallocPtrListPtr[tempOccupy]));*/
      /*  LOG_INFO("Test Passed: Pointer '%p'",
       * (rallocPtrListPtr[tempOccupy]));*/
      /*  tempOccupy++;*/
      /*}*/

      dumpRalloc(numOccupy);
      int *some = dumpRallocToArr(numOccupy);
      tempOccupy = 0;

      while (tempOccupy < numOccupy) {
        int expected = expectedValueListPtr[tempOccupy];
        int actual = some[tempOccupy];
        LOG_DEBUG("Test InProgreess: Expected '%d', Actual '%d'", expected,
                  actual);

        assert(expected == actual && "Unexpected Value Mismatch");
        LOG_INFO("Test Passed: Expected '%d', Actual '%d'",
                 expectedValueListPtr[tempOccupy], some[tempOccupy]);
        tempOccupy++;
      }
      free(some);
    } else {
      int tempOccupy = 0;
      int tempPrevNumOccupy = maxCount;

      while (tempOccupy < numOccupy) {
        int *testPtr = rallocate(sizeof(int));
        int num = generateRandomNum(1, 100);

        if (!isEmpty(&testStack)) {
          int lastPushed = pop(&testStack);
          expectedValueListPtr[lastPushed] = num;
          rallocPtrListPtr[lastPushed] = testPtr;
          *(rallocPtrListPtr[lastPushed]) = num;
        } else {
          rallocPtrListPtr[tempPrevNumOccupy] = testPtr;
          expectedValueListPtr[tempPrevNumOccupy] = num;
          *(rallocPtrListPtr[tempPrevNumOccupy]) = num;
          tempPrevNumOccupy++;
          maxCount++;
        }
        tempOccupy++;
      }

      // Running the free cycle
      tempOccupy = 0;
      while (tempOccupy < numFree) {
        int num = generateRandomNum(0, tempPrevNumOccupy);
        expectedValueListPtr[num] = -1;
        ralloc_free(rallocPtrListPtr[num]);
        push(&testStack, num);
        tempOccupy++;
      }

      // Running the assertions
      /*tempOccupy = 0;*/
      /*while (tempOccupy < tempPrevNumOccupy) {*/
      /*  assert(expectedValueListPtr[tempOccupy] ==*/
      /*             *(rallocPtrListPtr[tempOccupy]) &&*/
      /*         "Unexpected Value Mismatch");*/
      /*  LOG_INFO("Test Passed: Expected '%d', Actual '%d'",*/
      /*           expectedValueListPtr[tempOccupy],*/
      /*           *(rallocPtrListPtr[tempOccupy]));*/
      /*  LOG_INFO("Test Passed: Pointer '%p'",
       * (rallocPtrListPtr[tempOccupy]));*/
      /*  tempOccupy++;*/
      /*}*/

      // running main testPtr
      dumpRalloc(maxCount);
      int *some = dumpRallocToArr(maxCount);
      tempOccupy = 0;

      while (tempOccupy < tempPrevNumOccupy) {
        int expected = expectedValueListPtr[tempOccupy];
        int actual = some[tempOccupy];
        LOG_DEBUG("Test InProgreess: Expected '%d', Actual '%d'", expected,
                  actual);

        assert(expected == actual && "Unexpected Value Mismatch");
        LOG_INFO("Test Passed: Expected '%d', Actual '%d'",
                 expectedValueListPtr[tempOccupy], some[tempOccupy]);
        tempOccupy++;
      }
      free(some);
    }

    numOccupy = generateRandomNum(1, 50);
    numFree = generateRandomNum(1, numOccupy);
    cycleItr--;
  }
  return 0;
}

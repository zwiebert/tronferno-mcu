#include "unity.h"
#include "test_runner.h"
#include "assert.h"
#include "string.h"

#define MAX_TEST_COUNT 20
static int idx;
test_desc_t tests[MAX_TEST_COUNT];


static void test_run(test_desc_t *t)
{
    Unity.CurrentTestName = t->desc;
    Unity.CurrentTestLineNumber = (UNITY_LINE_TYPE)t->line;
    Unity.TestFile = t->file;
    Unity.NumberOfTests++;
    UNITY_CLR_DETAILS();
    UNITY_EXEC_TIME_START();
    if (TEST_PROTECT())
    {
        //setUp();

        (*t->fn)();
    }
    if (TEST_PROTECT())
    {
        //tearDown();
    }
    UNITY_EXEC_TIME_STOP();
    UnityConcludeTest();
}


void unity_testcase_register(test_desc_t *desc) {
  assert(idx < MAX_TEST_COUNT);

  memcpy(&tests[idx++], desc, sizeof *desc);
}

int main() {
  UNITY_BEGIN();
  setUp();
  for (int i = 0; i < MAX_TEST_COUNT; ++i) {
    test_desc_t *desc = &tests[i];
    if (desc->fn)
      test_run(desc);
  }
  tearDown();
  return UNITY_END();
}


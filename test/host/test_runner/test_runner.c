#include "unity.h"
#include "test_runner.h"
#include "assert.h"
#include "string.h"

#define MAX_TEST_COUNT 20
static int idx;
test_desc_t tests[MAX_TEST_COUNT];

void unity_testcase_register(test_desc_t *desc) {
  assert(idx < MAX_TEST_COUNT);

  memcpy(&tests[idx++], desc, sizeof *desc);
}

int main() {
  UNITY_BEGIN();
  for (int i = 0; i < MAX_TEST_COUNT; ++i) {
    test_desc_t *desc = &tests[i];
    if (desc->fn)
      if (TEST_PROTECT())
        (*desc->fn)();
  }
  return UNITY_END();
}


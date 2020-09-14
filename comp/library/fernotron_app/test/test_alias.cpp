/*
 * test_alias.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "unity.h"
#include "fernotron/alias/pairings.h"
#include "txtio/inout.h"
#include <string.h>

#define A 0x101234
static GmBitMask gm, gme;
const GmBitMask gmz;
static bool succ;
#define SIZE_GM sizeof (GmBitMask)


static int pair_and_read_back(unsigned a, uint8_t g, uint8_t m, bool unpair) {
  if (!pair_controller(a, g, m, unpair))
    return -1;
  if (!pair_getControllerPairings(a, &gm))
    return -2;

  gme.putBit(g, m, !unpair);
  return 0;
}

static void test_pair_multiple_members() {

  pair_rmController(A);
  TEST_ASSERT_FALSE(pair_getControllerPairings(A, &gm));

  gme.clear();

  TEST_ASSERT_EQUAL(0, pair_and_read_back(A, 1, 2, false));
  TEST_ASSERT_EQUAL_HEX8_ARRAY(gme, gm, SIZE_GM);

  TEST_ASSERT_EQUAL(0, pair_and_read_back(A, 1, 3, false));
  TEST_ASSERT_EQUAL_HEX8_ARRAY(gme, gm, SIZE_GM);

  TEST_ASSERT_EQUAL(0, pair_and_read_back(A, 1, 3, true));
  TEST_ASSERT_EQUAL_HEX8_ARRAY(gme, gm, SIZE_GM);

  TEST_ASSERT_EQUAL(0, pair_and_read_back(A, 4, 5, false));
  TEST_ASSERT_EQUAL_HEX8_ARRAY(gme, gm, SIZE_GM);

  TEST_ASSERT_EQUAL(0, pair_and_read_back(A, 4, 6, false));
  TEST_ASSERT_EQUAL_HEX8_ARRAY(gme, gm, SIZE_GM);

  TEST_ASSERT_EQUAL(0, pair_and_read_back(A, 4, 5, true));
  TEST_ASSERT_EQUAL_HEX8_ARRAY(gme, gm, SIZE_GM);

  TEST_ASSERT_EQUAL(0, pair_and_read_back(A, 4, 6, true));
  TEST_ASSERT_EQUAL_HEX8_ARRAY(gme, gm, SIZE_GM);

  TEST_ASSERT_TRUE(pair_rmController(A));
}



TEST_CASE("pair multiple members", "[fernotron/alias]")
{
  test_pair_multiple_members();
}


#ifdef TEST_HOST
typedef int so_msg_t;
void  so_output_message(so_msg_t mt, void *arg) {}
void so_output_message(so_msg_t mt, const void *arg) {}
#endif


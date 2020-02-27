/*
 * test_kvs.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */


/*
 * test_timer_data.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "unity.h"
#include "key_value_store/kvs_wrapper.h"

kvshT handle;
bool succ;
int res;
const char *NS = "testing";
const char *asdfg = "asdfg", *asdf = "asdf", *asdfge = "asdfge";

static void g(uint8_t g, uint8_t m) {
  handle = kvs_open(NS, kvs_WRITE);
  TEST_ASSERT_NOT_NULL(handle);
  succ = kvs_set_i8(handle, asdfg, 42);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);
  kvs_close(handle);

  handle = kvs_open(NS, kvs_READ);
  TEST_ASSERT_NOT_NULL(handle);
  succ = kvs_set_i8(handle, asdf, 42);
  TEST_ASSERT_FALSE_MESSAGE(succ, "handle is read-only");
  kvs_close(handle);

  handle = kvs_open(NS, kvs_WRITE);
  TEST_ASSERT_NOT_NULL(handle);
  succ = kvs_set_i8(handle, asdf, 42);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);
  kvs_close(handle);

  res = kvs_foreach(NS, KVS_TYPE_I8, asdf, 0);
  TEST_ASSERT_EQUAL(res, 2);
  res = kvs_foreach(NS, KVS_TYPE_I8, asdfg, 0);
  TEST_ASSERT_EQUAL(res, 1);
  res = kvs_foreach(NS, KVS_TYPE_U8, asdfg, 0);
  TEST_ASSERT_EQUAL_MESSAGE(res, 0, "wrong type");
  res = kvs_foreach(NS, KVS_TYPE_I8, asdfge, 0);
  TEST_ASSERT_EQUAL_MESSAGE(res, 0, "match string longer than key");

  handle = kvs_open(NS, kvs_WRITE);
  TEST_ASSERT_NOT_NULL(handle);
  succ = kvs_set_i8(handle, asdf, 42);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);
  kvs_close(handle);

  res = kvs_foreach(NS, KVS_TYPE_I8, asdf, 0);
  TEST_ASSERT_EQUAL(res, 2);

  handle = kvs_open(NS, kvs_WRITE);
  succ = kvs_erase_key(handle, asdf);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_erase_key(handle, asdfg);
  TEST_ASSERT_TRUE(succ);
  succ = kvs_erase_key(handle, asdfge);
  TEST_ASSERT_FALSE_MESSAGE(succ, "non-existing key");
  succ = kvs_commit(handle);
  TEST_ASSERT_TRUE(succ);
  kvs_close(handle);

}

static void f() {
  g(1,2);
}

TEST_CASE("kvs", "[kvs]") {
  f();
}

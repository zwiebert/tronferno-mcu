/*
 * test_prg.c
 *
 *  Created on: 07.03.2020
 *      Author: bertw
 */
#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#include <sys/random.h>
#endif

#include <fernotron_trx/raw/fer_msg_plain.h>
#include "fernotron_trx/astro.h"
#include <string.h>
#include "../fer_app_cfg.h"
#include "../fer_api.h"
#include "debug/dbg.h"
#include "fernotron_trx/raw/fer_rawmsg_build.h"
#include <fernotron_trx/fer_trx_api.hh>
#include "utils_misc/bcd.h"


 void tst_ferMsg_writeRtc_andVerify_random() {
#ifdef TEST_HOST
   time_t tim = random();
   setenv("TZ", "UTC+0", 1);
   struct tm *tm = localtime(&tim);
   struct fer_rtc_sd rtc{};
   fer_msg_rtc_from_time(&rtc, tim, MSG_TYPE_RTC);
   TEST_ASSERT_EQUAL(tm->tm_mon+1, bcd2dec(rtc.mont));
   TEST_ASSERT_EQUAL(tm->tm_mday, bcd2dec(rtc.mday));
   TEST_ASSERT_EQUAL(tm->tm_hour, bcd2dec(rtc.hour));
   TEST_ASSERT_EQUAL(tm->tm_min, bcd2dec(rtc.mint));
   TEST_ASSERT_EQUAL(tm->tm_sec, bcd2dec(rtc.secs));
   TEST_ASSERT_TRUE(rtc.wd2.sd.rtc_only);
   TEST_ASSERT_EQUAL(tm->tm_wday == 0 ? 7 : tm->tm_wday, rtc.wd2.sd.wday);
   //TEST_ASSERT_EQUAL((0x80|wd2_WED),rtc.wd2.bd);
   TEST_ASSERT_FALSE(rtc.flags.bits.dst);
   //TEST_ASSERT_TRUE(rtc.wDayBits.wed);
#endif
 }


 void tst_ferMsg_writeRtc_andVerify() {
   time_t tim = 1593605594; //2020-07-01T12:13:14+00:00 (Wednesday)
   setenv("TZ", "CET-1CEST-2,M3.5.0,M10.5.0", 1);
   struct fer_rtc_sd rtc{};
   fer_msg_rtc_from_time(&rtc, tim, MSG_TYPE_RTC);
   TEST_ASSERT_EQUAL(7, bcd2dec(rtc.mont));
   TEST_ASSERT_EQUAL(1, bcd2dec(rtc.mday));
   TEST_ASSERT_EQUAL(14, bcd2dec(rtc.hour));
   TEST_ASSERT_EQUAL(13, bcd2dec(rtc.mint));
   TEST_ASSERT_EQUAL(14, bcd2dec(rtc.secs));
   TEST_ASSERT_TRUE(rtc.wd2.sd.rtc_only);
   TEST_ASSERT_EQUAL(wd2_WED, rtc.wd2.sd.wday);
   TEST_ASSERT_EQUAL((0x80|wd2_WED),rtc.wd2.bd);
   TEST_ASSERT_TRUE(rtc.flags.bits.dst);
   TEST_ASSERT_TRUE(rtc.wDayBits.wed);
 }

static struct fer_raw_msg  test_msg = {
    .cmd = { .bd = { 0x80, 0x49, 0x5d, 0x68, 0x2f, 0xbd, }},

    .rtc = { .bd = { 0x32, 0x51, 0x01, 0x04, 0x14, 0x08, 0x02, 0x86, 0xa6 }},

    .wdtimer = { .bd = {
        { 0x30, 0x09, 0x00, 0x05, 0x15, 0x09, 0x00, 0x05, 0xad },
        { 0x15, 0x09, 0x00, 0x05, 0x15, 0x09, 0x00, 0x05, 0xa0 },
        { 0x15, 0x09, 0x00, 0x05, 0x15, 0x09, 0x00, 0x05, 0x86 },
        { 0x30, 0x09, 0x00, 0x05, 0xff, 0x0f, 0xff, 0x0f, 0x66 },
    }},

    .astro = {.bd = {
        { 0x34, 0x15, 0x36, 0x15, 0x36, 0x15, 0x38, 0x15, 0xf8 },
        { 0x40, 0x15, 0x44, 0x15, 0x48, 0x15, 0x52, 0x15, 0x62 },
        { 0x58, 0x15, 0x04, 0x16, 0x10, 0x16, 0x16, 0x16, 0x9d },
        { 0x24, 0x16, 0x30, 0x16, 0x38, 0x16, 0x46, 0x16, 0x64 },
        { 0x52, 0x16, 0x00, 0x17, 0x08, 0x17, 0x18, 0x17, 0x95 },
        { 0x26, 0x17, 0x34, 0x17, 0x42, 0x17, 0x50, 0x17, 0x72 },
        { 0x58, 0x17, 0x06, 0x18, 0x16, 0x18, 0x24, 0x18, 0xdb },
        { 0x32, 0x18, 0x42, 0x18, 0x50, 0x18, 0x58, 0x18, 0x32 },
        { 0x08, 0x1f, 0x16, 0x1f, 0x24, 0x1f, 0x32, 0x1f, 0x54 },
        { 0x40, 0x1f, 0x46, 0x1f, 0x54, 0x1f, 0x00, 0x20, 0xff },
        { 0x06, 0x20, 0x10, 0x20, 0x14, 0x20, 0x18, 0x20, 0xc0 },
        { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x80 },

    }},
    .last = { 0x00, 0x80, 0x49, 0x5d, 0x53, 0x00, 0x10, 0x00, 0x05 },

};

const uint8_t testdat_wtimer[] =
  { 0x56, 0x06, 0x45, 0x19, 0x12, 0x07, 0x34, 0x21, 0x12, 0x07, 0x34, 0x21, 0x12, 0x07, 0x34, 0x21, 0x12, 0x07, 0x34, 0x21, 0x12, 0x07, 0x34, 0x21, 0x56,
      0x06, 0x45, 0x19, 0x0 };
const uint8_t testdat_dtimer[] = { };
extern const uint8_t astro_data[12][8];

static void test_ferMsg_size() {
  TEST_ASSERT_EQUAL(FER_CMD_BYTE_CT - 1, sizeof test_msg.cmd.sd.cmd);
  TEST_ASSERT_EQUAL(FER_CMD_BYTE_CT, sizeof test_msg.cmd);
  TEST_ASSERT_EQUAL(FER_PRG_BYTE_CT, sizeof test_msg.rtc);
  TEST_ASSERT_EQUAL(FER_FPR_TIMER_HEIGHT * FER_PRG_BYTE_CT,sizeof test_msg.wdtimer);
  TEST_ASSERT_EQUAL(FER_FPR_ASTRO_HEIGHT * FER_PRG_BYTE_CT,sizeof test_msg.astro);
  TEST_ASSERT_EQUAL(FER_CMD_BYTE_CT + FER_FPR_DATA_HEIGHT * FER_PRG_BYTE_CT,sizeof test_msg);
}

static void test_ferMsg_verifyChecksums() {
  bool result;
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_PLAIN);
  TEST_ASSERT_TRUE(result);
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_RTC);
  TEST_ASSERT_TRUE(result);
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_TIMER);
  TEST_ASSERT_TRUE(result);
}

static void test_ferMsg_writeChecksums() {
  fer_msg_raw_checksumsCreate(&test_msg, MSG_TYPE_TIMER);
}

static void test_ferMsg_writeData() {
  fer_msg_raw_from_time(&test_msg, time(NULL), MSG_TYPE_TIMER);
  fer_msg_raw_from_weeklyTimer(&test_msg, testdat_wtimer);
  fer_msg_raw_from_dailyTimer(&test_msg, testdat_wtimer);
  fer_msg_raw_from_astro(&test_msg, 0);
}

static void test_ferMsg_modData_andVerifyCS() {
  bool result;

  ++test_msg.astro.bd[1][8];
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_PLAIN);
  TEST_ASSERT_TRUE(result);
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_RTC);
  TEST_ASSERT_TRUE(result);
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_TIMER);
  TEST_ASSERT_FALSE(result);
  --test_msg.astro.bd[1][8];

  ++test_msg.wdtimer.bd[1][8];
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_PLAIN);
  TEST_ASSERT_TRUE(result);
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_RTC);
  TEST_ASSERT_TRUE(result);
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_TIMER);
  TEST_ASSERT_FALSE(result);
  --test_msg.wdtimer.bd[1][8];

  ++test_msg.rtc.bd[3];
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_PLAIN);
  TEST_ASSERT_TRUE(result);
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_RTC);
  TEST_ASSERT_FALSE(result);
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_TIMER);
  TEST_ASSERT_FALSE(result);
  --test_msg.rtc.bd[3];

  ++test_msg.cmd.bd[3];
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_PLAIN);
  TEST_ASSERT_FALSE(result);
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_RTC);
  TEST_ASSERT_FALSE(result);
  result = fer_msg_raw_checksumsVerify(&test_msg, MSG_TYPE_TIMER);
  TEST_ASSERT_FALSE(result);
  --test_msg.cmd.bd[3];
}

TEST_CASE("fer_msg struct size", "[fernotron]")
{
  test_ferMsg_size();
}

TEST_CASE("fer_msg verify checksums", "[fernotron]")
{
  test_ferMsg_verifyChecksums();
}
TEST_CASE("fer_msg write checksums and verify", "[fernotron]")
{
  test_ferMsg_writeChecksums();
  test_ferMsg_verifyChecksums();
}
TEST_CASE("fer_msg write data/checksums and verify", "[fernotron]")
{
  test_ferMsg_writeData();
  test_ferMsg_writeChecksums();
  test_ferMsg_verifyChecksums();
}

TEST_CASE("fer_msg modify data and verify checksums", "[fernotron]")
{
  test_ferMsg_modData_andVerifyCS();
}

TEST_CASE("fer_msg write/verify rtc_msg", "[fernotron]")
{
  tst_ferMsg_writeRtc_andVerify();
  for (int i=0; i < 100; ++i)
    tst_ferMsg_writeRtc_andVerify_random();
}


#ifdef TEST_HOST
static struct cfg_astro cfg_astro =
    { .astroCorrection = acAverage, .geo_longitude = 13, .geo_latitude = 52, .geo_timezone = 1,  };
void setUp() {
  Fer_Trx_API::setup_astro(&cfg_astro);
}
#endif

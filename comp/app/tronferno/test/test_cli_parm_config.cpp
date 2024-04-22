#include <unity.h>

#include "../app_cli/parm_config.hh"
#include "../app_cli/cli_internal.hh"
#include "../app_cli/include/app_cli/cli_app.hh"

#include <uout/uout_writer.hh>
#include <array>


//#define DEFINE_COMP_SETTINGS_OBJECT
#include <config_kvs/register_settings.hh>

class UoutWriterTest final : public UoutWriter {
  typedef int (*writeReq_fnT)(void *req, const char *s, ssize_t len, bool final);

public:
  UoutWriterTest(so_target_bits tgt) :
      UoutWriter(tgt) {
  }

  UoutWriterTest(const UoutWriterConsole&) = delete;
  virtual ~UoutWriterTest() {
  }

public:
private:
  virtual int priv_write(const char *s, ssize_t len, bool final) override {
    for (ssize_t i = 0; i < len; ++i, ++wbuf_idx_) {
      wbuf_[wbuf_idx_] = s[i];
    }
    wbuf_[wbuf_idx_] = '\0';
    return len;
  }

public:
  mutable std::array<char, 200> wbuf_ = { };
  mutable unsigned wbuf_idx_ = 0;
  void clear_buf() const {
    wbuf_idx_ = 0;
    wbuf_[0] = '\0';
  }
  void clear() {
    clear_buf();
    sj().free_buffer();
  }
};

void tst_parm_config_empty() {
  clpar parm[] = { { "config", nullptr }, };
  {
  UoutWriterTest td(SO_TGT_FLAG_TXT | SO_TGT_ANY);

  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("cli_reply: ok\n", &td.wbuf_[0]);
}
  {
  UoutWriterTest td(SO_TGT_FLAG_JSON | SO_TGT_ANY);

  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
    td.sj().write_json();
  }
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{}}", &td.wbuf_[0]);
}
}
void tst_parm_config_cu() {
  clpar parm[] = { { "config", nullptr }, { "cu", "806789" }, { "cu", "?" } };
  {
  UoutWriterTest td(SO_TGT_FLAG_TXT | SO_TGT_ANY);

  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("tf: cli_reply: config: cu=806789;\ncli_reply: ok\n", &td.wbuf_[0]);
}
  {
  UoutWriterTest td(SO_TGT_FLAG_JSON | SO_TGT_ANY);

  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
    td.sj().write_json();
  }
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{\"cu\":\"806789\"}}", &td.wbuf_[0]);
}
}

void tst_parm_config_tz() {
  clpar parm[] = { { "config", nullptr }, { "tz", "CET" }, { "tz", "?" } };
  {
  UoutWriterTest td(SO_TGT_FLAG_TXT | SO_TGT_ANY);

  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("tf: cli_reply: config: tz=CET;\ncli_reply: ok\n", &td.wbuf_[0]);
  }
  {
  UoutWriterTest td(SO_TGT_FLAG_JSON | SO_TGT_ANY);

  clpar parm[] = { { "config", nullptr }, { "tz", "CET" }, { "tz", "?" } };
  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
    td.sj().write_json();
  }
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{\"tz\":\"CET\"}}", &td.wbuf_[0]);
  }
}
void tst_parm_config_verbose() {
  clpar parm[] = { { "config", nullptr }, { "verbose", "3" }, { "verbose", "?" } };
  {
  UoutWriterTest td(SO_TGT_FLAG_TXT | SO_TGT_ANY);

  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("tf: cli_reply: config: verbose=3;\ncli_reply: ok\n", &td.wbuf_[0]);
  }
  {
  UoutWriterTest td(SO_TGT_FLAG_JSON | SO_TGT_ANY);

  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
    td.sj().write_json();
  }
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{\"verbose\":3}}", &td.wbuf_[0]);
  }
}

void tst_parm_config_longitude() {
  clpar parm[] = { { "config", nullptr }, { "longitude", "13.45" }, { "longitude", "?" } };
  {
  UoutWriterTest td(SO_TGT_FLAG_TXT | SO_TGT_ANY);

  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("tf: cli_reply: config: longitude=13.45;\ncli_reply: ok\n", &td.wbuf_[0]);
  }
  {
  UoutWriterTest td(SO_TGT_FLAG_JSON | SO_TGT_ANY);

  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
    td.sj().write_json();
  }
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{\"longitude\":13.45}}", &td.wbuf_[0]);
  }
}

void tst_parm_config_latitude() {
  clpar parm[] = { { "config", nullptr }, { "latitude", "53.21" }, { "latitude", "?" } };
  {
  UoutWriterTest td(SO_TGT_FLAG_TXT | SO_TGT_ANY);

  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("tf: cli_reply: config: latitude=53.21;\ncli_reply: ok\n", &td.wbuf_[0]);
}
{
  UoutWriterTest td(SO_TGT_FLAG_JSON | SO_TGT_ANY);

  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
    td.sj().write_json();
  }
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{\"latitude\":53.21}}", &td.wbuf_[0]);
}
}


int main() {
  UNITY_BEGIN();

  RUN_TEST(tst_parm_config_latitude);
  RUN_TEST(tst_parm_config_longitude);
  RUN_TEST(tst_parm_config_verbose);
  RUN_TEST(tst_parm_config_tz);
  RUN_TEST(tst_parm_config_cu);
  RUN_TEST(tst_parm_config_empty);

  return UNITY_END();
}

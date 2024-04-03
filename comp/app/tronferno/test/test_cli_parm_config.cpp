#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

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
  UoutWriterTest(so_target_bits tgt = (SO_TGT_FLAG_TXT | SO_TGT_FLAG_JSON | SO_TGT_ANY)) :
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
  UoutWriterTest td;
  const char *txt = &td.wbuf_[0];

  clpar parm[] = { { "config", nullptr }, };
  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("cli_reply: ok\n", txt);
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{}}", td.sj().get_json());
}

void tst_parm_config_cu() {
  UoutWriterTest td;
  const char *txt = &td.wbuf_[0];

  clpar parm[] = { { "config", nullptr }, { "cu", "806789" }, { "cu", "?" } };
  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("tf: cli_reply=0: config: cu=806789;\ncli_reply: ok\n", txt);
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{\"cu\":\"806789\"}}", td.sj().get_json());
}

void tst_parm_config_tz() {
  UoutWriterTest td;
  const char *txt = &td.wbuf_[0];

  clpar parm[] = { { "config", nullptr }, { "tz", "CET" }, { "tz", "?" } };
  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("tf: cli_reply=0: config: tz=CET;\ncli_reply: ok\n", txt);
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{\"tz\":\"CET\"}}", td.sj().get_json());
}
void tst_parm_config_verbose() {
  UoutWriterTest td;
  const char *txt = &td.wbuf_[0];

  clpar parm[] = { { "config", nullptr }, { "verbose", "3" }, { "verbose", "?" } };
  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("tf: cli_reply=0: config: verbose=3;\ncli_reply: ok\n", txt);
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{\"verbose\":3}}", td.sj().get_json());
}

void tst_parm_config_longitude() {
  UoutWriterTest td;
  const char *txt = &td.wbuf_[0];

  clpar parm[] = { { "config", nullptr }, { "longitude", "13.45" }, { "longitude", "?" } };
  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("tf: cli_reply=0: config: longitude=13.45;\ncli_reply: ok\n", txt);
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{\"longitude\":13.45}}", td.sj().get_json());
}

void tst_parm_config_latitude() {
  UoutWriterTest td;
  const char *txt = &td.wbuf_[0];

  clpar parm[] = { { "config", nullptr }, { "latitude", "53.21" }, { "latitude", "?" } };
  if (td.sj().open_root_object("tfmcu")) {
    process_parmConfig(parm, (sizeof parm / sizeof parm[0]), td);
    td.sj().close_root_object();
  }
  TEST_ASSERT_EQUAL_STRING("tf: cli_reply=0: config: latitude=53.21;\ncli_reply: ok\n", txt);
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"tfmcu\",\"config\":{\"latitude\":53.21}}", td.sj().get_json());
}


TEST_CASE("parm_config", "[app_cli]")
{
  tst_parm_config_latitude();
  tst_parm_config_longitude();
  tst_parm_config_verbose();
  tst_parm_config_tz();
  tst_parm_config_cu();
  tst_parm_config_empty();
}

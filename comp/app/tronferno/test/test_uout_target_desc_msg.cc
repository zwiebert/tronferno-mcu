#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

//#define final
#include <uout/uout_writer.hh>
#include <app_uout/so_msg.h>

#include <array>

class UoutWriterTest final : public UoutWriter {
  typedef int (*writeReq_fnT)(void *req, const char *s, ssize_t len, bool final);

public:
  UoutWriterTest(so_target_bits tgt = so_target_bits(SO_TGT_FLAG_TXT | SO_TGT_FLAG_JSON | SO_TGT_ANY)) :
      UoutWriter(tgt) {
  }

  UoutWriterTest(const UoutWriterConsole&) = delete;
  virtual ~UoutWriterTest() {
  }

public:
private:
  virtual int priv_write(const char *s, ssize_t len, bool final)  {
    for (ssize_t i = 0; i < len; ++i,++wbuf_idx_) {
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

void test_td() {
  UoutWriterTest td;
  const char *str = &td.wbuf_[0];

  td.write('a');
  TEST_ASSERT_EQUAL(1, td.wbuf_idx_);
  TEST_ASSERT_EQUAL_STRING("a", str);
  td.write("bcdef");
  TEST_ASSERT_EQUAL_STRING("abcdef", str);
  td.clear_buf();
  TEST_ASSERT_EQUAL_STRING("", str);
}

void test_td2() {
  UoutWriterTest td;
  const char *str = &td.wbuf_[0];

  td.so().root_open("test");
  soMsg_pos_print_gmp(td, {2,3,50});
  td.so().root_close();

  TEST_ASSERT_EQUAL_STRING("A:position: g=2 m=3 p=50;\n", str);
  TEST_ASSERT_EQUAL_STRING("{\"from\":\"test\",\"pct\":{\"23\":50}}", td.sj().get_json());

}


TEST_CASE("td", "[app_uout]")
{
  test_td();
  test_td2();
}

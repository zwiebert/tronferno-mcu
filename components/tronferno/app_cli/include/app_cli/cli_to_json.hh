/*
* \file  Unused experimental file
*/
#include <fernotron_trx/timer_data.h>
#include <fernotron/fer_main.h>
#include <net_http_server/content.hh>

#include <cstdint>
#include <cstdio>

struct ctj_shutter {
  uint8_t g, m;
  char name[32];

  struct {
    uint8_t mvut, mvdt, mvspdt;
  } timings;
  Fer_TimerData automatic;
public:
  int to_json(char *dst, size_t dst_size) const {
    auto n = snprintf(dst, dst_size, //
        R"({"g":%d,"m":%d,"name":"%s","automatic":)", //
        g, m, name);

     n += automatic.to_json(dst + n, dst_size - n - 1, manual_bits.getMember(g, m));
     dst[n++] = '}';
    return n < dst_size ? n : 0;
  }
#if 0
  bool from_json(const char *json);
  bool from_json(JsmnBase::Iterator &it);
#endif
  };



class CtjContentReader final: public ContentReader {
public:
  virtual int open(const char *name, const char *query);
  virtual int read(int fd, char *buf, unsigned buf_size);
  virtual int close(int fd);
private:
  static constexpr unsigned OUR_NMB_FILES = 1;
  struct FileData {
  int objects_read = 0;
  bool is_open = false; // FIXME: only one open file allowed for now
  } m_file_data[OUR_NMB_FILES];
};




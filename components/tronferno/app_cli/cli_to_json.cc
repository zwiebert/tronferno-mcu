#include <app_cli/cli_to_json.hh>
#include "cli_internal.hh"

#include <cstring>

#define DT(x) x



int CtjContentReader::read(int fd, char *buf, unsigned buf_size) {
  if (fd != 0)
    return -1;
  auto &fda = m_file_data[fd];

  if (!fda.is_open)
    return -1;

  return 0;
}

int CtjContentReader::open(const char *name, const char *query) {
  int fd = 0;
  auto &fda = m_file_data[fd];
  if (fda.is_open)
    return -1;

  fda.is_open = true;

  return fd;
}
int CtjContentReader::close(int fd) {
  if (fd != 0)
    return -1;
  auto &fda = m_file_data[fd];

  if (!fda.is_open)
    return -1;

  fda = FileData();
  return 0;
}


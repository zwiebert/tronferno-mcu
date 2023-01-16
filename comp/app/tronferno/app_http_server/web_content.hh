#pragma once

#include "webapp/content.h"


struct file_map {
  const char *uri, *type;
  struct web_content wc;
};

const struct file_map *wc_getContent(const char *uri);

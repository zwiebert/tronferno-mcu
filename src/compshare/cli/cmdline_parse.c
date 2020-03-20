#include "cli_app_cfg.h"
#include "cli/cli.h"
#include "string.h"
#include <ctype.h>
#include <stdlib.h>


clpar cli_par[MAX_PAR];

static char * 
skip_leading_whitespace(char *s) {
  while (*s == ' ')
    *s++ = '\0';

  return s;
}

static char * 
find_next_space_eq_eol(char *s) {
  while (*s != '\0' && *s != ' ' && *s != '=') {
    s++;
  }
  return s;
}
static char * 
find_next_whitespace_or_eol(char *s) {
  while (*s != '\0' && *s != ' ' && *s != '=') {
    s++;
  }
  return s;
}

int 
cli_parseCommandline(char *cl) {
  int p;
  cli_msgid = 0;

  for (p = 0; p < MAX_PAR; ++p) {
    bool isValQuoted = false;

    // trim
    cl = skip_leading_whitespace(cl);

    // eol
    if (*cl == '\0') {
      return p;
    }

    // first word is the key
    cli_par[p].key = cl;

    // skip the key
    cl = find_next_space_eq_eol(cl);

    switch (*cl) {

    // no val follows the key. set val to NULL (interpreted as default)
    case '\0':
    case ' ':
      cli_par[p].val = NULL;
      continue;

      // got val for the key
    case '=':
      // add null terminator to key
      *cl++ = '\0';
      if (*cl == ';' || *cl == ' ') {
        return -1;
      }

      if (*cl == '\"') {
        isValQuoted = true;
        ++cl;
      }

      // eol - error
      if (*cl == '\0') {
        return -1;
      }

      // start of val
      cli_par[p].val = cl;

      // end of val
      if (isValQuoted) {
        char *q = strchr(cl, '\"');
        // quote unbalanced
        if (q == 0) {
          return -1;
        }
        cl = q;

      } else {
        cl = find_next_whitespace_or_eol(cl);
      }

      // terminate val if not already terminated by eol
      if (*cl != '\0') {
        *cl++ = '\0';
      }

      // found global option
      // process it here and remove the key/val
      if (strcmp(cli_par[p].key, "mid") == 0) {
        cli_msgid = atoi(cli_par[p].val);
        --p;
      }

      break;
    }
  }

  return -1;

}

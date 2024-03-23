/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#include <uout/so_out.h>
#include "fernotron_uout/fer_uo_publish.h"
#include "fernotron/gm_set.hh"
#include "utils_misc/int_types.h"

#include "txtio/inout.h"
#include "uout/status_json.hh"
#include "uout/cli_out.h"

#include <unistd.h>
#include <string.h>
#include <utils_misc/cstring_utils.hh>
#include <stdio.h>
#include <time.h>

#ifdef CONFIG_FERNOTRON_UOUT_DEBUG
#define DEBUG
#define DB_INFO 0
#define DT(x) x
#define D(x) x
#else
#define DB_INFO 0
#define DT(x)
#define D(x)
#endif
#define logtag "ferno.uout"


static void so_gmbitmask_to_str(char *dst, Fer_GmSet *mm) {
  int8_t g;
  bool leading_zeros = true;

  for (g = 7; g >= 0; --g) {
    if (leading_zeros && mm->getGroup(g) == 0)
      continue; // no leading zeros
    leading_zeros = false;

    if (mm->getGroup(g) & 0xf0) {
      itoa(mm->getGroup(g), dst, 16);
    } else {
      *dst++ = '0';
      itoa(mm->getGroup(g), dst, 16);
    }
    dst = dst + strlen(dst);
  }
  if (leading_zeros) {
    *dst++ = '0';
    *dst++ = '0';
  }
}


/////////////////////////////////////////////////////////////////////////////////
void soMsg_sep_enable(class UoutWriter &td) {
  D(td.write("sep enable\n"));
}
void soMsg_sep_disable(class UoutWriter &td) {
  D(td.write("sep disable\n"));
}
void soMsg_sep_button_pressed_error(class UoutWriter &td) {
  td.write("error: hardware button is pressed\n");
}

/////////////////////////////////////////////////////////////////////////////////
static uint16_t fer_cuas_msgid;
void soMsg_cuas_start(class UoutWriter &td, uint16_t id) {
  fer_cuas_msgid = id;
  td.write("U: Press Stop on the Fernotron central unit\n");
}

void soMsg_cuas_timeout(class UoutWriter &td) {
  td.write("U: Nothing received\n");
  reply_id_message(td, fer_cuas_msgid, "cuas=time-out", 0);
}

void soMsg_cuas_done(class UoutWriter &td) {
  td.write("U: Central Unit received and stored\n");
  reply_message(td, "cuas=ok", 0);
}

void soMsg_cuas_state(class UoutWriter &td, int state) {
  td.so().print("cuas", state);

}


/////////////////////////////////////////////////////////////////////////////////
int soMsg_timer_print_begin(class UoutWriter &td, const char *tag) {
  td.so().x_open(tag);
  return 0;
}

void soMsg_timer_print_end(class UoutWriter &td) {
  td.so().x_close();
}

int soMsg_timer_begin(class UoutWriter &td, const so_arg_gm_t a) {

  if (so_cco) {
    char val[] = "0";
    val[0] = '0' + a.g;
    td.st().cli_out_x_reply_entry("g", val);
    val[0] = '0' + a.m;
    td.st().cli_out_x_reply_entry("m", val);
  }
  if (so_jco) {
    char dict[] = "autoGM";
    dict[4] = '0' + a.g;
    dict[5] = '0' + a.m;
    td.sj().add_object(dict);
  }
  return 0;
}

void soMsg_timer_end(class UoutWriter &td) {
  td.so().x_close();
}

void soMsg_astro_minutes_print(class UoutWriter &td, const int am) {
    td.so().print("astro-minute", am);
}

/////////////////////////////////////////////////////////////////////////////////

void soMsg_pos_print_gmp(class UoutWriter &td, const so_arg_gmp_t a) {
  if (so_cco) {
    char buf[64];
    if (int n = snprintf(buf, sizeof buf, "A:position: g=%d m=%d p=%d;\n", (int)a.g, (int)a.m, (int)a.p); n > 0 && n < sizeof buf) {
      td.write(buf, n);
    }
  }

  if (so_jco) {
    td.sj().add_object("pct");
    char buf[] = "00";
    buf[0] += a.g;
    buf[1] += a.m;
    td.sj().add_key_value_pair_d(buf, a.p);
    td.sj().close_object();
  }
}

void soMsg_pos_print_gmpa(class UoutWriter &td, const so_arg_gmp_t *a) {
  if (so_cco)
    for (int i = 0; a[i].g <= 7; ++i) {
      char buf[64];
      if (int n = snprintf(buf, sizeof buf, "A:position: g=%d m=%d p=%d;\n", (int)a[i].g, (int)a[i].m, (int)a[i].p); n > 0 && n < sizeof buf) {
        td.write(buf, n);
      }
    }

  if (so_jco) {
    td.sj().add_object("pct");
    for (int i = 0; a[i].g <= 7; ++i) {
      char buf[] = "00";
      buf[0] += a[i].g;
      buf[1] += a[i].m;
      td.sj().add_key_value_pair_d(buf, a[i].p);
    }
    td.sj().close_object();
  }
}

void soMsg_pos_print_mmp(class UoutWriter &td, const so_arg_mmp_t a) {
  if (so_cco) {
    char buf[64];
    if (int n = snprintf(buf, sizeof buf, "U:position: p=%d mm=%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x;\n", a.p,
        a.mm->getGroup(0), a.mm->getGroup(1), a.mm->getGroup(2), a.mm->getGroup(3),
        a.mm->getGroup(4), a.mm->getGroup(5), a.mm->getGroup(6), a.mm->getGroup(7)
        ); n > 0 && n < sizeof buf) {
      td.write(buf, n);
    }
  }
  if (so_jco) {
    for (auto it = a.mm->begin(1); it; ++it) {
      char buf[] = "00";
      buf[0] += it.getG();
      buf[1] += it.getM();
      td.sj().add_key_value_pair_d(buf, a.p);
    }
  }

}

void soMsg_pos_begin(class UoutWriter &td) {
  if (so_cco)
    td.write("U:position:start;\n");
  if (so_jco)
    td.sj().add_object("pct");
}

void soMsg_pos_end(class UoutWriter &td) {
  if (so_cco)
    td.write("U:position:end;\n");
  if (so_jco)
    td.sj().close_object();
}

void soMsg_pair_begin(class UoutWriter &td) {
  td.so().x_open("pair");
}

void soMsg_pair_end(class UoutWriter &td) {
  td.so().x_close();
}

void soMsg_pair_all_begin(class UoutWriter &td) {
  td.so().x_open("all");
}

void soMsg_pair_all_end(class UoutWriter &td) {
  td.so().x_close();
}

void soMsg_pair_print_kmm(class UoutWriter &td, const so_arg_kmm_t a) {
  //td.write("pair a="), td.write(a.key), td.write(" mm="), so_print_gmbitmask(a.mm), td.write(";\n");
  char buf[20];
  so_gmbitmask_to_str(buf, a.mm);
  td.so().print(a.key, buf);

}

void soMsg_pair_print_kmm_single(class UoutWriter &td, const so_arg_kmm_t a) {
  //td.write("pair a="), td.write(a.key), td.write(" mm="), so_print_gmbitmask(a.mm), td.write(";\n");
  char buf[20];
  so_gmbitmask_to_str(buf, a.mm);
  td.so().print("a", a.key);
  td.so().print("mm", buf);

}

#ifdef CONFIG_APP_USE_REPEATER
void soMsg_repeater_begin(class UoutWriter &td) {
  td.so().x_open("repeater");
}

void soMsg_repeater_end(class UoutWriter &td) {
  td.so().x_close();
}
#endif


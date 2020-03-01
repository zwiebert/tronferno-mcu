#include "app/proj_app_cfg.h"


#include <string.h>

#include "fernotron_sep/set_endpos.h"
#include "fernotron_pos/current_state.h"
#include "config/config.h"
#include "fernotron_auto/timer_state.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "userio_app/status_output.h"
#include "fernotron_auto/timer_data.h"
#include "cli_app.h"


const char *Obj_tag="";
#define SET_OBJ_TAG(tag) Obj_tag=(tag)
#define OBJ_TAG (Obj_tag+0)
#define OBJ_TAG_TIMER "timer"
#define OBJ_TAG_CONFIG "config"
#define OBJ_TAG_MCU "mcu"
#define OBJ_TAG_SEND "send"


static void  cli_out_top_tag(void) {
    io_puts("tf: ");
}

static void  cli_out_reply_tag(void) {
  if (msgid) {
    io_puts("reply="), io_putd(msgid), io_puts(": ");
  } else {
    io_puts("reply: ");
  }
}

static void  cli_out_obj_tag(void) {
    io_puts(OBJ_TAG), io_puts(":");
}

static void  cli_out_start_reply(void) {
  cli_out_top_tag();
  cli_out_reply_tag();
  cli_out_obj_tag();
}


static void  cli_out_start_timer_reply(void) {
  SET_OBJ_TAG(OBJ_TAG_TIMER);
  cli_out_start_reply();
}


static void  cli_out_start_config_reply(void) {
  SET_OBJ_TAG(OBJ_TAG_CONFIG);
  cli_out_start_reply();
}

static void  cli_out_start_mcu_reply(void) {
  SET_OBJ_TAG(OBJ_TAG_MCU);
  cli_out_start_reply();
}


typedef void (*void_fun_ptr)(void);
const int OUT_MAX_LEN = 80;

static void  cli_out_entry(void_fun_ptr tag, const char *key, const char *val, int len) {
  static int length;

  if (!key || len == -1) {
    if (length) {
      io_puts(";\n");
      length = 0;
    }
  } else {
    if (key)
      len += strlen(key) + 1;
    if (val)
      len += strlen(val);

    if ((length + len) > OUT_MAX_LEN && length > 0) {
      io_puts(";\n");
      length = 0;
    }
    if (length == 0) {
      tag();
    }
    length += len;
    if (key) {
      io_putc(' '), io_puts(key), io_putc('=');
    }
    if (val) {
      io_puts(val);
    }
  }
}

void   cli_out_set_config(void) {
  SET_OBJ_TAG(OBJ_TAG_CONFIG);
}

void   cli_out_set_x(const char *obj_tag) {
  SET_OBJ_TAG(obj_tag);
}
void cli_out_close() {
  cli_out_x_reply_entry(0, 0, 0);
}
void  cli_out_x_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  cli_out_entry(cli_out_start_reply, key, val, len);
}

void  cli_out_timer_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  cli_out_entry(cli_out_start_timer_reply, key, val, len);
}

void  cli_out_config_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  cli_out_entry(cli_out_start_config_reply, key, val, len);
}

void  cli_out_mcu_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  cli_out_entry(cli_out_start_mcu_reply, key, val, len);
}

int ENR; // error number
void  print_enr(void) {
  io_puts("enr: "), io_putd(ENR), io_putlf();
}

void  msg_print(const char *msg, const char *tag) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  if (msg)
    io_puts(msg);
  if (msgid) {
    io_putc('@');
    io_putd(msgid);
  }
  if (tag) {
    io_putc(':');
    io_puts(tag);
  }
  io_puts(": ");
}

void  warning_unknown_option(const char *key) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  msg_print("warning", "unknown-option"), io_puts(key), io_putc('\n');
}

void  reply_print(const char *tag) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  msg_print("reply", tag);
}

void  reply_message(const char *tag, const char *msg) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
}

void  cli_msg_ready(void) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  io_puts("\nready:\n");
}

void  reply_id_message(u16 id, const char *tag, const char *msg) {
  u16 old_id = msgid;
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;

  msgid = id;
  reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
  msgid = old_id;
}


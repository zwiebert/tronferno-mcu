#include "positions/current_state.h"
#include "main/inout.h"
#include "misc/int_macros.h"
#include "config/config.h"
#include "main/pairings.h"
#include "main/debug.h"

#ifndef DISTRIBUTION
#define DB_INFO 0
#define DT(x)
#define D(x)
#else
#define DB_INFO 0
#define DT(x)
#define D(x)
#endif


#define GRP_COUNT 7
#define MBR_COUNT 7
#define GRP_MAX 7
#define MBR_MAX 7

enum pos { POS_0, POS_50, POS_100, POS_SIZE };
static const uint8_t pos_map[POS_SIZE] = {0, 50, 100};


static gm_bitmask_t positions[POS_SIZE];

static int ICACHE_FLASH_ATTR
get_state(uint32_t a, int g, int m) {
  uint8_t p;

  precond(1 <= g && g <= 7 && 1 <= m && m <= 7);

  for (p=0; p < POS_SIZE; ++p) {
    if (GET_BIT(positions[p][g], m))
      return pos_map[p];
  }
  return -1;
}


static int ICACHE_FLASH_ATTR
set_state(uint32_t a, int g, int m, int position) {
  uint8_t p, gi, mi;
  DT(ets_printf("%s: %d, %d, %d\n", __func__, g, m, position));
  precond(0 <= g && g <= 7 && 0 <= m && m <= 7);
  precond(0 <= position && position <= 100);

    for (gi = 1; gi <= GRP_MAX; ++gi) {
      for (mi = 1; mi <= MBR_MAX; ++mi) {

	if (g == 0 || gi == g)
	  for (p=0; p < POS_SIZE; ++p) {
	    if (m == 0 || mi == m)
	      PUT_BIT(positions[p][gi], mi, (pos_map[p] == position));
	  }
      }
    }
    return 0;
}



int ICACHE_FLASH_ATTR
get_shutter_state(uint32_t a, uint8_t g, uint8_t m) {
  precond(g <= 7 && m <= 7);

  return get_state(a, g, m);
}


int ICACHE_FLASH_ATTR
set_shutter_state(uint32_t a, uint8_t g, uint8_t m, fer_cmd cmd) {
  int position = -1;
  precond(g <= 7 && m <= 7);
  
  DT(ets_printf("%s: %d, %d, %d\n", __func__, (int)g, (int)m, (int)cmd));
#ifdef USE_PAIRINGS
  if (!(a == 0 || a == C.fer_centralUnitID)) {
    gm_bitmask_t gm;
    if (read_pairings(&gm, a))
      for (g=1; g <= GRP_MAX; ++g) {
	for (m=1; m <= MBR_MAX; ++m) {
	  if (GET_BIT(gm[g], m)) {
	    // recursion for each paired g/m
	    set_shutter_state(0, g, m, cmd);
	  }
	}
      }
    return 0;
  }
#endif
  
  if (cmd == fer_cmd_UP)
    position = 100;
  else if (cmd == fer_cmd_DOWN)
    position = 0;
  else if (cmd == fer_cmd_SunDOWN)
    position = 50;

  if (0 <= position && position <= 100) {
    io_puts("U:position: a="), io_print_hex_32(a, false),
      io_puts(" g="), io_putd(g),
      io_puts(" m="), io_putd(m),
      io_puts(" p="), io_putd(position), io_puts(";\n");
  }

  if (position < 0)
    return -1;
  else
    return set_state(a, g, m, position);
}


int ICACHE_FLASH_ATTR
modify_shutter_positions(gm_bitmask_t mm, uint8_t p) {
  uint8_t g, m;

  for (g=1; g <= GRP_MAX; ++g) {
    for (m=1; m <= MBR_MAX; ++m) {
      if (GET_BIT(mm[g], m)) {
	set_state(0, g, m, p);
      }
    }
  }

  io_puts("U:position:"),
    io_puts(" p="), io_putd(p),
    io_puts(" mm=");
  for (g=0; g < 8; ++g) {
    io_putx8(mm[g]);
    if (g < 7)
      io_putc(',');
  }
  io_puts(";\n");

  return 0;
}

int ICACHE_FLASH_ATTR
print_shutter_positions() {
  uint8_t p, g;

  io_puts("U:position:start;\n");
  for (p = 0; p < POS_SIZE; ++p) {
    for (g = 1; g < 8; ++g) {
      // don't print a position with no members
      if (positions[p][g] == 0)
        continue;

      // found a member - now print;

      io_puts("U:position:"), io_puts(" p="), io_putd(pos_map[p]), io_puts(" mm=");
      for (g = 0; g < 8; ++g) {
        io_putx8(positions[p][g]);
        if (g < 7)
          io_putc(',');
      }
      io_puts(";\n");

      break; // printing done, got to next position
    }
  }
  io_puts("U:position:end;\n");
  return 0;

}




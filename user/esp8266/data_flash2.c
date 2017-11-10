/*
 * config_flash2.c
 *
 * read and write multiple data objects with same size from/to flash.
 * (its an array of objects, where each can be written independently)
 *
 * object size cannot be larger than 4092 byte
 *
 * note: configure type of your data and pointer to the object in related header file

 *  Created on: 16.09.2017
 *      Author: bertw
 */

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <mem.h>

#include "main/config.h"
#include "esp_missing_includes.h"

#include "data_flash2.h"

#define NO_CACHE 1

#define DB(x) ((C.app_verboseOutput >= vrbDebug) && (x),1)

#define printf ets_uart_printf
extern int ENR; // error number
enum enr { enr_none, enr_inv_fid, enr_2, enr_3, enr_4, enr_5 };

////////////////// private ///////////////////////////////////////////////////////////////
#define FLASH_MAPPED 0x40200000
#define SECTOR_SIZE 4096
#define BLOCK_SIZE (sizeof (MAGIC_COOKIE) + sizeof (DATA_TYPE))
#define BLOCKS_PER_SECTOR (SECTOR_SIZE / BLOCK_SIZE)

// reserve us some flash_sectors we can access via spi_flash_read() and spi_flash_write() from mem.h
static volatile const uint8_t our_flash_storage[SECTOR_SIZE * FLASH_SECTORS] __attribute__((section(".irom.text"), aligned(4096)));

// byte address of our storage in memory mapped flash ROM
static uint32_t our_start_sector_address;
// number of first sector of our_flash_storage
static uint16_t our_start_sector_number;

// magic cookie to mark the data we save, so we can find it at program start
static uint32_t MAGIC_COOKIE = 0xBEEF0000;

#define addr_getBySecBlk(sec, blk) (our_start_sector_address + (sec) * SECTOR_SIZE + (blk) * BLOCK_SIZE)
#define sec_getBySecIdx(idx) (our_start_sector_number + (idx))

#define cookie_isUsed(cookie) ((cookie) != 0 && ~0 != (cookie))
#define cookie_isMagic(cookie) (((cookie) & 0xffff0000) == MAGIC_COOKIE)

#define fid_isValid(fid)  (0 <= (fid) && (fid) < NMB_FILES)
#define fid_getAddr(fid) (files[fid] + 0)
#define fid_isfileExisting(fid) (files[(fid)] != 0)
#define fid_delete(fid) (files[(fid)] = 0)
#define fid_setAddr(fid, addr) (files[fid] = (addr))
#define fid_makeCookie(fid)  (MAGIC_COOKIE | (fid))
#define fid_getByGM(g, m) (((g)) * 8 + (m))
#define fid_getByCookie(cookie) ((cookie) & 0xffff)

#define NMB_FILES  64
typedef fid_t;

uint32_t files[NMB_FILES];
uint16_t free_block_count;

typedef struct {
	uint32_t cookie;
	DATA_TYPE data;
} block_object;

// move file to a new address
// called to prepare a sector for erase by moving files elsewhere
static bool ICACHE_FLASH_ATTR move_file(uint16_t file_id, uint32_t new_addr) {
	if (!fid_isValid(file_id)) {
		return false;
	} else if (!fid_isfileExisting(file_id)) {
		return false;
	} else {
		block_object buf;
		uint32_t old_addr = fid_getAddr(file_id);
		spi_flash_read(old_addr, &buf.cookie, sizeof(buf));
		spi_flash_write(new_addr, &buf.cookie, sizeof(buf));
		buf.cookie = 0;
		spi_flash_write(old_addr, &buf.cookie, sizeof(buf.cookie));
		fid_setAddr(file_id, new_addr);
		--free_block_count;
		return true;
	}
}

// initialize files table and free_block_count
// called at startup
static void ICACHE_FLASH_ATTR find_all_data_blocks(void) {
	int sec, blk;

	os_bzero(files, sizeof(files));
    free_block_count = 0;

	for (sec = 0; sec < FLASH_SECTORS; ++sec) {
		for (blk = 0; blk < BLOCKS_PER_SECTOR; ++blk) {
			uint32_t cookie, addr;
			addr = addr_getBySecBlk(sec, blk);
			spi_flash_read(addr, &cookie, sizeof(cookie));

			if (~cookie == 0) {
				++free_block_count;
			} else if (cookie_isMagic(cookie)) {
				fid_t fid = fid_getByCookie(cookie);

				if (fid_isValid(fid))
					fid_setAddr(fid, addr);
			}
		}
	}
}

static bool ICACHE_FLASH_ATTR erase_sector_by_index(uint16_t sec_idx) {

}

// find the next free block
static bool ICACHE_FLASH_ATTR find_free_block(uint32_t *dst) {
	static uint16_t sec_idx, block_idx;

	for (; sec_idx < FLASH_SECTORS; ++sec_idx, block_idx = 0) {
		bool sector_used = false;
		for (; block_idx < BLOCKS_PER_SECTOR; ++block_idx) {
			uint32_t cookie;
			uint32_t addr = addr_getBySecBlk(sec_idx, block_idx);
			spi_flash_read(addr, &cookie, sizeof(cookie));

			if (cookie == ~0UL) {
				*dst = addr;
				return true; // free block found ... return it
			} else if (cookie == 0UL) {
				continue; // deleted block ... skip it
			} else {
				sector_used = true;
			}
		}
		if (!sector_used) {  // FIXME: erasing here is not good for wear leveling
			spi_flash_erase_sector(sec_getBySecIdx(sec_idx));
			block_idx = 0;
			*dst = addr_getBySecBlk(sec_idx, block_idx);
			free_block_count += BLOCKS_PER_SECTOR;
			return true;
		}
	}
	sec_idx = 0;
    // try again recursive. should now succeed
	return free_block_count && find_free_block(dst);
}



// FIXME: seems still buggy to me
// populate files table
static bool ICACHE_FLASH_ATTR reclaim_space(void) {
	int sec, blk;

	if (free_block_count > NMB_FILES) {
		return false;
	}

	for (sec = 0; sec < FLASH_SECTORS; ++sec) {
		for (blk = 0; blk < BLOCKS_PER_SECTOR; ++blk) {
			uint32_t cookie, addr;
			addr = addr_getBySecBlk(sec, blk);
			spi_flash_read(addr, &cookie, sizeof(cookie));

            if (cookie_isMagic(cookie)) {
				fid_t fid = fid_getByCookie(cookie);
				uint32_t new_addr = 0;

				if (find_free_block(&new_addr)) {
					move_file(fid, new_addr);
				}
			}
		}

		if (free_block_count > NMB_FILES) {
			return true;
		}
	}
	return true;
}

// find file which ID is pointed to by p
// stores found address in *p
static bool ICACHE_FLASH_ATTR find_data_block(uint32_t *p) {
	int sec, blk;

	for (sec = 0; sec < FLASH_SECTORS; ++sec) {
		for (blk = 0; blk < BLOCKS_PER_SECTOR; ++blk) {
			uint32_t cookie, addr;
			addr = addr_getBySecBlk(sec, blk);

			spi_flash_read(addr, &cookie, sizeof(cookie));
			if (cookie == *p) {
				*p = addr;
				return true;
			}
		}
	}
	return false;
}



// read data from flash and copy it to DATA_PTR
// does nothing if not data was found in flash
static bool ICACHE_FLASH_ATTR read_data2(DATA_TYPE *p, uint16_t file_id) {

	if (!fid_isValid(file_id)) {
		return false;
	} else if (!fid_isfileExisting(file_id)) {
		return false;
	} else {
		spi_flash_read(fid_getAddr(file_id) + sizeof(MAGIC_COOKIE), (uint32_t*) p, sizeof(DATA_TYPE));
	}

	DB(printf("file read: fid=%d\n", (int)file_id));

	return true;
}


static bool ICACHE_FLASH_ATTR delete_file(uint16_t file_id) {
	if (!fid_isValid(file_id)) {
		return false;
	} else if (!fid_isfileExisting(file_id)) {
		return false;
	} else {
		uint32_t cookie = 0;
		spi_flash_write(fid_getAddr(file_id), &cookie, sizeof(cookie));
		fid_delete(file_id);
		DB(printf("file deleted: fid=%d\n", (int)file_id));
		return true;
	}
}

static int ICACHE_FLASH_ATTR delete_shadowded_files(uint8_t group, uint8_t memb) {
	int g, m, result = 0;
	for (g = 0; g <= 7; ++g) {
		for (m = 0; m <= 7; ++m) {
			if ((group == 0 || group == g) && (memb == 0 || memb == m)) {
				if (delete_file(fid_getByGM(g, m))) {
					DB(printf("shadow deleted: g=%d, m=%d, fid=%d\n", (int)g, (int)m, fid_getByGM(g, m)));
					++result;
				}
			}
		}
	}
	return result;
}

// reading data from DATA_PTR and write it to flash
// does nothing if data is unchanged
static bool ICACHE_FLASH_ATTR save_data2(DATA_TYPE *p, uint16_t file_id) {
	uint32_t addr = 0, cookie;

	reclaim_space();

	if (!fid_isValid(file_id)) {
		ENR = enr_inv_fid;
		return false;
	} else if (fid_isfileExisting(file_id)) {
		if (!delete_file(file_id)) {
			ENR = enr_2;
			return false;
		}
	}

	if (find_free_block(&addr)) {
		fid_setAddr(file_id, addr);
		++free_block_count;
	} else {
		ENR = enr_3;
		return false; // FIXME: make room
	}

	cookie = fid_makeCookie(file_id);
	spi_flash_write(fid_getAddr(file_id), &cookie, sizeof(cookie));
	spi_flash_write(fid_getAddr(file_id) + sizeof(cookie), (uint32_t*) p, sizeof(DATA_TYPE));

	DB(printf("file saved: fid=%d\n", (int)file_id));

	return true;

}


////////////////////////////////// public ////////////////////////////////////////////////////////////////////


bool ICACHE_FLASH_ATTR save_timer_data(DATA_TYPE *p, uint8_t g, uint8_t m) {
	delete_shadowded_files(g, m);
	return save_data2(p, fid_getByGM(g, m));
}

bool ICACHE_FLASH_ATTR read_timer_data(DATA_TYPE *p, uint8_t *g, uint8_t *m, bool wildcard) {
	bool result = read_data2(p, fid_getByGM(*g, *m));

	if (!result && wildcard) {
		if ((result = read_data2(p, fid_getByGM(*g, 0)))) {
			*m = 0;
		} else if ((result = read_data2(p, fid_getByGM(0, *m)))) {
			*g = 0;
		} else if ((result = read_data2(p, fid_getByGM(0, 0)))) {
			*g = *m = 0;
		}
	}
	return result;
}


// initialize this software module
void ICACHE_FLASH_ATTR setup_dataFlash2() {

	our_start_sector_address = (uint32_t) &our_flash_storage[0] - FLASH_MAPPED;
	our_start_sector_number = (our_start_sector_address >> (3 * 4));
	find_all_data_blocks();
	DB(ets_uart_printf("setup_dataFlash()\n"));
}


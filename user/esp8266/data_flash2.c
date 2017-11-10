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
#define TEST_THIS_MODULE 0

#if TEST_THIS_MODULE
#define DB(x) x
#define DB2(x)
#else
#define DB(x) ((C.app_verboseOutput >= vrbDebug) && (x),1)
#define DB2(x) DB(x)
#endif
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
#define secn_getBySecIdx(sec) (our_start_sector_number + (sec))

#define sec_getSucc(sec) (sec >= FLASH_SECTORS-1 ? 0 : sec+1)
#define sec_getPred(sec) (sec <= 1) ? FLASH_SECTORS-1 : sec-1)
#define sec_getByAddr(addr) (((addr) - our_start_sector_address) / SECTOR_SIZE)

#define cookie_isUsed(cookie) ((cookie) != 0 && ~0 != (cookie))
#define cookie_isMagic(cookie) (((cookie) & 0xffff0000) == MAGIC_COOKIE)
#define cookie_isFree(cookie) ((cookie) == ~0)
#define cookie_isDeleted(cookie) ((cookie) == 0)

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
int free_block_count;
int free_sec;
bool flag_garbage;

#define fbc_add(val) (free_block_count + (val))
#define fbc_none()  (free_block_count <= 0)
#define fbc_th()    (free_block_count <= NMB_FILES)
#define fbc_addSec() (free_block_count += BLOCKS_PER_SECTOR)
#define fbc_decr()   (--free_block_count)
#define fbc_incr()   (++free_block_count)

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
		return true;
	}
}

// erase sector.
// sector must be full, or the free_block_clock will be wrong after
static void ICACHE_FLASH_ATTR erase_full_sector(uint16_t sec_idx) {
	SpiFlashOpResult result = spi_flash_erase_sector(secn_getBySecIdx(sec_idx));
	DB(printf("erase result %d\n", result));
	fbc_addSec();
	DB(printf("eras_full_sec sec %d, fbc=%d\n", (int)sec_idx, (int)free_block_count));
#if TEST_THIS_MODULE
	{
		uint32_t cookie= 42;
		uint32_t addr = addr_getBySecBlk(sec_idx, 0);
		spi_flash_read(addr, &cookie, sizeof(cookie));
		DB(printf("cookie %lx\n", cookie));

	}
#endif
}

// initialize files table and free_block_count
// called at startup
static void ICACHE_FLASH_ATTR find_all_data_blocks(void) {
	int sec, blk;

	os_bzero(files, sizeof(files));
    free_block_count = 0;
    int used_block_count = 0;
    int deleted_block_count = 0;
    free_sec = -1;

	for (sec = 0; sec < FLASH_SECTORS; ++sec) {
		for (blk = 0; blk < BLOCKS_PER_SECTOR; ++blk) {
			uint32_t cookie, addr;
			addr = addr_getBySecBlk(sec, blk);
			spi_flash_read(addr, &cookie, sizeof(cookie));

			if (cookie == 0) {
				++deleted_block_count;
			} else if (cookie == ~0) {
			     fbc_incr();
				if (free_sec < 0 || used_block_count || deleted_block_count) {
				  free_sec = sec;
				}
			} else if (cookie_isMagic(cookie)) {
				fid_t fid = fid_getByCookie(cookie);
				++used_block_count;
				if (fid_isValid(fid))
					fid_setAddr(fid, addr);
			}
		}
		DB(printf("fadb: sec=%d, fbc=%d, fs=%d, ublk=%d\n", sec, free_block_count, free_sec, used_block_count));
	}


#if 1
	if (fbc_none()) {
		int sec;
		// first run after firmware has installed
		for (sec=0; sec < FLASH_SECTORS; ++sec) {
			erase_full_sector(0);
		}

		free_sec = 0;
	}
#endif
}

// find the next free block
static bool ICACHE_FLASH_ATTR find_free_block(uint32_t *dst) {
	static uint16_t block_idx;
	int i;
#define sec_idx free_sec
	for (i = 0; i < FLASH_SECTORS*2; ++i, (sec_idx = sec_getSucc(sec_idx)), (block_idx = 0)) {
		DB(printf("find_free try: s=%d, b=%d\n", (int)sec_idx, (int)block_idx));
		bool sector_used = block_idx != 0;
		for (; block_idx < BLOCKS_PER_SECTOR; ++block_idx) {
			uint32_t cookie;
			uint32_t addr = addr_getBySecBlk(sec_idx, block_idx);
			spi_flash_read(addr, &cookie, sizeof(cookie));

			if (cookie == ~0UL) {
				*dst = addr;
				fbc_decr();
				if (block_idx == 0) {
					flag_garbage = true;
				}
				DB(printf("find_free_block succ: fb_ct: %d, free_sec: %d, block_idx: %d \n", free_block_count, free_sec, (int)block_idx));
				return true; // free block found ... return it

			} else if (cookie == 0UL) {
				continue; // deleted block ... skip it
			} else {
				sector_used = true;
			}
		}
		if (!sector_used) {  // FIXME: erasing here is not good for wear leveling
			erase_full_sector(sec_idx);
			block_idx = 0;
			*dst = addr_getBySecBlk(sec_idx, block_idx);
			fbc_decr();
			return true;
		}
	}
	DB(printf("find_free_block failed: fbc=%d, free_sec: %d, block_idx: %d \n", free_block_count, free_sec, (int)block_idx));
	return false;
#undef sec_idx
}

#if 0
// FIXME: seems still buggy to me
// populate files table
static bool ICACHE_FLASH_ATTR reclaim_space(void) {
	int sec, blk;

	if (!fbc_th()) {
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
#else

// move away all used files from this sector
// the sector should contain only deletes files after that, so it can be erased if we run out of free blocks
// call this for the sector after the last one with free files in it
static bool ICACHE_FLASH_ATTR reclaim_sector(int sec) {
	int blk;
	int unused_blocks = 0;


		DB(printf("reclaim sec=%d\n", sec));
		for (blk = 0; blk < BLOCKS_PER_SECTOR; ++blk) {
			uint32_t cookie, addr;
			addr = addr_getBySecBlk(sec, blk);
			spi_flash_read(addr, &cookie, sizeof(cookie));

			if (cookie_isFree(cookie)) {
				if (blk == 0) {
					return false;  // sector is empty
				}
				++unused_blocks;
			} else  if (cookie_isMagic(cookie)) {
				fid_t fid = fid_getByCookie(cookie);
				uint32_t new_addr = 0;

				if (find_free_block(&new_addr)) {
					move_file(fid, new_addr);
					DB(printf("move_file fid=%d, from sec=%d, blk=%d; to sec=%d addr=%d\n", fid, sec, blk, sec_getByAddr(new_addr), new_addr));
				}
			}
		}
	erase_full_sector(sec);
	fbc_add(-unused_blocks);
	return true;
}


static bool ICACHE_FLASH_ATTR reclaim_space(void) {
	if (flag_garbage) {
		int i, ignore_sec = free_sec;
		for (i = 0; i < FLASH_SECTORS; ++i) {
			if (i != ignore_sec && i != free_sec) {
				if (reclaim_sector(i)) {
					os_delay_us(20000);
				}
			}
		}
		flag_garbage = false;
	}
	return true;
}
#endif


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
		DB2(printf("file deleted: fid=%d\n", (int)file_id));
		return true;
	}
}

static int ICACHE_FLASH_ATTR delete_shadowded_files(uint8_t group, uint8_t memb) {
	int g, m, result = 0;
	for (g = 0; g <= 7; ++g) {
		for (m = 0; m <= 7; ++m) {
			if ((group == 0 || group == g) && (memb == 0 || memb == m)) {
				if (delete_file(fid_getByGM(g, m))) {
					DB2(printf("shadow deleted: g=%d, m=%d, fid=%d\n", (int)g, (int)m, fid_getByGM(g, m)));
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
	} else {
		ENR = enr_3;
		return false; // FIXME: make room
	}

	cookie = fid_makeCookie(file_id);
	spi_flash_write(fid_getAddr(file_id), &cookie, sizeof(cookie));
#if !TEST_THIS_MODULE
	spi_flash_write(fid_getAddr(file_id) + sizeof(cookie), (uint32_t*) p, sizeof(DATA_TYPE));
#endif

	DB2(printf("file saved: fid=%d\n", (int)file_id));

	return true;

}


////////////////////////////////// public ////////////////////////////////////////////////////////////////////


bool ICACHE_FLASH_ATTR save_timer_data(DATA_TYPE *p, uint8_t g, uint8_t m) {
	bool result = false;

	delete_shadowded_files(g, m);
	result = save_data2(p, fid_getByGM(g, m));
	reclaim_space();

	return result;
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

#if TEST_THIS_MODULE
static void module_selftest(void) {
	DATA_TYPE data = { };
	int i, g, m;

	DB(printf("FS: %d, BPS: %d, \n", FLASH_SECTORS, BLOCKS_PER_SECTOR));

	for (i = 1; i < 6; ++i) {
		os_delay_us(20000);
		for (g = 1; g <= 7; ++g) {
			os_delay_us(500);
			for (m = 1+i; m <= 7; ++m) {
				save_timer_data(&data, g, m);
			}
		}
	}

}
#endif

// initialize this software module
void ICACHE_FLASH_ATTR setup_dataFlash2() {

	our_start_sector_address = (uint32_t) &our_flash_storage[0] - FLASH_MAPPED;
	our_start_sector_number = (our_start_sector_address >> (3 * 4));
	find_all_data_blocks();
	DB(ets_uart_printf("setup_dataFlash()\n"));
#if TEST_THIS_MODULE
	module_selftest();
#endif
}


/*
 * config_flash.c
 *
 * read and write data from/to flash. data cannot be larger than 4092 byte
 * configure type and pointer to your data it in data_flash.h
 *
 * FIXME: better check if data is actually larger (even if its very unlikely)
 *
 *  Created on: 16.09.2017
 *      Author: bertw
 */

#include "data_flash.h"

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <mem.h>

#include "config.h"
#include "esp_missing_includes.h"
extern int ets_uart_printf(const char *fmt, ...);

#define DB(x)
#define printf ets_uart_printf

////////////////// private ///////////////////////////////////////////////////////////////
#define FLASH_MAPPED 0x40200000
#define SECTOR_SIZE 4096
#define BLOCK_SIZE (sizeof (flash_obj))
#define BLOCKS_PER_SECTOR (SECTOR_SIZE / BLOCK_SIZE)

// reserve one or more flash_sectors for us we can read and write to
static volatile const uint8_t flash_sector[SECTOR_SIZE * FLASH_SECTORS] __attribute__((section(".irom.text"), aligned(4096)));

// sector number of our first reserved sector
static uint16_t sector_number;

// current sector number (relative to sector number)
static uint16_t rel_sector_number;
// current block number
static uint16_t block_number;

#define get_flash_sector() (sector_number + rel_sector_number)
#define get_flash_address() (get_flash_sector() * SECTOR_SIZE + block_number * BLOCK_SIZE)

// to verify the read data is the one we saved
#define MAGIC_COOKIE 0xDEADBEEF

static struct {
	uint32_t magic_cookie;
	DATA_TYPE data;
} flash_obj;

static bool ICACHE_FLASH_ATTR
find_free_block(void) {

	for (; rel_sector_number < FLASH_SECTORS; ++rel_sector_number, block_number = 0) {
		for (; block_number < BLOCKS_PER_SECTOR; ++block_number) {
			uint32_t cookie;

			spi_flash_read(get_flash_address(), &cookie, sizeof(cookie));
			switch (cookie) {

			case 0xffffffff:
				return true;
				break;

			case 0x00000000:
				break;

			case MAGIC_COOKIE:
			default:
				// mark block as deleted
				cookie = 0;
				spi_flash_write(get_flash_address(), &cookie, sizeof(cookie));
				break;

			}
		}

		DB(ets_uart_printf("find_free_block(): erase sector %d\n", (int)get_flash_sector()));
		spi_flash_erase_sector(get_flash_sector());

	}
	rel_sector_number = 0;

	return true;
}

static bool ICACHE_FLASH_ATTR
find_data_block() {

	for (; rel_sector_number < FLASH_SECTORS; ++rel_sector_number, block_number = 0) {
		for (; block_number < BLOCKS_PER_SECTOR; ++block_number) {
			uint32_t cookie;

			spi_flash_read(get_flash_address(), &cookie, sizeof(cookie));
			if (cookie == MAGIC_COOKIE)
				return true;

		}

	}

	rel_sector_number = 0;
	return false;
}

static void ICACHE_FLASH_ATTR
write_flash() {
	DB(ets_uart_printf("write_flash(): sn=%d, bn=%d, size=%d\n", (int)(sector_number + rel_sector_number), (int)block_number, (int)sizeof(flash_obj)));
	spi_flash_write(get_flash_address(), (uint32_t*) &flash_obj, sizeof(flash_obj));
}

static void ICACHE_FLASH_ATTR
read_flash() {
	spi_flash_read(get_flash_address(), (uint32_t*) &flash_obj, sizeof(flash_obj));
}

////////////////////////////////// public ////////////////////////////////////////////////////////////////////
// read DATA_TYPE from flash and copy it to global DATA_TYPE
void ICACHE_FLASH_ATTR read_data(void) {
	if (!find_data_block(true))
		return; // nothing in flash. just keep the default values until save_config() is called from code

	read_flash();

	os_memcpy(DATA_PTR, &flash_obj.data, sizeof(DATA_TYPE));

}

// copy global DATA_TYPE and write it to flash
void ICACHE_FLASH_ATTR save_data(void) {
	if (os_memcmp(&flash_obj.data, DATA_PTR, sizeof(DATA_TYPE)) == 0)
		return;  // no changes to update

	flash_obj.magic_cookie = MAGIC_COOKIE;
	os_memcpy(&flash_obj.data, DATA_PTR, sizeof(DATA_TYPE));

	if (find_free_block()) {
		write_flash();
	}
}

void ICACHE_FLASH_ATTR
setup_dataFlash() {
	uint32_t sector_start;

	sector_start = (uint32_t) &flash_sector[0] - FLASH_MAPPED;
	sector_number = (sector_start >> (3 * 4));

	DB(ets_uart_printf("setup_dataFlash()\n"));
}


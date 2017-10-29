/*
 * config_flash.c
 *
 * read and write a single data object from/to flash.
 *
 * the object cannot be larger than 4092 byte
 *
 * note: configure type of your data and pointer to the object in the header file data_flash.h
 *
 * FIXME: better check if data is actually larger than 4092 byte (even if its very unlikely)
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

#include "main/config.h"
#include "esp_missing_includes.h"

#define DB(x)
#define printf ets_uart_printf

////////////////// private ///////////////////////////////////////////////////////////////
#define FLASH_MAPPED 0x40200000
#define SECTOR_SIZE 4096
#define BLOCK_SIZE (sizeof (flash_obj))
#define BLOCKS_PER_SECTOR (SECTOR_SIZE / BLOCK_SIZE)

// reserve us some flash_sectors we can access via spi_flash_read() and spi_flash_write() from mem.h
static volatile const uint8_t our_flash_storage[SECTOR_SIZE * FLASH_SECTORS] __attribute__((section(".irom.text"), aligned(4096)));

// number of first sector of our_flash_storage
static uint16_t our_start_sector_number;

// the index of the sector we currently use (our_start_sector == index 0) ...
static uint16_t our_current_sector_index;
// ... the number of the current sector ...
#define our_current_sector_number (our_start_sector_number + our_current_sector_index)
// ... the byte address of the current sector
#define our_current_sector_address (our_current_sector_number * SECTOR_SIZE + our_current_block_index * BLOCK_SIZE)

// the index of the block we currently use in the current sector
static uint16_t our_current_block_index;

// magic cookie to mark the data we save, so we can find it at program start
#define MAGIC_COOKIE 0xDEADBEEF

static struct {
	uint32_t magic_cookie;
	DATA_TYPE data;
} flash_obj;

// find the next free block
// adjusts current_sector and current_block to point at the free block
// deletes blocks with previously saved or unknown data
// erases all visited sectors without free blocks
static bool ICACHE_FLASH_ATTR find_free_block(void) {

	for (; our_current_sector_index < FLASH_SECTORS; ++our_current_sector_index, our_current_block_index = 0) {
		for (; our_current_block_index < BLOCKS_PER_SECTOR; ++our_current_block_index) {
			uint32_t cookie;

			spi_flash_read(our_current_sector_address, &cookie, sizeof(cookie));
			switch (cookie) {

			case 0xffffffff:  // free block found ... return it
				return true;
				break;

			case 0x00000000:  // deleted block ... skip it
				break;

			case MAGIC_COOKIE: // in-use block found ... delete it
				// fall trough ...
			default:  // unknown data found ... delete it
				cookie = 0;
				spi_flash_write(our_current_sector_address, &cookie, sizeof(cookie));
				break;

			}
		}

		DB(ets_uart_printf("find_free_block(): erase sector %d\n", (int)our_current_sector_number));
		spi_flash_erase_sector(our_current_sector_number);

	}
	our_current_sector_index = 0;

	return true;
}

// find saved data
// adjusts current_sector and current_block to point at the saved data
// erase all visited sectors which only contain deleted blocks
// returns false,  if no saved data was found
static bool ICACHE_FLASH_ATTR find_data_block() {

	for (; our_current_sector_index < FLASH_SECTORS; ++our_current_sector_index, our_current_block_index = 0) {
		for (; our_current_block_index < BLOCKS_PER_SECTOR; ++our_current_block_index) {
			uint32_t cookie;

			spi_flash_read(our_current_sector_address, &cookie, sizeof(cookie));
			if (cookie == MAGIC_COOKIE)
				return true;

		}

	}

	our_current_sector_index = 0;
	return false;
}

// write data to current block
static void ICACHE_FLASH_ATTR write_flash() {
	DB(ets_uart_printf("write_flash(): sn=%d, bn=%d, size=%d\n", (int)(our_start_sector_number + our_current_sector_index), (int)our_current_block_index, (int)sizeof(flash_obj)));
	spi_flash_write(our_current_sector_address, (uint32_t*) &flash_obj, sizeof(flash_obj));
}

// read data from current block
static void ICACHE_FLASH_ATTR read_flash() {
	spi_flash_read(our_current_sector_address, (uint32_t*) &flash_obj, sizeof(flash_obj));
}

////////////////////////////////// public ////////////////////////////////////////////////////////////////////
// read data from flash and copy it to DATA_PTR
// does nothing if not data was found in flash
void ICACHE_FLASH_ATTR read_data(void) {
	if (!find_data_block(true))
		return; // nothing in flash. just keep the default values until save_config() is called from code

	read_flash();

	os_memcpy(DATA_PTR, &flash_obj.data, sizeof(DATA_TYPE));

}

// reading data from DATA_PTR and if write it to flash
// does nothing if data is unchanged
void ICACHE_FLASH_ATTR save_data(void) {
	if (os_memcmp(&flash_obj.data, DATA_PTR, sizeof(DATA_TYPE)) == 0)
		return;  // no changes to update

	flash_obj.magic_cookie = MAGIC_COOKIE;
	os_memcpy(&flash_obj.data, DATA_PTR, sizeof(DATA_TYPE));

	if (find_free_block()) {
		write_flash();
	}
}

// initialize this software module
void ICACHE_FLASH_ATTR setup_dataFlash() {
	uint32_t our_start_sector_address;

	our_start_sector_address = (uint32_t) &our_flash_storage[0] - FLASH_MAPPED;
	our_start_sector_number = (our_start_sector_address >> (3 * 4));

	DB(ets_uart_printf("setup_dataFlash()\n"));
}


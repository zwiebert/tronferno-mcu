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
#include <user_config.h>
#include "data_flash.h"

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <mem.h>

#include "config/config.h"

#define LEAN_MODULE 1

#define NO_CACHE 1

#if LEAN_MODULE || LEAN_ALL
#define DB(x)
#else
#define DB(x) ((C.app_verboseOutput >= vrbDebug) && (x),1)
#endif
#define printf ets_uart_printf
extern int ENR;

////////////////// private ///////////////////////////////////////////////////////////////
#define FLASH_MAPPED 0x40200000
#define SECTOR_SIZE 4096
#define BLOCK_SIZE (sizeof (MAGIC_COOKIE) + sizeof (DATA_TYPE))
#define BLOCKS_PER_SECTOR (SECTOR_SIZE / BLOCK_SIZE)

#ifndef C_DATA_FLASH_ADDR
// reserve us some flash_sectors we can access via spi_flash_read() and spi_flash_write() from mem.h
static volatile const u8 our_flash_storage[SECTOR_SIZE * FLASH_SECTORS] __attribute__((section(".irom.text"), aligned(4096)));
// number of first sector of our_flash_storage
static u16 our_start_sector_number;
#else
static u16 our_start_sector_number = ((u16)((C_DATA_FLASH_ADDR) / 4096));
#undef FLASH_SECTORS
#define FLASH_SECTORS (C_DATA_FLASH_SIZE / 4096)
#endif



// the index of the sector we currently use (our_start_sector == index 0) ...
static u16 our_current_sector_index;
// ... and the number of that current sector ...
#define our_current_sector_number (our_start_sector_number + our_current_sector_index)
// ... and the byte address of that current sector in the memory mapped flash ROM
#define our_current_sector_address (our_current_sector_number * SECTOR_SIZE + our_current_block_index * BLOCK_SIZE)

// the index of the block we currently use in the current sector
static u16 our_current_block_index;

// magic cookie to mark the data we save, so we can find it at program start
static u32 MAGIC_COOKIE = 0xDEADBEEF;


#if ! NO_CACHE
// locally cache the user data in flash_obj to keep track of changes and keep code simple (FIXME: not really that useful, waste of RAM)
static struct {
	u32 magic_cookie;
	DATA_TYPE data;
} flash_obj;
#endif

// find the next free block
// adjusts current_sector and current_block to point at the free block
// deletes blocks with previously saved or unknown data
// erases all visited sectors without free blocks
/// always succeeds
static bool ICACHE_FLASH_ATTR find_free_block(void) {

	for (; our_current_sector_index < FLASH_SECTORS; ++our_current_sector_index, our_current_block_index = 0) {
		for (; our_current_block_index < BLOCKS_PER_SECTOR; ++our_current_block_index) {
			u32 cookie;

			spi_flash_read(our_current_sector_address, &cookie, sizeof(cookie));

			if (cookie == 0xffffffff) {
				return true; // free block found ... return it
			} else if (cookie == 0x00000000) {
				continue; // deleted block ... skip it
			} else {
				cookie = 0;	// in-use or undefined block found ... delete it
			spi_flash_write(our_current_sector_address, &cookie, sizeof(cookie));
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
			u32 cookie;

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
	DB(ets_uart_printf("write_flash(): sn=%d, bn=%d, size=%d\n", (int)(our_start_sector_number + our_current_sector_index), (int)our_current_block_index, (int)BLOCK_SIZE));
#if NO_CACHE
	spi_flash_write(our_current_sector_address, &MAGIC_COOKIE, sizeof (MAGIC_COOKIE));
	spi_flash_write(our_current_sector_address + sizeof (MAGIC_COOKIE), (u32*) DATA_PTR, sizeof (DATA_TYPE));
#else
	spi_flash_write(our_current_sector_address, (u32*) &flash_obj, sizeof(flash_obj));
#endif
}

// read data from current block
static void ICACHE_FLASH_ATTR read_flash() {
#if NO_CACHE
	spi_flash_read(our_current_sector_address + sizeof (MAGIC_COOKIE), (u32*) DATA_PTR, sizeof (DATA_TYPE));
#else
	spi_flash_read(our_current_sector_address, (u32*) &flash_obj, sizeof(flash_obj));
#endif
}

////////////////////////////////// public ////////////////////////////////////////////////////////////////////
// read data from flash and copy it to DATA_PTR
// does nothing if not data was found in flash
void ICACHE_FLASH_ATTR read_data(void) {
	if (!find_data_block(true))
		return; // nothing in flash. just keep the default values until save_config() is called from code

	read_flash();
#if ! NO_CACHE
	os_memcpy(DATA_PTR, &flash_obj.data, sizeof(DATA_TYPE));
#endif
}

// reading data from DATA_PTR and write it to flash
// does nothing if data is unchanged
void ICACHE_FLASH_ATTR save_data(void) {
#if ! NO_CACHE
	if (os_memcmp(&flash_obj.data, DATA_PTR, sizeof(DATA_TYPE)) == 0)
		return;  // no changes to update

	flash_obj.magic_cookie = MAGIC_COOKIE;
	os_memcpy(&flash_obj.data, DATA_PTR, sizeof(DATA_TYPE));
#endif
	if (find_free_block()) {
		write_flash();
	}
}

// initialize this software module
void ICACHE_FLASH_ATTR setup_dataFlash() {
#ifndef C_DATA_FLASH_ADDR
	u32 our_start_sector_address; // byte address of our storage in memory mapped flash ROM

	our_start_sector_address = (u32) &our_flash_storage[0] - FLASH_MAPPED;
	our_start_sector_number = (our_start_sector_address >> (3 * 4));
#endif

	DB(ets_uart_printf("setup_dataFlash()\n"));
}


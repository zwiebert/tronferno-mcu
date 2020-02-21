
#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <mem.h>

#include "app/proj_app_cfg.h"

#if ENABLE_SPIFFS

#include "spiffs_config.h"
#include "spiffs.h"

#define min(X,Y) ((X) < (Y) ? (X) : (Y))

#define printf ets_uart_printf

#if 0
#define DEBUGV printf
#else
#define DEBUGV(...)
#endif

#define SECTOR_SIZE SPI_FLASH_SEC_SIZE
#ifdef C_SPIFFS_FLASH_ADDR
#define PHYS_SIZE C_SPIFFS_FLASH_SIZE
#define PHYS_ADDR C_SPIFFS_FLASH_ADDR
#else
#define PHYS_SIZE 0x8000
#define PHYS_ADDR (0xF0000)  // we start at 1MB of 4MB flash
#endif


#define FLASH_NMB_SECTORS (PHYS_SIZE / SECTOR_SIZE)
#define PHYS_ERASE_BLOCK SPI_FLASH_SEC_SIZE
#define PHYS_LOG_PAGE_SIZE PHYS_ERASE_BLOCK
#define FLASH_FIRST_SECTOR ((PHYS_ADDR) / SECTOR_SIZE)

spiffs fs;

#define LOG_PAGE_SIZE   4096

static u8_t spiffs_work_buf[LOG_PAGE_SIZE * 2];
static u8_t spiffs_fds[32 * 4];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE + 32) * 4];


// read/write/erase functions are taken from arduino/esp8266/spiffs_hal.cpp -bw/20-Nov-17


/*
 spiffs_hal.cpp - SPI read/write/erase functions for SPIFFS.
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 spi_flash_read function requires flash address to be aligned on word boundary.
 We take care of this by reading first and last words separately and memcpy
 relevant bytes into result buffer.
 alignment:       012301230123012301230123
 bytes requested: -------***********------
 read directly:   --------xxxxxxxx--------
 read pre:        ----aaaa----------------
 read post:       ----------------bbbb----
 alignedBegin:            ^
 alignedEnd:                      ^
 */

i32  my_spiffs_read(u32 addr, u32 size, u8 *dst) {
	DEBUGV("spiffs: read 0x%lx, %lu\n", addr, size);

	u32 result = SPIFFS_OK;
	u32 alignedBegin = (addr + 3) & (~3);
	u32 alignedEnd = (addr + size) & (~3);
	if (alignedEnd < alignedBegin) {
		alignedEnd = alignedBegin;
	}

	if (addr < alignedBegin) {
		u32 nb = alignedBegin - addr;
		u32 tmp;
		if (SPI_FLASH_RESULT_OK != spi_flash_read(alignedBegin - 4, &tmp, 4)) {
			DEBUGV("_spif_read(%d) addr=%x size=%x ab=%x ae=%x\r\n",
			__LINE__, addr, size, alignedBegin, alignedEnd);
			return SPIFFS_ERR_INTERNAL;
		}
		memcpy(dst, ((u8*) &tmp) + 4 - nb, nb);
	}

	if (alignedEnd != alignedBegin) {
		if (SPI_FLASH_RESULT_OK != spi_flash_read(alignedBegin, (u32*) (dst + alignedBegin - addr), alignedEnd - alignedBegin)) {
			DEBUGV("_spif_read(%d) addr=%x size=%x ab=%x ae=%x\r\n",
			__LINE__, addr, size, alignedBegin, alignedEnd);
			return SPIFFS_ERR_INTERNAL;
		}
	}

	if (addr + size > alignedEnd) {
		u32 nb = addr + size - alignedEnd;
		u32 tmp;
		if (SPI_FLASH_RESULT_OK != spi_flash_read(alignedEnd, &tmp, 4)) {
			DEBUGV("_spif_read(%d) addr=%x size=%x ab=%x ae=%x\r\n",
			__LINE__, addr, size, alignedBegin, alignedEnd);
			return SPIFFS_ERR_INTERNAL;
		}

		memcpy(dst + size - nb, &tmp, nb);
	}

	return result;
}

/*
 Like spi_flash_read, spi_flash_write has a requirement for flash address to be
 aligned. However it also requires RAM address to be aligned as it reads data
 in 32-bit words. Flash address (mis-)alignment is handled much the same way
 as for reads, but for RAM alignment we have to copy data into a temporary
 buffer. The size of this buffer is a tradeoff between number of writes required
 and amount of stack required. This is chosen to be 512 bytes here, but might
 be adjusted in the future if there are good reasons to do so.
 */


static const int UNALIGNED_WRITE_BUFFER_SIZE = 512;

i32  my_spiffs_write(u32 addr, u32 size, u8 *src) {
	DEBUGV("spiffs: write 0x%lx, %lu\n", addr, size);

	u32 alignedBegin = (addr + 3) & (~3);
	u32 alignedEnd = (addr + size) & (~3);
	if (alignedEnd < alignedBegin) {
		alignedEnd = alignedBegin;
	}

	if (addr < alignedBegin) {
		u32 ofs = alignedBegin - addr;
		u32 nb = (size < ofs) ? size : ofs;
		u8 tmp[4] __attribute__((aligned(4))) = { 0xff, 0xff, 0xff, 0xff };
		memcpy(tmp + 4 - ofs, src, nb);
		if (SPI_FLASH_RESULT_OK != spi_flash_write(alignedBegin - 4, (u32*) tmp, 4)) {
			DEBUGV("_spif_write(%d) addr=%x size=%x ab=%x ae=%x\r\n",
			__LINE__, addr, size, alignedBegin, alignedEnd);
			return SPIFFS_ERR_INTERNAL;
		}
	}

	if (alignedEnd != alignedBegin) {
		u32* srcLeftover = (u32*) (src + alignedBegin - addr);
		u32 srcAlign = ((u32) srcLeftover) & 3;
		if (!srcAlign) {
			if (SPI_FLASH_RESULT_OK != spi_flash_write(alignedBegin, (u32*) srcLeftover, alignedEnd - alignedBegin)) {
				DEBUGV("_spif_write(%d) addr=%x size=%x ab=%x ae=%x, src=%p\r\n",
				__LINE__, addr, size, alignedBegin, alignedEnd, srcLeftover);
				return SPIFFS_ERR_INTERNAL;
			}
		} else {
			u32 sizeLeft;
			u8 buf[UNALIGNED_WRITE_BUFFER_SIZE];
			for (sizeLeft = alignedEnd - alignedBegin; sizeLeft;) {
				size_t willCopy = min(sizeLeft, sizeof(buf));
				memcpy(buf, srcLeftover, willCopy);

				if (SPI_FLASH_RESULT_OK != spi_flash_write(alignedBegin, (u32*) buf, willCopy)) {
					DEBUGV("_spif_write(%d) addr=%x size=%x ab=%x ae=%x\r\n",
					__LINE__, addr, size, alignedBegin, alignedEnd);
					return SPIFFS_ERR_INTERNAL;
				}

				sizeLeft -= willCopy;
				srcLeftover += willCopy;
				alignedBegin += willCopy;
			}
		}
	}

	if (addr + size > alignedEnd) {
		u32 nb = addr + size - alignedEnd;
		u32 tmp = 0xffffffff;
		memcpy(&tmp, src + size - nb, nb);

		if (SPI_FLASH_RESULT_OK != spi_flash_write(alignedEnd, &tmp, 4)) {
			DEBUGV("_spif_write(%d) addr=%x size=%x ab=%x ae=%x\r\n",
			__LINE__, addr, size, alignedBegin, alignedEnd);
			return SPIFFS_ERR_INTERNAL;
		}
	}

	return SPIFFS_OK;
}

i32  my_spiffs_erase(u32 addr, u32 size) {
	u32 i;
	const u32 sector = addr / SPI_FLASH_SEC_SIZE;
	const u32 sectorCount = size / SPI_FLASH_SEC_SIZE;
	for (i = 0; i < sectorCount; ++i) {

		if (SPI_FLASH_RESULT_OK != spi_flash_erase_sector((u16) (sector + i))) {
			DEBUGV("_spif_erase addr=%x size=%d i=%d\r\n", addr, size, i);
			return SPIFFS_ERR_INTERNAL;
		}
	}
	return SPIFFS_OK;
}

int  my_spiffs_mount() {
	spiffs_config cfg;
	cfg.phys_size = PHYS_SIZE;
	cfg.phys_addr = PHYS_ADDR; // start spiffs at start of spi flash
	cfg.phys_erase_block = 65536; // according to datasheet
	cfg.log_block_size = 65536; // let us not complicate things
	cfg.log_page_size = LOG_PAGE_SIZE; // as we said

	cfg.hal_read_f = my_spiffs_read;
	cfg.hal_write_f = my_spiffs_write;
	cfg.hal_erase_f = my_spiffs_erase;

	int res = SPIFFS_mount(&fs, &cfg, spiffs_work_buf, spiffs_fds, sizeof(spiffs_fds), spiffs_cache_buf, sizeof(spiffs_cache_buf), 0);
	printf("spiffs: mount result: %d\n", res);
	return res;
}

void  spiffs_test() {
	char buf[12];

	// Surely, I've mounted spiffs before entering here

	spiffs_file fd = SPIFFS_open(&fs, "my_file", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
	if (fd < 0) {
		printf("1: errno %d\n", (int) SPIFFS_errno(&fs));
		return;
	}

	if (SPIFFS_write(&fs, fd, (u8_t *) "Hello world", 12) < 0) {
		printf("2: errno %d\n", (int) SPIFFS_errno(&fs));
		SPIFFS_close(&fs, fd);
		return;
	}

	SPIFFS_close(&fs, fd);

	if ((fd = SPIFFS_open(&fs, "my_file", SPIFFS_RDWR, 0)) < 0) {
		printf("3: errno %d\n", (int) SPIFFS_errno(&fs));
		return;
	}

	if (SPIFFS_read(&fs, fd, (u8_t *) buf, 12) < 0) {
		printf("4: errno %d\n", (int) SPIFFS_errno(&fs));
		SPIFFS_close(&fs, fd);
		return;
	}

	SPIFFS_close(&fs, fd);
	printf("--> %s <--\n", buf);
}

bool 
spiffs_format_fs(spiffs *fs) {
	if (SPIFFS_mounted(fs)) {
		SPIFFS_unmount(fs);
	} else if (my_spiffs_mount(fs) == 0) {
		SPIFFS_unmount(fs);
	}

#if SPIFFS_USE_MAGIC
	printf("spiffs: format\n");
	SPIFFS_format(&fs);
#else
	printf("spiffs: mass erase sectors: %d\n", FLASH_NMB_SECTORS);
	int i;
	for (i=0; i < FLASH_NMB_SECTORS; ++i) {
		spi_flash_erase_sector(i + FLASH_FIRST_SECTOR);
	}
#endif
	return 0 == my_spiffs_mount();
}


// setup module
void  setup_spiffs(void) {
	int result = my_spiffs_mount(); // FIXME: error handling, like reformat fs
}
#endif



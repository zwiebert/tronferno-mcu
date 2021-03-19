/*
 * cc1101.cc
 *
 *  Created on: Feb 25, 2021
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"

#include "cc1101_ook/spi.hh"
#include "cc1101_ook/trx.hh"

#include "cc1101.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils_misc/int_macros.h"

#ifndef TEST_HOST
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define LCD_HOST    SPI2_HOST
#define SPI_HANDLE spi
#define SPI_HANDLE_ADDR (&spi)

static spi_device_handle_t spi;
static bool rx_mode;
static bool tx_mode;
volatile int marcstate;

static bool ms_delay(unsigned ms) {
  vTaskDelay(ms / portTICK_PERIOD_MS);
  return true;
}

static int Write_CC_CmdStrobe(uint8_t cmd) {

  struct spi_transaction_t ta = { .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA, .length = 8, .rxlength = 8, .tx_data = { cmd, }, };

  if (esp_err_t res = spi_device_transmit(SPI_HANDLE, &ta); res != ESP_OK) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(res);
    return -1;
  }

  return ta.rx_data[0];
}

static int Write_CC_Config(uint8_t addr, uint8_t data) {

  struct spi_transaction_t ta = { .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA, .length = 16, .rxlength = 16, .tx_data = { addr, data, }, };

  if (esp_err_t res = spi_device_transmit(SPI_HANDLE, &ta); res != ESP_OK) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(res);
    return -1;
  }

  return ta.rx_data[1];
}

#define readReg(ra, rt) Read_CC_Register((ra)|(rt))

static int Read_CC_Register(uint8_t addr) {

  struct spi_transaction_t ta = { .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA, .length = 16, .rxlength = 16, .tx_data = { (uint8_t) (addr) }, };

  if (esp_err_t res = spi_device_transmit(SPI_HANDLE, &ta); res != ESP_OK) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(res);
    return -1;
  }

  return ta.rx_data[1];
}

#if 1
bool Read_CC_Burst(uint8_t addr, void *data, size_t len) {
  uint8_t *dst = (uint8_t*) data;
  for (int i = 0; i < len; ++i) {
    if (int rv = Read_CC_Register(addr + i); rv >= 0) {
      dst[i] = rv;
    } else {
      return false;
    }
  }
  return true;
}
#else
bool Read_CC_Burst(uint8_t addr, void *data, size_t len) {

  struct spi_transaction_t ta = { .flags = SPI_TRANS_USE_TXDATA, .length = len * 8, .rxlength = len * 8, .tx_data = { (uint8_t) (addr) } , .rx_buffer = data};

  if (esp_err_t res = spi_device_transmit(SPI_HANDLE, &ta); res != ESP_OK) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(res);
    return;
  }

  return;
}
#endif

static int Read_CC_Config(uint8_t addr) {
  return Read_CC_Register(addr | CC1101_CONFIG_REGISTER);
}

static int Read_CC_Status(uint8_t addr) {
  return Read_CC_Register(addr | CC1101_STATUS_REGISTER);
}

void Write_CC_Burst(const void *data, size_t len) {

  struct spi_transaction_t ta = { .flags = 0, .length = len * 8, .tx_buffer = data, };

  if (esp_err_t res = spi_device_transmit(SPI_HANDLE, &ta); res != ESP_OK) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(res);
    return;
  }

  return;
}

static bool setup_CC_Idle() {
  Write_CC_CmdStrobe(CC1101_SIDLE);

  for (int i = 0; i < 10; ++i) {
    ms_delay(10);
    if (Read_CC_Status(CC1101_MARCSTATE) == 0x01) {
      rx_mode = tx_mode = false;
      return true;
    }
  }

  return false;
}

static bool setup_CC_TX() {
  if (tx_mode)
    return true;
  rx_mode = false;

  Write_CC_CmdStrobe(CC1101_STX); // STX - Transmit mode

  for (int i = 0; i < 10; ++i) {
    ms_delay(10);
    if (Read_CC_Status(CC1101_MARCSTATE) == 0x13) {
      tx_mode = true;
      return true;
    }
  }

  return false;
}

static bool setup_CC_RX() {
  if (rx_mode)
    return true;
  tx_mode = false;

  Write_CC_CmdStrobe(CC1101_SRX);  // SRX  - Receive Mode

  for (int i = 0; i < 10; ++i) {
    ms_delay(10);
    if (Read_CC_Status(CC1101_MARCSTATE) == 0x0d) {
      rx_mode = true;
      return true;
    }
  }
  return false;

}

#define E1 1

const uint8_t cc1101_config[] = {
//
    CC1101_IOCFG2, 0x0D,
    CC1101_IOCFG0, 0x0C,

    // AGC Adjust for OKK
#if E1
    CC1101_AGCCTRL2, 0x87,
#else
    CC1101_AGCCTRL2, 0x04,
#endif
    CC1101_AGCCTRL1, 0x00,
    CC1101_AGCCTRL0, 0x92,


    CC1101_FIFOTHR, 0x47,
    //CC1101_PKTLEN, 0x3E,
    CC1101_PKTCTRL0, 0x32,
    // CC1101_ADDR, 0xFF,
    CC1101_FSCTRL1, 0x06,

    // Set frequency to 433.92 MHz
    CC1101_FREQ2, 0x10,
    CC1101_FREQ1, 0xB0,
    CC1101_FREQ0, 0x71,

#if E1
    CC1101_MDMCFG4, 0x06,
#else
    CC1101_MDMCFG4, 0xF6,
#endif

    CC1101_MDMCFG3, 0xA3,
    CC1101_MDMCFG2, 0x30,

    CC1101_MCSM0, 0x18,
    CC1101_FOCCFG, 0x16,
    CC1101_WORCTRL, 0xFB,

    //CC1101_FREND1, 0x56,
    CC1101_FREND0, 0x11,

    CC1101_FSCAL3, 0xE9,
    CC1101_FSCAL2, 0x2A,
    CC1101_FSCAL1, 0x00,
    CC1101_FSCAL0, 0x1F,

    CC1101_TEST2, 0x81,
    CC1101_TEST1, 0x35,
    CC1101_TEST0, 0x09,

    0xff, 0xff // terminator
    };

bool cc1101_ook_get_regfile(uint8_t *dst, size_t *dst_size) {
  const size_t src_size = sizeof cc1101_config;
  if (src_size > *dst_size) {
    *dst_size = src_size;
    return false;
  }
  *dst_size = src_size;
  memcpy(dst, cc1101_config, src_size);
  return true;
}

static bool cc1101_write_regs(const uint8_t *src) {
  for (int i = 0; src[i] != 0xff; i += 2) {
    Write_CC_Config(src[i], src[i + 1]);
  }
  return true;
}

bool cc1101_ook_upd_regfile(const uint8_t *src) {
  if (!setup_CC_Idle())
    return false;

  cc1101_write_regs(src);

  if (!setup_CC_RX())
    return false;

  return true;
}

bool cc1101_ook_dump_config(uint8_t *buf, size_t *length) {
  if (*length < 48)
    return false;

  if (!Read_CC_Burst(0x00 | READ_BURST, buf, 48))
    return false;

  *length = 48;
  return true;
}

static bool cc1101_init() {
  Write_CC_CmdStrobe(CC1101_SRES);  // SRES - Reset chip
  rx_mode = tx_mode = false;
  ms_delay(100);

  cc1101_write_regs(cc1101_config);

  Write_CC_CmdStrobe(CC1101_SIDLE);  // SIDLE  - Idle state
  for (int i = 0; i < 10; ++i) {
    ms_delay(10);
    if (Read_CC_Status(CC1101_MARCSTATE) == 0x01) {
      return true;
    }
  }
  return false;
}

static void disable_cc1101() {
  if (SPI_HANDLE) {
    spi_bus_remove_device(SPI_HANDLE);
    spi_bus_free(LCD_HOST);
    SPI_HANDLE = nullptr;
  }
}

static bool enable_cc1101(struct cc1101_settings *cfg) {
  spi_bus_config_t buscfg = { .mosi_io_num = cfg->mosi, .miso_io_num = cfg->miso, .sclk_io_num = cfg->sclk, //
      .quadwp_io_num = -1, .quadhd_io_num = -1,
  // .max_transfer_sz=PARALLEL_LINES*320*2+8
      };
  spi_device_interface_config_t devcfg = { //
      .command_bits = 0, .address_bits = 0, .mode = 0,                                //SPI mode 0
          .clock_speed_hz = APB_CLK_FREQ / 20,           //Clock out at 4 MHz
          .spics_io_num = cfg->ss,               //CS pin
          .queue_size = 7,                          //We want to be able to queue 7 transactions at a time
      // .pre_cb = lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
      };

  if (esp_err_t ret = spi_bus_initialize(LCD_HOST, &buscfg, 0); ret == ESP_OK) {
    if (esp_err_t ret = spi_bus_add_device(LCD_HOST, &devcfg, SPI_HANDLE_ADDR); ret == ESP_OK) {
      return true;
    } else {
      ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    }

    spi_bus_free(LCD_HOST);
  } else {
    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
  }

  return false;
}

bool cc1101_ook_setDirection(bool tx) {
  if (!SPI_HANDLE)
    return false;

  if (tx_mode && tx)
    return true;
  if (rx_mode && !tx)
    return true;

  Write_CC_CmdStrobe(CC1101_SIDLE);
  for (int i = 0; i < 10; ++i) {
    ms_delay(10);
    if (Read_CC_Status(CC1101_MARCSTATE) == 0x01) {
      tx_mode = rx_mode = false;
      break;
    }
  }

  if (tx)
    for (int i = 0; i < 10; ++i) {
      if (setup_CC_TX())
        return true;
    }
  else
    for (int i = 0; i < 10; ++i) {
      if (setup_CC_RX())
        return true;
    }

  return false;
}


bool cc1101_ook_gdo_invert(int numb, bool inverted_output) {
  switch (numb) {
  case 2:
    return -1 != Write_CC_Config(CC1101_IOCFG2, 0x0D | (inverted_output ? 0x40 : 0x00));
    break;
  default:
    return false;
  }
  return true;
}

bool cc1101_ook_gdo_hw(int gdo_num, bool high) {
  switch (gdo_num) {
  case 0:
  case 1:
  case 2:
    return -1 != Write_CC_Config(CC1101_IOCFG0 - gdo_num, 0x2f | (high ? 0x40 : 0x00));
  default:
    break;
  }
  return false;
}


bool cc1101_ook_gdo_isConnected(int gdo_num, int gpio_num) {
  bool result = false;
  switch (gdo_num) {
  case 2: {
    result = cc1101_ook_gdo_hw(2, false) && ms_delay(2) && gpio_get_level((gpio_num_t)gpio_num) == 0 && cc1101_ook_gdo_hw(2, true) && ms_delay(2)
        && gpio_get_level((gpio_num_t)gpio_num) == 1;
    cc1101_ook_gdo_invert(2, false);
  }
    break;
  default:
    break;
  }
  return result;
}

void cc1101_ook_spi_setup(struct cc1101_settings *cfg) {
  disable_cc1101();
  if (!cfg->enable) {
    return;
  }

  if (!enable_cc1101(cfg)) {
    return;
  }

  cc1101_init();
  ms_delay(10);
  for (int i = 0; i < 10; ++i) {
    if (setup_CC_RX())
      break;
  }
}


#else

bool cc1101_ook_dump_config(uint8_t *buf, size_t *length) {
  return false;
}
bool cc1101_ook_upd_regfile(const uint8_t *src) {
  return false;
}
bool cc1101_ook_setDirection(bool tx) {
  return false;
}

bool cc1101_ook_gdo_invert(int numb, bool inverted_output) {
  return false;
}

bool cc1101_ook_gdo_isConnected(int gdo_num, int gpio_num) {
  return false;
}

void cc1101_ook_spi_setup(struct cc1101_settings *cfg) {
}

#endif

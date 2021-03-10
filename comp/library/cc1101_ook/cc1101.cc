/*
 * cc1101.cc
 *
 *  Created on: Feb 25, 2021
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"

#include "cc1101_ook/spi.hh"
#include "cc1101_ook/trx.hh"

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

static void ms_delay(unsigned ms) {
  vTaskDelay(ms / portTICK_PERIOD_MS);
}


static int Write_CC_CmdStrobe(uint8_t cmd) {

  struct spi_transaction_t ta = {
      .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
      .length = 8,
      .rxlength = 8,
      .tx_data = { cmd, },
  };

  if (esp_err_t res = spi_device_transmit(SPI_HANDLE, &ta); res != ESP_OK) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(res);
    return -1;
  }

  return ta.rx_data[0];
}


static int Write_CC_Config(uint8_t addr, uint8_t data) {

  struct spi_transaction_t ta = {
      .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
      .length = 16,
      .rxlength = 16,
      .tx_data = { addr, data, },
  };

  if (esp_err_t res = spi_device_transmit(SPI_HANDLE, &ta); res != ESP_OK) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(res);
    return -1;
  }


  return ta.rx_data[1];
}

static int Read_CC_Config(uint8_t addr) {

  struct spi_transaction_t ta = {
      .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
      .length = 16,
      .rxlength = 16,
      .tx_data = { (uint8_t)(addr | 0x80) },
  };

  if (esp_err_t res = spi_device_transmit(SPI_HANDLE, &ta); res != ESP_OK) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(res);
    return -1;
  }


  return ta.rx_data[1];
}

void Write_CC_Burst(const void *data, size_t len) {

  struct spi_transaction_t ta = {
      .flags = 0,
      .length = len * 8,
      .tx_buffer = data,
  };

  if (esp_err_t res = spi_device_transmit(SPI_HANDLE, &ta); res != ESP_OK) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(res);
    return;
  }


  return;
}

static bool setup_CC_TX() {
  if (tx_mode)
    return true;
  rx_mode = false;
  //Write_CC_CmdStrobe(0x33);
  //marcstate = Read_CC_Config(0xF5);

  //Write_CC_Config(0x10, 0xF5);                   //MDMCFG4
  //Write_CC_Config(0x11, 0x93);                   //MDMCFG3

  Write_CC_CmdStrobe(0x35); // STX - Transmit mode

  for (int i = 0; i < 10; ++i) {
    ms_delay(10);
    if (Read_CC_Config(0xF5) == 0x13) {
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
  //Write_CC_CmdStrobe(0x33);
  //marcstate = Read_CC_Config(0xF5);

  ////////////////////////////////////////////////////////////////////////

  //Write_CC_Config(0x10, 0xF7);                   //MDMCFG4
  //Write_CC_Config(0x11, 0x83);                   //MDMCFG3

  Write_CC_CmdStrobe(0x34);  // SRX  - Receive Mode

  for (int i = 0; i < 10; ++i) {
    ms_delay(10);
    if (Read_CC_Config(0xF5) == 0x0d) {
      rx_mode = true;
      return true;
    }
  }
  return false;

}


const uint8_t cc1101_config[] = {

0x00, 0x0D,                   //IOCFG2
    0x02, 0x0C,                   //IOCFG0
#if 1
    // AGC Adjust for OKK
    0x1B, 0x04,                   //AGCCTRL2                              eventuell Anpassen
    0x1C, 0x00,                   //AGCCTRL1
    0x1D, 0x90,                   //AGCCTRL0
#endif

    0x03, 0x47,                   //FIFOTHR
    //0x06, 0x3E,                   //PKTLEN
    0x08, 0x32,                   //PKTCTRL0
    // 0x09, 0xFF,                   //ADDR
    0x0B, 0x06,                   //FSCTRL1

    // Set frequency to 433.92 MHz
    0x0D, 0x10,                   //FREQ2
    0x0E, 0xB0,                   //FREQ1
    0x0F, 0x71,                   //FREQ0

    0x10, 0xF7,                   //MDMCFG4
    0x11, 0x93,                   //MDMCFG3
    0x12, 0x30,                   //MDMCFG2

    0x18, 0x18,                   //MCSM0
    0x19, 0x16,                   //FOCCFG
    0x20, 0xFB,                   //WORCTRL

    //0x21, 0x56,                   //FREND1
    0x22, 0x11,                   //FREND0

    0x23, 0xE9,                   //FSCAL3
    0x24, 0x2A,                   //FSCAL2
    0x25, 0x00,                   //FSCAL1
    0x26, 0x1F,                   //FSCAL0

    0x2C, 0x81,                   //TEST2
    0x2D, 0x35,                   //TEST1
    0x2E, 0x09,                   //TEST0

    //0x41, 0x04,                   //PA_TABLE0
    0x41, 0xC0,                   //PA_TABLE0  // XXX: +10dBm may be too much

    0xff, 0xff // terminator
    };

static bool cc1101_init() {
  Write_CC_CmdStrobe(0x30);  // SRES - Reset chip
  rx_mode = tx_mode = false;
  ms_delay(100);

  for (int i = 0; cc1101_config[i] != 0xff; i += 2) {
    Write_CC_Config(cc1101_config[i], cc1101_config[i + 1]);
  }

  Write_CC_CmdStrobe(0x36);  // SIDLE  - Idle state
  for (int i = 0; i < 10; ++i) {
    ms_delay(10);
    if (Read_CC_Config(0xF5) == 0x01) {
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
      .command_bits = 0,
      .address_bits = 0,
      .mode = 0,                                //SPI mode 0
      .clock_speed_hz = APB_CLK_FREQ/20,           //Clock out at 4 MHz
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

  Write_CC_CmdStrobe(0x36);
  for (int i = 0; i < 10; ++i) {
    ms_delay(10);
    if (Read_CC_Config(0xF5) == 0x01) {
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
  for (int i=0; i < 10; ++i) {
   if (setup_CC_RX())
     break;
  }
}

#else

bool cc1101_ook_setDirection(bool tx) {
  return false;
}
void cc1101_ook_spi_setup(struct cc1101_settings *cfg) {

}

#endif

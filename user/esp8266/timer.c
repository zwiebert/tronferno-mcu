/*
 * timer.c
 *
 *  Created on: 13.09.2017
 *      Author: bertw
 */


#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include "driver/uart.h"

#include "main/inout.h"
#include "main/fer.h"
#include "main/config.h"
#include "main/rtc.h"

#include "esp_missing_includes.h"

#define F_CPU 80000000UL
//////////
///////////////////////////////////////////////////////////
// definitions for RTC Timer1
	#define TIMER1_DIVIDE_BY_1              0x0000
	#define TIMER1_DIVIDE_BY_16             0x0004
	#define TIMER1_DIVIDE_BY_256            0x0008

	#define TIMER1_AUTO_LOAD                0x0040
	#define TIMER1_ENABLE_TIMER             0x0080
	#define TIMER1_FLAGS_MASK               0x00cc

	#define TIMER1_NMI                      0x8000

	#define TIMER1_COUNT_MASK               0x007fffff        // 23 bit timer

	void ICACHE_FLASH_ATTR
	timer1Start(uint32_t ticks, uint16_t flags, void (*handler)(void))
	{
	    RTC_REG_WRITE(FRC1_LOAD_ADDRESS, ticks & TIMER1_COUNT_MASK);
	    RTC_REG_WRITE(FRC1_CTRL_ADDRESS, (flags & TIMER1_FLAGS_MASK) | TIMER1_ENABLE_TIMER);
	    RTC_CLR_REG_MASK(FRC1_INT_ADDRESS, FRC1_INT_CLR_MASK);
	    if (handler != NULL)
	    {
	        if (flags & TIMER1_NMI)
	            ETS_FRC_TIMER1_NMI_INTR_ATTACH(handler);
	        else
	            ETS_FRC_TIMER1_INTR_ATTACH((void (*)(void *))handler, NULL);
	        TM1_EDGE_INT_ENABLE();
	        ETS_FRC1_INTR_ENABLE();
	    }
	}

	void ICACHE_FLASH_ATTR
	timer1Stop(void)
	{
	    ETS_FRC1_INTR_DISABLE();
	    TM1_EDGE_INT_DISABLE();
	    RTC_REG_WRITE(FRC1_CTRL_ADDRESS, 0);
	}
//////////////////////////////////////////////////////////////////////////

void timer_handler(void) {

#ifdef FER_TRANSMITTER
	if (transmTick == C.app_transm) {
		tick_ferSender();
	}
#endif

#ifdef FER_RECEIVER
	if (recvTick == C.app_recv) {
		tick_ferReceiver();
	}
#endif

	if (rtcAvrTime == C.app_rtc) {
		static uint32_t rtc_ticks;


		if (rtc_ticks-- == 0) {
			rtc_ticks = TICK_FREQ_HZ + MS_TO_TICKS(C.app_rtcAdjust);
			rtc_tick();
		}
	}

}



void setup_timer(void)
{
	uint32_t ticks = F_CPU / TICK_FREQ_HZ;
	uint16_t flags = TIMER1_DIVIDE_BY_1 | TIMER1_ENABLE_TIMER | TIMER1_AUTO_LOAD;

	timer1Start(ticks, flags, timer_handler);
}



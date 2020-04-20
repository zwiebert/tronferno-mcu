#pragma once

// imaginary "data clock" which is "four times preamble frequency"
#define BASE_CLOCK       5000UL  // 5kHz . 200us
#define DEFAULT_DATA_CLOCK_HZ  5000 // 5kHz
#define DEFAULT_DATA_CLOCK_US  200  // 200us (us=1/MHz)

//// interrupt timings
#define INTR_TICK_FREQ_MULT 4    // 1=200us, 2=100us, 4=50us, 8=25us

#define TICK_FREQ_HZ             (BASE_CLOCK * INTR_TICK_FREQ_MULT)
#define TICK_PERIOD_US           (1000000UL / TICK_FREQ_HZ)
#define TICK_PERIOD_MS           (TICK_PERIOD_US * 1000UL)
#define DEFAULT_DATA_CLOCK_TICKS US_TO_TICKS(DEFAULT_DATA_CLOCK_US)
#define DATA_CLOCK_TO_TICKS(rel) (((rel) * DEFAULT_DATA_CLOCK_US) / TICK_PERIOD_US)

#define HUS_TO_TICKS(hus) (((hus) * 100 ) / TICK_PERIOD_US)
#define US_TO_TICKS(us) ((us) / TICK_PERIOD_US)
#define MS_TO_TICKS(ms) ((ms) / TICK_PERIOD_MS)


/**
 * \file fernotron_trx/isr_timer_config.h
 * \brief configure timer interrupt
 */

#pragma once

/**
 * \brief   The lowest possible tick frequency
 * \note    Determined by the shortest required TX timing, which is 200us
 */
#define BASE_CLOCK       5000UL  // 5kHz . 200us


/**
 * \brief Determine the frequency for calling TX-ISR
 * \note  Should be kept at 1 (200us).
 */
#define FER_TX_FMULT          1 // 1=200us
#define FER_TX_TICK_HZ        (BASE_CLOCK * FER_TX_FMULT)  ///< calculated TX-tick frequency in Hz
#define FER_TX_TICK_US        (1000000UL / FER_TX_TICK_HZ) ///< calculated TX-tick frequency in us
#define FER_TX_US_TO_TCK(us)  ((us) / FER_TX_TICK_US)      ///< convert a us-timing to TX-tick timing


/**
 * \brief  Determine the frequency for calling RX-ISR (= sample frequency)
 * \note   A higher sample frequency may improve message receiving, but will may also increase CPU load
 */
#define FER_RX_FMULT          4    // 1=200us, 2=100us, 4=50us, 8=25us
#define FER_RX_TICK_HZ        (BASE_CLOCK * FER_RX_FMULT)  ///< calculated RX-tick frequency in Hz
#define FER_RX_TICK_US        (1000000UL / FER_RX_TICK_HZ) ///< calculated RX-tick frequency in us
#define FER_RX_US_TO_TCK(us)  ((us) / FER_RX_TICK_US)      ///< convert a us-timing to RX-tick timing




/**
 * \brief  Determine the frequency of the main timer ISR
 * \note   Determined by the highest required frequency (which would be RX-ISR frequency)
 */
#define FER_ISR_FMULT          FER_RX_FMULT
#define FER_ISR_TICK_HZ        (BASE_CLOCK * FER_ISR_FMULT)  ///< calculated ISR-tick frequency in Hz
#define FER_ISR_TICK_US        (1000000UL / FER_ISR_FMULT)   ///< calculated ISR-tick frequency in us
#define FER_US_TO_ISR_TCK(us)  ((us) / FER_ISR_TICK_US)      ///< convert a us-timing to ISR-tick timing

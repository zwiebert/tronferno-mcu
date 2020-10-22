/**
 * \file fernotron_trx/isr_timer_config.h
 * \brief configure timer interrupt
 */

#pragma once

/**
 * \brief   The lowest possible tick frequency
 * \note    Determined by the shortest required TX timing, which is 200us
 */
constexpr unsigned BASE_CLOCK_US = 200UL; // 5kHz . 200us

/**
 * \brief Determine the frequency for calling TX-ISR
 * \note  Should be kept at 1 (200us).
 */
constexpr unsigned FER_TX_FMULT = 1; // 1=200us
constexpr unsigned FER_TX_TICK_US = (BASE_CLOCK_US / FER_TX_FMULT); ///< calculated TX-tick frequency in us
constexpr unsigned FER_TX_US_TO_TCK(unsigned us)  { return ((us) / FER_TX_TICK_US); }      ///< convert a us-timing to TX-tick timing

/**
 * \brief  Determine the frequency for calling RX-ISR (= sample frequency)
 * \note   A higher sample frequency may improve message receiving, but will may also increase CPU load
 */
constexpr unsigned FER_RX_FMULT = 4   ; // 1=200us, 2=100us, 4=50us, 8=25us
constexpr unsigned FER_RX_TICK_US = (BASE_CLOCK_US / FER_RX_FMULT); ///< calculated RX-tick frequency in us
constexpr unsigned FER_RX_US_TO_TCK(unsigned us)  { return ((us) / FER_RX_TICK_US); }       ///< convert a us-timing to RX-tick timing

/**
 * \brief  Determine the frequency of the main timer ISR
 * \note   Determined by the highest required frequency (which would be RX-ISR frequency)
 */
constexpr unsigned FER_ISR_FMULT = FER_RX_FMULT;
constexpr unsigned FER_ISR_TICK_US = (BASE_CLOCK_US / FER_ISR_FMULT)  ; ///< calculated ISR-tick frequency in us
constexpr unsigned FER_ISR_US_TO_TCK(unsigned us)  { return ((us) / FER_ISR_TICK_US); }      ///< convert a us-timing to ISR-tick timing


static_assert(2 == FER_TX_US_TO_TCK(400));
static_assert(8 == FER_RX_US_TO_TCK(400));
static_assert(8 == FER_ISR_US_TO_TCK(400));

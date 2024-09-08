#pragma once

#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/cm3/systick.h>
    #include <libopencm3/stm32/rcc.h>
#endif

#include <stdint.h>

// RCC Clock Frequency [Hz]
static const int RCC_CLOCK_FREQ_HZ = 84000000;

// Interruptions = 10kHz = 100us (beaucoup ?)
static const int SYSTICK_FREQ_HZ = 1000;
static int SYSTICK_PERIOD = RCC_CLOCK_FREQ_HZ / SYSTICK_FREQ_HZ;

static int MICROS_SYSTICK_RATIO = SYSTICK_PERIOD / 1000;


void clock_setup();

uint32_t get_uptime_ms();
uint32_t get_uptime_us();

void delay_ms(uint32_t ms);

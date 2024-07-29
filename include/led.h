#pragma once

#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/stm32/rcc.h>
    #include <libopencm3/stm32/gpio.h>
    #include <libopencm3/cm3/nvic.h>
#endif

#include "config.h"


void ledSetup(void);

void led1_clear(void);
void led2_clear(void);
void led1_set(void);
void led2_set(void);
void led1_toggle(void);
void led2_toggle(void);


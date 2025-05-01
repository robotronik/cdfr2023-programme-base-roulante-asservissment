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

void RedLED_Clear(void);
void GreenLED_Clear(void);
void RedLED_Set(void);
void GreenLED_Set(void);
void RedLED_Toggle(void);
void GreenLED_Toggle(void);


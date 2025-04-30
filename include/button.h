#pragma once

#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/stm32/rcc.h>
    #include <libopencm3/stm32/gpio.h>
    #include <libopencm3/cm3/nvic.h>
#endif

#include "config.h"


void buttonSetup(void);
bool readPushButton(void);
bool readTestButton(void);
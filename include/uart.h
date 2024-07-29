#pragma once

#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/stm32/rcc.h>
    #include <libopencm3/stm32/gpio.h>
    #include <libopencm3/stm32/usart.h>
#endif

#include "config.h"

void usartSetup(void);

//Send information on uart2
//1000 char max
void usartprintf(const char* format,...);


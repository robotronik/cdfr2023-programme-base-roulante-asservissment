#pragma once

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include "config.h"

void usartSetup(void);

//Send information on uart2
//1000 char max
void usartprintf(const char* format,...);


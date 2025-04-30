#pragma once

#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/cm3/nvic.h>
    #include <libopencm3/stm32/exti.h>
#endif

#include <math.h>

#include "config.h"
#include "clock.h"
#include "uart.h"
#include "AsservissementMath.h"

void odometrySetup(void);
void odometryLoop(position_t &position);
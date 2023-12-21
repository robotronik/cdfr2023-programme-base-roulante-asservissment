#pragma once

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

#include "config.h"


void ledSetup(void);

void led1_clear(void);
void led2_clear(void);
void led1_set(void);
void led2_set(void);
void led1_toggle(void);
void led2_toggle(void);


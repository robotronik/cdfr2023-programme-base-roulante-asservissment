#pragma once

#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>

#include <string.h>

#include "config.h"
#include "uart.h"


#define ADDRI2CBASEROULANTE 42

#define BUFFERSIZE 10
#define BUFFERCIRCULARSIZE 10

void i2c_setup(void);

void setCallback(void (*f)(void));
void disableCallback(void);

bool I2CAvaillable(void);
void I2CGetCommand(uint8_t* data);

void I2CLoop(void);
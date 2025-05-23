#pragma once

#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/stm32/i2c.h>
    #include <libopencm3/stm32/gpio.h>
    #include <libopencm3/stm32/rcc.h>
    #include <libopencm3/cm3/nvic.h>
    #include <libopencm3/stm32/usart.h>
    #include <libopencm3/stm32/gpio.h>
    #include <libopencm3/stm32/exti.h>
#endif

#include <string.h>

#include "config.h"
#include "uart.h"


#define ADDRI2CBASEROULANTE 42


typedef enum{DIRRECEIVE,DIRSEND}commnucationDirection_t;

void i2c_setup(void);

void setCallbackReceive(void (*f)(uint8_t* data, int size));
void disableCallbackReceive(void);
void setCallbackSend(void (*f)(void));
void disableCallbackSend(void);

void I2CSetBuffer(uint8_t* data, int size);
void I2CGetBuffer(uint8_t* data, int size);

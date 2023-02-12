#pragma once

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include "config.h"

void usartSetup(void);
void uartSend(uint16_t data);
void uartSendln(uint16_t data);
void usart_send_int(int Message);
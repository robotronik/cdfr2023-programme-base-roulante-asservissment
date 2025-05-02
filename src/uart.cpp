#include "uart.h"
#include <stdio.h>
#include <stdarg.h>
#include "config.h"

#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/stm32/rcc.h>
    #include <libopencm3/stm32/gpio.h>
    #include <libopencm3/stm32/usart.h>
#endif


void _uartClock_setup(void);
void gpio_setup(void);

void usartSetup(void){

    _uartClock_setup();
	gpio_setup();

	/* Setup USART2 parameters. */
	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART2);
}

void usartSendMessage(uint32_t usart, char* Message){
	int i = 0;
	while (Message[i] != 0)	{
		usart_send_blocking(usart,Message[i]);
		i++;
	}
}

void usartprintf(const char* format, ...) {
    char buffer[1000];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 1000, format, args);
    usartSendMessage(USART2,buffer);
    va_end(args);
}


void _uartClock_setup(void){
	/* Enable GPIOD clock for LED & USARTs. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Enable clocks for USART2. */
	rcc_periph_clock_enable(RCC_USART2);
}

void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Setup GPIO pins for USART2 transmit. */
	gpio_mode_setup(port_TX, GPIO_MODE_AF, GPIO_PUPD_NONE, pin_TX);

	/* Setup USART2 TX pin as alternate function. */
	gpio_set_af(port_TX, GPIO_AF7, pin_TX);
}
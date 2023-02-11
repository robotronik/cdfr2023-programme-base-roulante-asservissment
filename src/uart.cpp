
#include "uart.h"
#include <stdio.h>

void clock_setup(void);
void gpio_setup(void);

void usartSetup(void){

    clock_setup();
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

void uartSend(uint16_t data){
    usart_send_blocking(USART2, data + '0');
}

void uartSendln(uint16_t data){
    usart_send_blocking(USART2, data + '0');
    usart_send_blocking(USART2, '\r');
	usart_send_blocking(USART2, '\n');
}

void uartSendMessage(char* Message){
	int i = 0;
	while (Message[i] != 0 && i <20)	{
		usart_send_blocking(USART2,Message[i]);
		i++;
	}	
}

void usart_send_int(int Message){
	char Buffer[20];
	sprintf(Buffer,"%d",Message);
	uartSendMessage(Buffer);
	usart_send_blocking(USART2, '\r');
	usart_send_blocking(USART2, '\n');
}


void clock_setup(void){
	/* Enable GPIOD clock for LED & USARTs. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Enable clocks for USART2. */
	rcc_periph_clock_enable(RCC_USART2);
}

void gpio_setup(void)
{
	/* Setup GPIO pins for USART2 transmit. */
	gpio_mode_setup(port_TX, GPIO_MODE_AF, GPIO_PUPD_NONE, pin_TX);

	/* Setup USART2 TX pin as alternate function. */
	gpio_set_af(port_TX, GPIO_AF7, pin_TX);
}
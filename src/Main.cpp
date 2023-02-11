#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>

#include "pinConfig.h"
#include "motor.h"
#include "uart.h"


static void gpio_setup(void)
{
	gpio_mode_setup(port_led1, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led1);
	gpio_mode_setup(port_led2, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led2);

	//gpio_mode_setup(GPIOA,GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO5);
	

}


static void clock_setup(void)
{
	//Swap clock to 84MHZ internal
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);

	/* Enable GPIO clock. */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	//rcc_periph_clock_enable(RCC_GPIOD);

}


int main(void)
{
	int i;
	clock_setup();
	gpio_setup();

	motorSetup();
	usartSetup();


	for (i = 0; i < 100000000; i++) {	/* Wait a bit. */
		__asm__("nop");
	}

	motorBrakeR(0);
	motorBrakeL(0);

	/* Blink the LED (PC8) on the board. */
	int speed = -100;
	int c =0;
	int j = 0;
	gpio_toggle(port_directionL, pin_directionL);
	uartSendln(1);
	while (1) {
		
		// if(gpio_get(port_info12,pin_info12)){
		// 	gpio_set(port_led1,pin_led1);
		// }
		// else{
		// 	gpio_clear(port_led1,pin_led1);
		// }
		
		// if(gpio_get(port_info22,pin_info22)){
		// 	gpio_set(port_led2,pin_led2);
		// }
		// else{
		// 	gpio_clear(port_led2,pin_led2);
		// }

		// for(int speed2 = -100 ;speed2<0;speed2+=3){
		// 	gpio_toggle(GPIOA, GPIO15);	/* LED on/off */
		// 	for (i = 0; i < 1000000; i++) {	/* Wait a bit. */
		// 		__asm__("nop");
		// 	}
		// 	motorSpeedSignedL(speed2);
		// 	motorSpeedSignedR(speed2);
		// }

		// if(gpio_get(port_info1L,pin_info1L)){
		// 	gpio_set(port_led1,pin_led1);
		// }
		// else{
		// 	gpio_clear(port_led1,pin_led1);
		// }
		
		// if(gpio_get(port_info2L,pin_info2L)){
		// 	gpio_set(port_led2,pin_led2);
		// }
		// else{
		// 	gpio_clear(port_led2,pin_led2);
		// }
		
		for(int speed2 =0;speed2<95;speed2+=4){
			for (i = 0; i < 1000000; i++) {	/* Wait a bit. */
				__asm__("nop");
			}
			motorSpeedUnsignedR(speed2);
			motorSpeedUnsignedL(speed2);
		}
		// for (i = 0; i < 100000000; i++) {	/* Wait a bit. */
		// 	__asm__("nop");
		// }
		uartSendln(2);
		for(int speed2 =95;speed2>0;speed2-=4){
			for (i = 0; i < 1000000; i++) {	/* Wait a bit. */
				__asm__("nop");
			}
			motorSpeedUnsignedR(speed2);
			motorSpeedUnsignedL(speed2);
		}
		gpio_toggle(GPIOA, GPIO15);	/* LED on/off */
		gpio_toggle(GPIOA, GPIO12);
		uartSendln(3);
		gpio_toggle(port_directionL, pin_directionL);
		gpio_toggle(port_directionR, pin_directionR);
		// usart_send_int(c); /* USART2: Send byte. */
		// c = (c == 9) ? 0 : c + 1;	/* Increment c. */
		// if ((j++ % 80) == 0) {		/* Newline after line full. */
		// 	uartSendln(c);
		// }
		// for (i = 0; i < 300000; i++) {	/* Wait a bit. */
		// 	__asm__("NOP");
		// }
		
		
	}

	return 0;
}
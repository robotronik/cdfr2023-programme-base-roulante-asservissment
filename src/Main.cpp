#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>

#include "config.h"
#include "motor.h"
#include "uart.h"
#include "odometrie.h"


static void ledSetup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(port_led1, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led1);
	gpio_mode_setup(port_led2, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led2);

	//gpio_mode_setup(GPIOA,GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO5);
	

}


static void clock_setup(void)
{
	//Swap clock to 84MHZ internal
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);

}


int main(void)
{

	//SETUP
	clock_setup();
	ledSetup();
	motorSetup();
	usartSetup();
	odometrieSetup();


	//WAIT
	for (int i = 0; i < 100000000; i++) {	/* Wait a bit. */
		__asm__("nop");
	}

	//UNBRAKE MOTOR
	motorBrakeR(0);
	motorBrakeL(0);

	//TEST UART
	uartSendln(1);

	//TEST MOTOR
		//motorSpeedSignedL(-100);
		//motorSpeedSignedR(100);

	//LOOP
	while (1) {
		

		for (int i = 0; i < 300000; i++) {	/* Wait a bit. */
			__asm__("NOP");
		}
		
		
	}

	return 0;
}
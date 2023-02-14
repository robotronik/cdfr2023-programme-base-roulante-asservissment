#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>

#include "config.h"
#include "motor.h"
#include "uart.h"
#include "odometrie.h"
#include "clock.h"


static void ledSetup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(port_led1, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led1);
	gpio_mode_setup(port_led2, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led2);

	//gpio_mode_setup(GPIOA,GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO5);
	

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

	//TEST MOTOR
		//motorSpeedSignedL(-100);
		//motorSpeedSignedR(100);

	//LOOP
	while (1) {
		

		// delay_ms(1000);
		// double f = 5.5;
		// int test = get_uptime_ms();
		// usartprintf("%d\n",test/1000);
		odometrieLoop();
		printPosition();
		
		
	}

	return 0;
}
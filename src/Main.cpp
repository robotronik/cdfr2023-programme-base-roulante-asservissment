#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>

#include "config.h"
#include "motor.h"
#include "uart.h"
#include "odometrie.h"
#include "clock.h"
#include "I2C.h"


static void ledSetup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(port_led1, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led1);
	gpio_mode_setup(port_led2, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led2);

	//gpio_mode_setup(GPIOA,GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO5);
}


void I2CRecieveData(uint8_t* data, int size){
	if(data[0]==10){
		gpio_set(port_led1,pin_led1);
	}
	else if (data[0]==11){
		gpio_clear(port_led1,pin_led1);
	}
	if(data[0]==12){
		gpio_set(port_led2,pin_led2);
	}
	else if (data[0]==13){
		gpio_clear(port_led2,pin_led2);
	}
	else if (data[0]==20){
		position_u posi = odometrieGetPosition();
		I2CSetBuffer(posi.tab,6);
	}
	else if( data[0]==21 && size == 7){
		position_u posi;
		memcpy(posi.tab, data+1, 6);
		odometrieSetPosition(posi);
	}
	
}


int main(void)
{

	//SETUP
	clock_setup();
	ledSetup();
	motorSetup();
	usartSetup();
	odometrieSetup();
	i2c_setup();	
	setCallbackReceive(I2CRecieveData);



	//WAIT
	for (int i = 0; i < 100000000; i++) {	/* Wait a bit. */
		__asm__("nop");
	}
	usartprintf("Start\n");

	//UNBRAKE MOTOR
	motorBrakeR(0);
	motorBrakeL(0);

	//TEST MOTOR
		//motorSpeedSignedL(-100);
		//motorSpeedSignedR(100);
	
	//LOOP
	while (1) {
		odometrieLoop();
		//printPosition();
		// delay_ms(1000);
		// double f = 5.5;
		// int test = get_uptime_ms();
		// usartprintf("%d\n",test/1000);
		//odometrieLoop();
		//printPosition();
		//printBuffer();
		//I2CLoop();
		// while(I2CAvaillable()){
		// 	uint8_t data[10];
		// 	I2CGetCommand(data);
		// 	//usartprintf("test\n");
		// 	usartprintf("%s\n",data);
		// }
		//delay_ms(2000);
		//delay_ms(500);
		
	}

	return 0;
}
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
#include "Asservissement.h"


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
		position_u posi = odometrieGetPositionInt();
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
	asservissementSetup();



	//WAIT
	for (int i = 0; i < 100000000; i++) {	/* Wait a bit. */
		__asm__("nop");
	}
	usartprintf("Start\n");

	//UNBRAKE MOTOR
	motorBrakeR(0);
	motorBrakeL(0);

	motorSetModeR(0);
	motorSetModeL(0);

	//TEST MOTOR
		// motorSpeedSignedL(5);
		// motorSpeedSignedR(5);
		// while (1);

		// for (int i = 0; i < 100; i++){
		// 	motorSpeedSignedL(i);
		// 	motorSpeedSignedR(i);
		// 	delay_ms(50);
		// }
		// for (int i = 100; i > -100; i--){
		// 	motorSpeedSignedL(i);
		// 	motorSpeedSignedR(i);
		// 	delay_ms(50);
		// }

		// for (int i = -100; i < 0; i++){
		// 	motorSpeedSignedL(i);
		// 	motorSpeedSignedR(i);
		// 	delay_ms(50);
		// }


		
	
	//LOOP

	uint32_t asservissementTime =  get_uptime_ms();
	uint32_t changePointTime =  get_uptime_ms()+5000;
	uint32_t changePointTime2 =  get_uptime_ms()+15000;
	uint32_t changePointTime3 =  get_uptime_ms()+25000;
	
	

	
	

	while (1) {
		odometrieLoop();
		if(asservissementTime < get_uptime_ms()){
			//usartprintf("retard : %d		",asservissementTime-get_uptime_ms());
			asservissementTime = get_uptime_ms() + 50;
			asservissementLoop();
			printPosition();
			usartprintf(">infoL1:%d\n>infoL2:%d\n>infoR1:%d\n>infoR2:%d\n",gpio_get(port_info1L,pin_info1L),gpio_get(port_info2L,pin_info2L),gpio_get(port_info1R,pin_info1R),gpio_get(port_info2R,pin_info2R));
		}

		if(changePointTime < get_uptime_ms()){
			changePointTime = get_uptime_ms() + 5000000;
			setLinearAsservissement(-1000,500,true);
			//setAngularAsservissement(-90);
		}
		if(changePointTime2 < get_uptime_ms()){
			changePointTime2 = get_uptime_ms() + 5000000;
			setLinearAsservissement(0,0,true);
			//setAngularAsservissement(-90);
		}

		if(changePointTime3 < get_uptime_ms()){
			changePointTime3 = get_uptime_ms() + 5000000;
			//setLinearAsservissement(0,0);
			setAngularAsservissement(0);
		}


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
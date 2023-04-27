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
		asservissementSetup();
	}
	else if( data[0]==30 && size == 7){
		uintConv x,y,arriere;
		x.tab[0] = data[1]; x.tab[1] = data[2];
		y.tab[0] = data[3]; y.tab[1] = data[4];
		arriere.tab[0] = data[5]; arriere.tab[1] = data[6];
		setLinearAsservissement((double)x.num,(double)y.num,(double)arriere.num);
	}
	else if( data[0]==31 && size == 3){
		uintConv teta;
		teta.tab[0] = data[1]; teta.tab[1] = data[2];
		setAngularAsservissement((double)teta.num);
	}
	else if( data[0]==32){
		asservissmentStop();
	}
	else if( data[0]==33){
		uintConv error;
		error.num = (int16_t)getAngularError();
		I2CSetBuffer(error.tab,2);
	}
	else if( data[0]==34){
		uintConv error;
		error.num = (int16_t)getLinearError();
		I2CSetBuffer(error.tab,2);;
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
	uint32_t PrintTime =  get_uptime_ms()+500;
	uint32_t changePointTime =  get_uptime_ms()+2000;
	uint32_t changePointTime2 =  get_uptime_ms()+2000;
	uint32_t changePointTime3 =  get_uptime_ms()+7000;
	
	

	
	

	while (1) {
		odometrieLoop();
		asservissementLoop();

		//PRINT DEBUG
		if(PrintTime<get_uptime_ms()){
			PrintTime =  get_uptime_ms()+500;
			printPosition();
			printAllInformation();
		}

		//BLINK LED
		if(changePointTime < get_uptime_ms()){
			changePointTime = get_uptime_ms() + 1000;
			gpio_toggle(port_led1,pin_led1);
		}

		//TEST ASSERVISSEMENT
		// if(changePointTime2 < get_uptime_ms()){
		// 	changePointTime2 = get_uptime_ms() + 5000000;
		// 	setLinearAsservissement(1000,0,false);
		// 	//setAngularAsservissement(200);
		// }

		// if(changePointTime3 < get_uptime_ms()){
		// 	changePointTime3 = get_uptime_ms() + 5000000;
		// 	setLinearAsservissement(0,0,true);
		// 	//setAngularAsservissement(0);
		// }
		
	}

	return 0;
}
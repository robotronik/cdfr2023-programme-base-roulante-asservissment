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
#include "led.h"
#include "sequence.h"
#include "robot.h"


#define TESTROBOT
//#define TESTMOTOR

robot* robotCDFR = new robot();

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
		//position_u posi = odometrieGetPositionInt();
		//I2CSetBuffer(posi.tab,6);
	}
	else if( data[0]==21 && size == 7){
		// position_u posi;
		// memcpy(posi.tab, data+1, 6);
		// robotCDFR.setPostion(posi);
		// asservissementSetup();
	}
	else if( data[0]==30 && size == 7){
		uintConv x,y,arriere;
		x.tab[0] = data[1]; x.tab[1] = data[2];
		y.tab[0] = data[3]; y.tab[1] = data[4];
		arriere.tab[0] = data[5]; arriere.tab[1] = data[6];
		asservissementSetup();
		setLinearAsservissement((double)x.num,(double)y.num,(double)arriere.num);
	}
	else if( data[0]==31 && size == 3){
		uintConv teta;
		teta.tab[0] = data[1]; teta.tab[1] = data[2];
		asservissementSetup();
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

void testloop(sequence* seq){
	seq->start();
	seq->delay([](){
		setLinearAsservissement(1000,0,false);
	},7000);

	seq->delay([](){
		setLinearAsservissement(-1000,0,true);
		led2_clear();
	},7000);

	seq->delay([](){
		setLinearAsservissement(0,0,false);
	},7000);

	// seq->delay([](){
	// 	setAngularAsservissement(90);
	// },7000);

	// seq->delay([](){
	// 	setAngularAsservissement(-90);
	// },7000);

	// seq->delay([](){
	// 	setAngularAsservissement(0);
	// },7000);
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
	delay_ms(3000);
	usartprintf("Start\n");


	//UNBRAKE MOTOR
	motorBrakeR(0);
	motorBrakeL(0);
	motorSetModeR(0);
	motorSetModeL(0);


//
// Test motor
// Accelerate Forward -> Decelerate Forward -> Accelerate backward -> Decelerate backward 
//	
	#ifdef TESTMOTOR
		for (int i = 0; i < 100; i++){
			motorSpeedSignedL(i);
			motorSpeedSignedR(i);
			delay_ms(50);
		}
		for (int i = 100; i > -100; i--){
			motorSpeedSignedL(i);
			motorSpeedSignedR(i);
			delay_ms(50);
		}

		for (int i = -100; i < 0; i++){
			motorSpeedSignedL(i);
			motorSpeedSignedR(i);
			delay_ms(50);
		}
		while (1);		
	#endif
	
#ifdef TESTROBOT
//
// Test move on the robot
// Forwarde backwadr and rotation
//	
	sequence mySeq;
	sequence ledToggleSeq;
	while (1){
		odometrieLoop(robotCDFR);
		position_t robotPosition = robotCDFR->getPosition();
		usartprintf(">x:%lf\n>y:%lf\n>teta:%lf\n",robotPosition.x,robotPosition.y,robotPosition.teta);
		asservissementLoop(robotCDFR);
		testloop(&mySeq);


		//BLINK LED
		ledToggleSeq.interval([](){
			led1_toggle();
		},1000);
	}
#endif


#ifndef TESTROBOT
//
//	Main Loop off the robot
//	
	sequence ledToggleSeq;
	while (1) {
		//odometrieLoop();
		//asservissementLoop();

		//BLINK LED
		ledToggleSeq.interval([](){
			led1_toggle();
		},100);	
	}
	return 0;
#endif
}
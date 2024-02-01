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
#include "led.h"
#include "sequence.h"
#include "robot.h"
#include "Asservissement.h"


#define TESTROBOT
//#define TESTMOTOR

robot* robotCDFR = new robot();
Asservissement* robotAsservisement = new Asservissement(robotCDFR);

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
		position_u posi;
		posi.position.x =  robotCDFR->getPosition_X();
		posi.position.y =  robotCDFR->getPosition_Y();
		posi.position.teta =  robotCDFR->getPosition_Teta();
		I2CSetBuffer(posi.tab,6);
	}
	else if( data[0]==21 && size == 7){
		position_u posi;
		memcpy(posi.tab, data+1, 6);
		position_t positionConv;
		positionConv.x = posi.position.x;
		positionConv.y = posi.position.y;
		positionConv.teta = posi.position.teta;
		robotCDFR->setPosition(positionConv);
		robotAsservisement->setConsigne(positionConv);
	}
	else if( data[0]==30){
		robotAsservisement->setConsigneStop();
	}
	else if( data[0]==31 && size == 5){
		uintConv x,y;
		x.tab[0] = data[1]; x.tab[1] = data[2];
		y.tab[0] = data[3]; y.tab[1] = data[4];
		robotAsservisement->setConsigneLineaire((double)x.num,(double)y.num);
	}
	else if( data[0]==32 && size == 5){
		uintConv teta,rotation;
		teta.tab[0] = data[1]; teta.tab[1] = data[2];
		rotation.tab[0] = data[3]; rotation.tab[1] = data[4];
		robotAsservisement->setConsigneAngulaire((double)teta.num,(sensRotation_t)rotation.num);
	}
	else if( data[0]==33 && size == 7){
		uintConv x,y,rotation;
		x.tab[0] = data[1]; x.tab[1] = data[2];
		y.tab[0] = data[3]; y.tab[1] = data[4];
		rotation.tab[0] = data[5]; rotation.tab[1] = data[6];
		robotAsservisement->setConsigneLookAt((double)x.num,(double)y.num,(sensRotation_t)rotation.num);
	}
	else if( data[0]==40){
		uintConv boolData;
		boolData.num = robotAsservisement->robotIsMoving();
		I2CSetBuffer(boolData.tab,2);
	}
	else if( data[0]==41){
		uintConv boolData;
		boolData.num = robotAsservisement->robotIsRunning();
		I2CSetBuffer(boolData.tab,2);
	}	
	else if( data[0]==42){
		uintConv boolData;
		boolData.num = robotAsservisement->robotIsTurning();
		I2CSetBuffer(boolData.tab,2);
	}	
	else if( data[0]==43){
		uintConv boolData;
		//PAS LA BONNE FONCTION
		boolData.num = robotAsservisement->getLinearError();
		I2CSetBuffer(boolData.tab,2);
	}	
	else if( data[0]==44){
		uintConv boolData;
		//PAS LA BONNE FONCTION
		boolData.num = robotAsservisement->getAngularError();
		I2CSetBuffer(boolData.tab,2);
	}	
}

void testloop(sequence* seq){
	
	seq->start();

	seq->delay([](){
		robotAsservisement->setConsigneAngulaire(90,ROTATION_HORRAIRE);
	},7000);

	seq->delay([](){
		robotAsservisement->setConsigneAngulaire(0,ROTATION_TRIGO);
	},2000);

	seq->delay([](){
		robotAsservisement->setConsigneLineaire(1000,0);
	},10000);

	seq->delay([](){
		robotAsservisement->setConsigneStop();
	},1500);

	seq->delay([](){
		robotAsservisement->setConsigneLineaire(0,0);
	},10000);

	seq->delay([](){
		robotAsservisement->setConsigneAngulaire(90,ROTATION_TRIGO);
	},7000);

	seq->delay([](){
		robotAsservisement->setConsigneAngulaire(0,ROTATION_HORRAIRE);
	},7000);
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
		delay_ms(50);
		odometrieLoop(robotCDFR);
		position_t robotPosition = robotCDFR->getPosition();
		usartprintf(">x:%lf\n",robotPosition.x);
		usartprintf(">y:%lf\n",robotPosition.y);
		usartprintf(">teta:%lf\n",robotPosition.teta);
		usartprintf(">consigney:%lf\n",robotAsservisement->consigne.y);
		motorSpeed_t speed = robotAsservisement->asservissementLoop();
		motorSpeedSignedL(speed.L);
		motorSpeedSignedR(speed.R);

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
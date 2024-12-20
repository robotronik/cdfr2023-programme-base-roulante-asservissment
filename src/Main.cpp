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
#include "movement.h"


//#define TESTROBOT
//#define TESTMOTOR

#ifdef SIMULATION
	#include "hardware_interface.h"
	#define main stm_main
#endif

position_t newPostion;
bool needChangePos = false;

robot* robotCDFR = new robot();
movement* robotAsservisement = new movement(robotCDFR);

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
		newPostion.x = (double)((int16_t) (data[1]<<8 | data[2]));
		newPostion.y = (double)((int16_t) (data[3]<<8 | data[4]));
		newPostion.teta = (double)((int16_t) (data[5]<<8 | data[6]));
		needChangePos = true;
	}
	else if( data[0]==30){
		robotAsservisement->setConsigneStop();
	}
	else if( data[0]==31 && size == 5){
		uintConv x,y;
		x.tab[1] = data[1]; x.tab[0] = data[2];
		y.tab[1] = data[3]; y.tab[0] = data[4];
        robotAsservisement->goToPoint((double)x.num,(double)y.num);
	}
	else if( data[0]==32 && size == 5){
		uintConv teta,rotation;
		teta.tab[1] = data[1]; teta.tab[0] = data[2];
		rotation.tab[1] = data[3]; rotation.tab[0] = data[4];
		robotAsservisement->setConsigneAngulaire((double)teta.num,(Rotation)rotation.num);
	}
	else if( data[0]==33 && size == 7){
		uintConv x,y,rotation;
		x.tab[1] = data[1]; x.tab[0] = data[2];
		y.tab[1] = data[3]; y.tab[0] = data[4];
		rotation.tab[0] = data[5]; rotation.tab[1] = data[6];
		robotAsservisement->setConsigneLookAtForward((double)x.num,(double)y.num,(Rotation)rotation.num);
	}
	else if( data[0]==34 && size == 7){
		uintConv x,y,rotation;
		x.tab[1] = data[1]; x.tab[0] = data[2];
		y.tab[1] = data[3]; y.tab[0] = data[4];
		rotation.tab[0] = data[5]; rotation.tab[1] = data[6];
		robotAsservisement->setConsigneLookAtBackward((double)x.num,(double)y.num,(Rotation)rotation.num);
	}
	else if( data[0]==40){
		uintConv boolData;
		boolData.num = robotAsservisement->robotMovingIsFinish();
		I2CSetBuffer(boolData.tab,2);
	}
	else if( data[0]==41){
		uintConv boolData;
		boolData.num = robotAsservisement->robotRunningIsFinish();
		I2CSetBuffer(boolData.tab,2);
	}
	else if( data[0]==42){
		uintConv boolData;
		boolData.num = robotAsservisement->robotTurningIsFinish();
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
	else if( data[0]==45){
		uintConv dataConv;
		dataConv.num = robotAsservisement->getBrakingDistance();
		I2CSetBuffer(dataConv.tab,2);
	}
    else if( data[0]==46){
		uintConv dataConv;
		dataConv.num = robotAsservisement->commandRun();
		I2CSetBuffer(dataConv.tab,2);
	}
    else if( data[0]==47){
		uintConv dataConv;
		dataConv.num = (int16_t)robotAsservisement->getDirectionSide();
		I2CSetBuffer(dataConv.tab,2);
	}
    else if( data[0]==48){
		uintConv dataConv;
		dataConv.num = (int16_t)robotAsservisement->getRotationSide();
		I2CSetBuffer(dataConv.tab,2);
	}
    else if( data[0]==49){
        position_t consigne = robotAsservisement->getCurrentConsigne();
		position_u posi;
		posi.position.x = (int16_t)consigne.x;
		posi.position.y = (int16_t)consigne.y;
		posi.position.teta = (int16_t)consigne.teta;
		I2CSetBuffer(posi.tab,6);
	}
	else if( data[0]==50){
		disableMotor();
	}
	else if( data[0]==51){
		robotAsservisement->reset();
		motorSpeedSignedL(0);
		motorSpeedSignedR(0);
		enableMotor();
	}
	else if( data[0]==52){
		motorBrakeL(true);
		motorBrakeR(true);
	}
	else if( data[0]==53){
		motorBrakeL(false);
		motorBrakeR(false);
	}
	else if( data[0]==54){
		uintConv max;
		max.tab[1] = data[1]; max.tab[0] = data[2];
		setMaxTorqueL(max.num);
		setMaxTorqueR(max.num);
	}
	else if( data[0]==60){
		uintConv x;
		x.tab[1] = data[1];
		x.tab[0] = data[2];
		robotAsservisement->positionControlLineaire.vitesseMaxAv = x.num;
	}
	else if( data[0]==61){
		uintConv x;
		x.tab[1] = data[1];
		x.tab[0] = data[2];
		robotAsservisement->positionControlLineaire.vitesseMaxAr = x.num;
	}
	else if( data[0]==62){
		uintConv x;
		x.tab[1] = data[1];
		x.tab[0] = data[2];
		robotAsservisement->positionControlAngulaire.vitesseMaxAv = x.num;
	}
	else if( data[0]==63){
		uintConv x;
		x.tab[1] = data[1];
		x.tab[0] = data[2];
		robotAsservisement->positionControlAngulaire.vitesseMaxAr = x.num;
	}
}

void testloop(sequence* seq){

	seq->start();

	seq->delay([](){
		robotAsservisement->setConsigneAngulaire(90,Rotation::SHORTEST);
	},0);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneAngulaire(-90,ROTATION_TRIGO);
	// },3000);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneAngulaire(90,ROTATION_HORRAIRE);
	// },3000);

	seq->delay([](){
	robotAsservisement->setConsigneAngulaire(0,Rotation::SHORTEST);
	},3000);

	seq->delay([](){
		robotAsservisement->goToPoint(1000,1000);
	},3000);

	seq->delay([](){
		robotAsservisement->goToPoint(0,0,0);
	},7000);

	seq->delay([](){
		robotAsservisement->goToPoint(1000,1000,Rotation::ANTICLOCKWISE,Direction::BACKWARD);
	},7000);

	seq->delay([](){
		robotAsservisement->goToPoint(0,0,0,Rotation::CLOCKWISE,Direction::BACKWARD,Rotation::ANTICLOCKWISE);
	},7000);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneStop();
	// },1500);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneLineaire(0,0);
	// },10000);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneAngulaire(90,ROTATION_TRIGO);
	// },7000);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneAngulaire(0,ROTATION_HORRAIRE);
	// },7000);
}

int main(void)
{

	//SETUP
	clock_setup();
	ledSetup();
	usartSetup();
	motorSetup();
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
	disableMotor();


//
// Test motor
// Accelerate Forward -> Decelerate Forward -> Accelerate backward -> Decelerate backward
//
	#ifdef TESTMOTOR
		enableMotor();
		for (int i = 0; i < 100; i++){
			usartprintf("%d\n",i);
			motorSpeedSignedL(i);
			motorSpeedSignedR(i);
			delay_ms(100);
			usartprintf("Right : %d %d\n",gpio_get(port_info1R,port_info1R),gpio_get(port_info2R,port_info2R));
			usartprintf("Left : %d %d\n\n",gpio_get(port_info1R,port_info1L),gpio_get(port_info2R,port_info2L));
		}
		for (int i = 100; i > -100; i--){
			usartprintf("%d\n",i);
			motorSpeedSignedL(i);
			motorSpeedSignedR(i);
			delay_ms(100);
			usartprintf("Right : %d %d\n",gpio_get(port_info1R,port_info1R),gpio_get(port_info2R,port_info2R));
			usartprintf("Left : %d %d\n\n",gpio_get(port_info1R,port_info1L),gpio_get(port_info2R,port_info2L));
		}

		for (int i = -100; i < 0; i++){
			usartprintf("%d\n",i);
			motorSpeedSignedL(i);
			motorSpeedSignedR(i);
			delay_ms(100);
			usartprintf("Right : %d %d\n",gpio_get(port_info1R,port_info1R),gpio_get(port_info2R,port_info2R));
			usartprintf("Left : %d %d\n\n",gpio_get(port_info1R,port_info1L),gpio_get(port_info2R,port_info2L));
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
	robotAsservisement->reset();
	enableMotor();
	while (1){
		//delay_ms(50);
		odometrieLoop(robotCDFR);
        robotAsservisement->loop();
		position_t robotPosition = robotCDFR->getPosition();
        // usartprintf("%s\n",robotAsservisement->getDirectionSide());
		// prinAdcValue();
		// usartprintf(">x:%lf\n",robotPosition.x);
		// usartprintf(">y:%lf\n",robotPosition.y);
		// usartprintf(">teta:%lf\n",robotPosition.teta);
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
	robotAsservisement->reset();
	uint32_t nextTime =  get_uptime_ms();

	while (1){

		odometrieLoop(robotCDFR);
        robotAsservisement->loop();
		if(needChangePos){
			needChangePos = false;
			robotCDFR->setPosition(newPostion);
			robotAsservisement->setConsigne(newPostion);
		}
		if(nextTime < get_uptime_ms()){
			//prinAdcValue();
			nextTime = get_uptime_ms() + 50;
			motorSpeed_t speed = robotAsservisement->asservissementLoop();
			// usartprintf(">x : %.3lf\n>y : %.3lf\n>teta : %.3lf\n",robotCDFR->getPosition_X(),robotCDFR->getPosition_Y(),robotCDFR->getPosition_Teta());
			// usartprintf("%d %d\n",speed.L,speed.R);
			motorSpeedSignedL(speed.L);
			motorSpeedSignedR(speed.R);
		}

		//BLINK LED
		ledToggleSeq.interval([](){
			led1_toggle();
		},1000);
	}
#endif
	return 0;
}
#include "config.h"
#include "motor.h"
#include "uart.h"
#include "odometrie.h"
#include "clock.h"
#include "I2C.h"
#include "led.h"
#include "sequence.h"
#include "position.h"
#include "Asservissement.h"
#include "movement.h"
#include "i2c_interface.h"


//#define TESTROBOT
//#define TESTMOTOR

#ifdef SIMULATION
	#include "hardware_interface.h"
	#define main stm_main
#endif

position* robotPosition = new position();
movement* robotAsservisement = new movement();
i2c_interface* robotI2cInterface = new i2c_interface(robotPosition, robotAsservisement);

void I2CRecieveData(uint8_t* data, int size){
    robotI2cInterface->I2CDataSwitch(data,size);
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

		robotPosition->loop();
        robotAsservisement->loop();
		if(nextTime < get_uptime_ms()){
			//prinAdcValue();
			nextTime = get_uptime_ms() + 50;
			motorSpeed_t speed = robotAsservisement->asservissementLoop(*robotPosition);
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
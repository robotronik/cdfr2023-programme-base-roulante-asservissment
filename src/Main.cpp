#include "config.h"
#include "Motor.h"
#include "Wheel.h"
#include "uart.h"
#include "clock.h"
#include "I2C.h"
#include "led.h"
#include "types/sequence.h"
#include "position.h"
#include "Asservissement.h"
#include "movement.h"
#include "i2c_interface.h"
#include "button.h"
#include "odometry/I2Cdevice.h"
#include "odometry/OTOS.h"

// If debug is define, the code will be compiled with debug information
#define DEBUG

#ifdef SIMULATION
	#include "hardware_interface.h"
	#define main stm_main
#endif

// This is needed for the linker to find the __dso_handle symbol

movement* robotAsserv = nullptr;
i2c_interface* robotI2cInterface = nullptr;

void I2CRecieveData(uint8_t* data, int size) {
    robotI2cInterface->I2CDataSwitch(data, size);
}

void testMotors();
void testloop(sequence* seq);

int main(void)
{
	//SETUP
	clock_setup();

	
	robotAsserv = new movement();
	robotI2cInterface = new i2c_interface(robotAsserv);

	ledSetup();
					GreenLED_Set();
    buttonSetup();
	usartSetup();
	DriveSetup();
	i2c_setup();
	setCallbackReceive(I2CRecieveData);
	setupDeviceI2C();


	//WAIT
	delay_ms(1000);
	usartprintf("Start\n");


	i2cDevice = new I2CDevice(kDefaultAddress);
	otos = new OTOS(i2cDevice);

	// Check the connection with the OTOS
	while (otos->begin() != ret_OK) {
		usartprintf("OTOS not connected\n");
		RedLED_Toggle();
		delay_ms(200);
	}
	usartprintf("OTOS connected\n");
	RedLED_Clear();

	wheelA =  new Wheel(DISTANCE_WHEEL,180, DIAMETER_WHEEL, motorA);
	wheelA =  new Wheel(DISTANCE_WHEEL, 60, DIAMETER_WHEEL, motorA);
	wheelA =  new Wheel(DISTANCE_WHEEL,-60, DIAMETER_WHEEL, motorA);
	



//
//	Main Loop of the robot
//
	sequence ledToggleSeq;
    sequence mySeq;
    sequence dbg;
    bool isDebug = false;

    // Reset because the stm has been booted for 3 seconds
	robotAsserv->reset();

	while (1) {
		updatePositionData();
        robotAsserv->loop();

        if (isDebug) {
            testloop(&mySeq);
        }
        else if (readTestButton()) {
            isDebug = true;
            mySeq.reset();
        }

		if (readPushButton())
			testMotors();

		// Write the position to debug console
        dbg.interval([](){
			usartprintf("x : %5lf mm, y : %5lf mm, a : %5lf degs\n", pos.x, pos.y, pos.a);;
		},100);

		//BLINK LED
		ledToggleSeq.interval([](){
			GreenLED_Toggle();
		},800);
	}

	return 0;
}

//
// Test motor
// Accelerate Forward -> Decelerate Forward -> Accelerate backward -> Decelerate backward
//
void testMotors(){
    DriveEnable();
    for (double i = 0.0; i < 100.0; i++) {
        usartprintf("%d\n",i);
		motorA->SetSpeedSigned(i);
		motorB->SetSpeedSigned(i);
		motorC->SetSpeedSigned(i);
		motorA->PrintValues();
		motorB->PrintValues();
		motorC->PrintValues();
        delay_ms(100);
    }
    for (double i = 100.0; i > -100.0; i--) {
        usartprintf("%d\n",i);
		motorA->SetSpeedSigned(i);
		motorB->SetSpeedSigned(i);
		motorC->SetSpeedSigned(i);
		motorA->PrintValues();
		motorB->PrintValues();
		motorC->PrintValues();
        delay_ms(100);
    }
    for (double i = -100.0; i <= 0.0; i++) {
        usartprintf("%d\n",i);
		motorA->SetSpeedSigned(i);
		motorB->SetSpeedSigned(i);
		motorC->SetSpeedSigned(i);
		motorA->PrintValues();
		motorB->PrintValues();
		motorC->PrintValues();
        delay_ms(100);
    }
    while (1);
}

void testloop(sequence* seq) {

	seq->start();

	seq->delay([](){
        robotI2cInterface->set_coordinates(0,0,0);
        robotI2cInterface->set_motor_state(true);
		robotI2cInterface->consigne_angulaire(90,Rotation::SHORTEST);
	},0);

	// seq->delay([](){
	// 	robotAsserv->setTargetAngulaire(-90,ROTATION_TRIGO);
	// },3000);

	// seq->delay([](){
	// 	robotAsserv->setTargetAngulaire(90,ROTATION_HORRAIRE);
	// },3000);

	seq->delay([](){
	robotI2cInterface->consigne_angulaire(0,Rotation::SHORTEST);
	},3000);

	seq->delay([](){
		robotI2cInterface->go_to_point(1000,1000);
	},3000);

    seq->delay([](){
		robotI2cInterface->stop();
	},2000);

	seq->delay([](){
		robotI2cInterface->go_to_point(0,0,0);
	},7000);

	seq->delay([](){
		robotI2cInterface->go_to_point(1000,1000,Rotation::ANTICLOCKWISE,Direction::BACKWARD);
	},7000);

	seq->delay([](){
		robotI2cInterface->go_to_point(0,0,0,Rotation::CLOCKWISE,Direction::BACKWARD,Rotation::ANTICLOCKWISE);
	},7000);

	// seq->delay([](){
	// 	robotAsserv->setTargetStop();
	// },1500);

	// seq->delay([](){
	// 	robotAsserv->setTargetLineaire(0,0);
	// },10000);

	// seq->delay([](){
	// 	robotAsserv->setTargetAngulaire(90,ROTATION_TRIGO);
	// },7000);

	// seq->delay([](){
	// 	robotAsserv->setTargetAngulaire(0,ROTATION_HORRAIRE);
	// },7000);
}
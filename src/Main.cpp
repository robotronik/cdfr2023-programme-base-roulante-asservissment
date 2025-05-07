#include "config.h"
#include "Motor.h"
#include "uart.h"
#include "clock.h"
#include "I2C.h"
#include "led.h"
#include "sequence.h"
#include "position.h"
#include "Asservissement.h"
#include "movement.h"
#include "i2c_interface.h"
#include "button.h"
#include "odometry/I2Cdevice.h"
#include "odometry/OTOS.h"

// #define TESTMOTOR

#ifdef SIMULATION
	#include "hardware_interface.h"
	#define main stm_main
#endif

// This is needed for the linker to find the __dso_handle symbol
void *__dso_handle = 0;

movement robotAsserv;
i2c_interface robotI2cInterface(robotAsserv);

void I2CRecieveData(uint8_t* data, int size) {
    robotI2cInterface.I2CDataSwitch(data, size);
}

void testloop(sequence* seq) {

	seq->start();

	seq->delay([](){
        robotI2cInterface.set_coordinates(0,0,0);
        robotI2cInterface.set_motor_state(true);
		robotI2cInterface.consigne_angulaire(90,Rotation::SHORTEST);
	},0);

	// seq->delay([](){
	// 	robotAsserv.setConsigneAngulaire(-90,ROTATION_TRIGO);
	// },3000);

	// seq->delay([](){
	// 	robotAsserv.setConsigneAngulaire(90,ROTATION_HORRAIRE);
	// },3000);

	seq->delay([](){
	robotI2cInterface.consigne_angulaire(0,Rotation::SHORTEST);
	},3000);

	seq->delay([](){
		robotI2cInterface.go_to_point(1000,1000);
	},3000);

    seq->delay([](){
		robotI2cInterface.stop();
	},2000);

	seq->delay([](){
		robotI2cInterface.go_to_point(0,0,0);
	},7000);

	seq->delay([](){
		robotI2cInterface.go_to_point(1000,1000,Rotation::ANTICLOCKWISE,Direction::BACKWARD);
	},7000);

	seq->delay([](){
		robotI2cInterface.go_to_point(0,0,0,Rotation::CLOCKWISE,Direction::BACKWARD,Rotation::ANTICLOCKWISE);
	},7000);

	// seq->delay([](){
	// 	robotAsserv.setConsigneStop();
	// },1500);

	// seq->delay([](){
	// 	robotAsserv.setConsigneLineaire(0,0);
	// },10000);

	// seq->delay([](){
	// 	robotAsserv.setConsigneAngulaire(90,ROTATION_TRIGO);
	// },7000);

	// seq->delay([](){
	// 	robotAsserv.setConsigneAngulaire(0,ROTATION_HORRAIRE);
	// },7000);
}


int main(void)
{
	//SETUP
	clock_setup();
	ledSetup();
    buttonSetup();
		RedLED_Set();
	usartSetup();
	DriveSetup();
	i2c_setup();
	setupDeviceI2C();
	setCallbackReceive(I2CRecieveData);


	//WAIT
	delay_ms(1000);
	usartprintf("Start\n");

	// Check the connection with the OTOS
	while (otos.begin() != ret_OK) {
		usartprintf("OTOS not connected\n");
		RedLED_Toggle();
		delay_ms(200);
	}
	usartprintf("OTOS connected\n");
	RedLED_Clear();
	

//
// Test motor
// Accelerate Forward -> Decelerate Forward -> Accelerate backward -> Decelerate backward
//
#ifdef TESTMOTOR
    enableMotor();
    for (int i = 0; i < 100; i++) {
        usartprintf("%d\n",i);
        motorSpeedSignedL(i);
        motorSpeedSignedR(i);
        delay_ms(100);
        usartprintf("Right : %d %d\n",gpio_get(port_info1R,pin_info1R),gpio_get(port_info2R,pin_info2R));
        usartprintf("Left : %d %d\n\n",gpio_get(port_info1L,pin_info1L),gpio_get(port_info2L,pin_info2L));
    }
    for (int i = 100; i > -100; i--) {
        usartprintf("%d\n",i);
        motorSpeedSignedL(i);
        motorSpeedSignedR(i);
        delay_ms(100);
        usartprintf("Right : %d %d\n",gpio_get(port_info1R,pin_info1R),gpio_get(port_info2R,pin_info2R));
        usartprintf("Left : %d %d\n\n",gpio_get(port_info1L,pin_info1L),gpio_get(port_info2L,pin_info2L));
    }

    for (int i = -100; i < 0; i++) {
        usartprintf("%d\n",i);
        motorSpeedSignedL(i);
        motorSpeedSignedR(i);
        delay_ms(100);
        usartprintf("Right : %d %d\n",gpio_get(port_info1R,port_info1R),gpio_get(port_info2R,port_info2R));
        usartprintf("Left : %d %d\n\n",gpio_get(port_info1R,port_info1L),gpio_get(port_info2R,port_info2L));
    }
    while (1);
#endif



//
//	Main Loop of the robot
//
	sequence ledToggleSeq;
    sequence mySeq;
    sequence dbg;
    bool enableDebug = false;

    // Reset because the stm has been booted for 3 seconds
	robotAsserv.reset();

	while (1) {
		updatePositionData();
        robotAsserv.loop();

        if (readTestButton() && !enableDebug) {
            enableDebug = true;
            mySeq.reset();
        }
        else if (enableDebug) {
            testloop(&mySeq);
        }

        dbg.interval([](){
			usartprintf("x : %5lf, y : %5lf, theta : %5lf\n", pos.x, pos.y, pos.a);;
		},100);

		//BLINK LED
		ledToggleSeq.interval([](){
			GreenLED_Toggle();
		},800);
	}

	return 0;
}
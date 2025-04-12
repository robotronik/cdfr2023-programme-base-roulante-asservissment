#include "config.h"
#include "motor.h"
#include "uart.h"
#include "clock.h"
#include "led.h"
#include "sequence.h"
#include "Asservissement.h"
#include "i2c_interface.h"
#include "button.h"
#include "calibration.h"
#include "test.h"

//#define TESTMOTOR

#ifdef SIMULATION
	#include "hardware_interface.h"
	#define main stm_main
#endif

position* robotPosition = new position();
movement* robotAsservisement = new movement(robotPosition);
i2c_interface* robotI2cInterface = new i2c_interface(robotPosition, robotAsservisement);

void I2CRecieveData(uint8_t* data, int size){
    robotI2cInterface->I2CDataSwitch(data,size);
}


int main(void)
{

	//SETUP
	clock_setup();
	ledSetup();
    buttonSetup();
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
    while(1);
#endif



//
//	Main Loop off the robot
//
	sequence ledToggleSeq;
    sequence mySeq;
    sequence dbg;
    bool enableDebug = false;
    bool enableCalibration = false;

    //reset because the stm has been booted for 3 seconds
	robotAsservisement->reset();

	while (1){
		robotPosition->loop();
        robotAsservisement->loop();

        if(readButton2() && !enableDebug){
            enableDebug = true;
            mySeq.reset();
        }
        else if(enableDebug){
            testloop(robotI2cInterface);
        }

        if(readButton1() && !enableCalibration){
            enableCalibration = true;
        }
        else if(enableCalibration){
            loopCalibration(robotI2cInterface);
        }

        // dbg.interval([](){
		// 	usartprintf("x : %5lf, y : %5lf, theta : %5lf\n",robotPosition->getPosition_X(),robotPosition->getPosition_Y(),robotPosition->getPosition_Teta());;
		// },1000);

		//BLINK LED
		ledToggleSeq.interval([](){
			led1_toggle();
		},1000);
	}

	return 0;
}
#pragma once

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>
#include "pinConfig.h"


void motorSetup(void);

//set to 0 to go forward. Other to backward
void motorDirectionL(int direction);
void motorDirectionR(int direction);

//set the speed of the motor with a value between -100 and 100
//value 0 are stop motor
//negative value are use to backward
//Don't forget to disable reset, coast and brake
void motorSpeedSignedL(int speed);
void motorSpeedSignedR(int speed);

//set the speed of the motor with a value between 0 and 100
//value 0 are stop motor
//Don't forget to disable reset, coast and brake
//direction = -1 by default. Set to 0 to go forward, 1 to go backward.
void motorSpeedUnsignedL(int speed,int direction = -1);
void motorSpeedUnsignedR(int speed,int direction = -1);

//Brake enable if the value is different of 0
//if 0, the wheel is free
void motorBrakeL(int brake);
void motorBrakeR(int brake);



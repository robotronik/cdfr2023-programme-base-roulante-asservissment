#pragma once

#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/stm32/rcc.h>
	#include <libopencm3/stm32/gpio.h>
	#include <libopencm3/cm3/nvic.h>
	#include <libopencm3/stm32/timer.h>
#endif
#include "type.h"

//#define ENABLE_STATISTIC


#define pin_ResetDrive 			GPIO15
#define port_ResetDrive				GPIOA
#define pin_CoastDrive 			GPIO12
#define port_CoastDrive				GPIOB
#define pin_ModeDrive 			GPIO11
#define port_ModeDrive 				GPIOA

// ESF : Enable Stop on Fault
//PIN and Port to control MotorA
#define pin_SpeedControlA	GPIO0
#define port_SpeedControlA		GPIOB
#define pin_BrakeA 			GPIO6
#define port_BrakeA 			GPIOA
#define pin_DirectionA 		GPIO4
#define port_DirectionA 		GPIOA
#define pin_ESFA 			GPIO10
#define port_ESFA   			GPIOB
#define pin_Err1A			GPIO4
#define port_Err1A				GPIOC
#define pin_Err2A			GPIO7
#define port_Err2A				GPIOA
#define pin_TachoA			GPIO5
#define port_TachoA				GPIOA
#define pin_InfoDirA		GPIO2
#define port_InfoDirA			GPIOB
#define adc_channelA		1


//PIN and Port to control MotorB
#define pin_SpeedControlB	GPIO13
#define port_SpeedControlB		GPIOB
#define pin_BrakeB 			GPIO15
#define port_BrakeB 			GPIOB
#define pin_DirectionB 		GPIO14
#define port_DirectionB 		GPIOB
#define pin_ESFB 			GPIO10
#define port_ESFB   			GPIOA
#define pin_Err1B			GPIO8
#define port_Err1B				GPIOC
#define pin_Err2B			GPIO7
#define port_Err2B				GPIOC
#define pin_TachoB			GPIO6
#define port_TachoB				GPIOC
#define pin_InfoDirB		GPIO9
#define port_InfoDirB			GPIOA
#define adc_channelB		11


//PIN and Port to control MotorC
#define pin_SpeedControlC	GPIO1
#define port_SpeedControlC		GPIOB
#define pin_BrakeC 			GPIO12
#define port_BrakeC 			GPIOB
#define pin_DirectionC 		GPIO11
#define port_DirectionC 		GPIOC
#define pin_ESFC 			GPIO4
#define port_ESFC   			GPIOB
#define pin_Err1C			GPIO3
#define port_Err1C				GPIOB
#define pin_Err2C			GPIO2
#define port_Err2C				GPIOD
#define pin_TachoC			GPIO0
#define port_TachoC				GPIOA
#define pin_InfoDirC		GPIO5
#define port_InfoDirC			GPIOB
#define adc_channelC		10


//PIN and Port to control the to led
#define pin_RedLED			GPIO8
#define port_RedLED				GPIOB
#define pin_GreenLED		GPIO9
#define port_GreenLED			GPIOB

// PIN and Port for the buttons
#define pin_TestButton		GPIO14
#define port_TestButton		    GPIOC
#define pin_PushButton		GPIO15
#define port_PushButton		    GPIOC


//PIN and PORT for the TX and RX pin. Use for debug
#define pin_TX              GPIO2
#define port_TX                 GPIOA
#define pin_RX              GPIO3
#define port_RX                 GPIOA

//PIN and PORT for programming
#define pin_SWDIO           GPIO13
#define port_SWDIO              GPIOA
#define pin_SWCLK           GPIO14
#define port_SWCLK              GPIOA

//PIN and PORT for the odometry sensor in I2C (I2C3)
#define pin_OdoClk			GPIO8
#define port_OdoClk				GPIOA
#define pin_OdoSda			GPIO9
#define port_OdoSda				GPIOC

//PIN and PORT for I2C   (I2C1)
#define pin_I2CClk			GPIO6
#define port_I2CClk				GPIOB
#define pin_I2CSda			GPIO7
#define port_I2CSda				GPIOB



#define DISTANCE_WHEEL 100.0
#define DIAMETER_WHEEL 60.0

#define PI 3.14159265359
#define DEG_TO_RAD PI/180
#define RAD_TO_DEG 180/PI

enum class BaseCommand {
    ANGULAR_THETA,
    ANGULAR_LOOKAT,
    LINEAR,
    MAX_SPEED_LINEAR,
    MAX_SPEED_ANGULAR
};

const char* directionToChar(Direction dir);
const char* rotationToChar(Rotation rot);
const char* baseCommandToString(BaseCommand cmd);

typedef struct{
	double x = 0;
    double y = 0;
	double teta = 0;
	uint64_t time = 0;
}position_t;

typedef struct{
	int L = 0;
    int R = 0;
}motorSpeed_t;

typedef struct{
	int16_t x;
    int16_t y;
	int16_t teta;
}positionint_t;

union position_u
{
	positionint_t position;
	uint8_t tab[6];
};

union uintConv
{
	int16_t num;
	uint8_t tab[2];
};



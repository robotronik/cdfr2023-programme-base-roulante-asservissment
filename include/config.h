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

// ESF : Enable Stop on Fault
//PIN and Port to control MotorA
// TODO check the pin and port
#define pin_ModeA 			GPIO0
#define port_ModeA 				GPIOA
#define pin_SpeedControlA	GPIO0
#define port_SpeedControlA		GPIOB
#define pin_brakeA 			GPIO6
#define port_brakeA 			GPIOA
#define pin_directionA 		GPIO4
#define port_directionA 		GPIOA
#define pin_resetA 			GPIO5
#define port_resetA				GPIOC
#define pin_CoastA 			GPIO1
#define port_CoastA				GPIOB
#define pin_esfA 			GPIO10
#define port_esfA   			GPIOB
#define pin_info1A			GPIO4
#define port_info1A				GPIOC
#define pin_info2A			GPIO7
#define port_info2A				GPIOA
#define pin_infoSpeedA		GPIO1
#define port_infoSpeedA			GPIOA
#define adc_channelA		1


//PIN and Port to control MotorB
// TODO check the pin and port
#define pin_ModeB 			GPIO12
#define port_ModeB 				GPIOB
#define pin_SpeedControlB	GPIO13
#define port_SpeedControlB		GPIOB
#define pin_brakeB 			GPIO15
#define port_brakeB 			GPIOB
#define pin_directionB 		GPIO14
#define port_directionB 		GPIOB
#define pin_resetB 			GPIO9
#define port_resetB				GPIOC
#define pin_CoastB 			GPIO8
#define port_CoastB				GPIOA
#define pin_esfB 			GPIO10
#define port_esfB   			GPIOA
#define pin_info1B			GPIO8
#define port_info1B				GPIOC
#define pin_info2B			GPIO7
#define port_info2B				GPIOC
#define pin_infoSpeedB		GPIO3
#define port_infoSpeedB			GPIOC
#define adc_channelB		13


//PIN and Port to control MotorC
// TODO check the pin and port
#define pin_ModeC 			GPIO12
#define port_ModeC 				GPIOB
#define pin_SpeedControlC	GPIO13
#define port_SpeedControlC		GPIOB
#define pin_brakeC 			GPIO15
#define port_brakeC 			GPIOB
#define pin_directionC 		GPIO14
#define port_directionC 		GPIOB
#define pin_resetC 			GPIO9
#define port_resetC				GPIOC
#define pin_CoastC 			GPIO8
#define port_CoastC				GPIOA
#define pin_esfC 			GPIO10
#define port_esfC   			GPIOA
#define pin_info1C			GPIO8
#define port_info1C				GPIOC
#define pin_info2C			GPIO7
#define port_info2C				GPIOC
#define pin_infoSpeedC		GPIO3
#define port_infoSpeedC			GPIOC
#define adc_channelC		11


//PIN and Port to control the to led
// TODO
#define pin_led1			GPIO12
#define port_led1				GPIOA
#define pin_led2			GPIO15
#define port_led2				GPIOA

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
#define port_OdoSda				GPIOC  // TODO Check these

//PIN and PORT for I2C   (I2C1)
#define pin_I2CClk			GPIO6
#define port_I2CClk				GPIOB
#define pin_I2CSda			GPIO7
#define port_I2CSda				GPIOB



#define DISTANCE_WHEEL 30.0 // TOOD check
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



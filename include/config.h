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
//#define OLDROBOT
#define ENABLE_STM32_LOG_I2C

//PIN and Port to control the first MotorL
#define pin_ModeL 			GPIO0
#define port_ModeL 				GPIOA
#define pin_SpeedControlL	GPIO0
#define port_SpeedControlL		GPIOB
#define pin_brakeL 			GPIO6
#define port_brakeL 			GPIOA
#define pin_directionL 		GPIO4
#define port_directionL 		GPIOA
#define pin_resetL 			GPIO5
#define port_resetL				GPIOC
#define pin_CoastL 			GPIO1
#define port_CoastL				GPIOB
#define pin_esfL 			GPIO10
#define port_esfL   			GPIOB
#define pin_info1L			GPIO4
#define port_info1L				GPIOC
#define pin_info2L			GPIO7
#define port_info2L				GPIOA
#define pin_infoSpeedL		GPIO1
#define port_infoSpeedL			GPIOA
#define adc_channelL		1


//PIN and Port to control the second MotorR
#define pin_ModeR 			GPIO12
#define port_ModeR 				GPIOB
#define pin_SpeedControlR	GPIO13
#define port_SpeedControlR		GPIOB
#define pin_brakeR 			GPIO15
#define port_brakeR 			GPIOB
#define pin_directionR 		GPIO14
#define port_directionR 		GPIOB
#define pin_resetR 			GPIO9
#define port_resetR				GPIOC
#define pin_CoastR 			GPIO8
#define port_CoastR				GPIOA
#define pin_esfR 			GPIO10
#define port_esfR   			GPIOA
#define pin_info1R			GPIO8
#define port_info1R				GPIOC
#define pin_info2R			GPIO7
#define port_info2R				GPIOC
#define pin_infoSpeedR		GPIO3
#define port_infoSpeedR			GPIOC
#define adc_channelR		13


//PIN and Port to control the to led
#define pin_led1			GPIO12
#define port_led1				GPIOA
#define pin_led2			GPIO15
#define port_led2				GPIOA


//PIN and PORT for the odometrie. Each pin are on different vector interrupt.
#define pin_odometrie1R     GPIO2
#define port_odometrie1R         GPIOD
#define pin_odometrie2R     GPIO3
#define port_odometrie2R         GPIOB
#define pin_odometrie1L     GPIO4
#define port_odometrie1L         GPIOB
#define pin_odometrie2L     GPIO5
#define port_odometrie2L         GPIOB

//PIN and PORT free
#define pin_free0           GPIO9
#define port_free0              GPIOB
#define pin_free1           GPIO13
#define port_free1              GPIOC
#define pin_free2           GPIO14
#define port_free2              GPIOC
#define pin_free3           GPIO15
#define port_free3              GPIOC
#define pin_free4           GPIO0
#define port_free4              GPIOC
#define pin_free5           GPIO1
#define port_free5              GPIOC
#define pin_free6           GPIO2
#define port_free6              GPIOC
#define pin_free7           GPIO3
#define port_free7              GPIOC


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

//PIN and PORT for the endstop. Use the end stop to calibrate the odometrie.
#define pin_endStopL         GPIO11
#define port_endstopL            GPIOC
#define pin_endStopR         GPIO12
#define port_endStopR            GPIOC

//PIN and PORT for I2C
#define pin_I2CClk			GPIO6
#define port_I2CClk				GPIOB
#define pin_I2CSda			GPIO7
#define port_I2CSda				GPIOB


#define CALIBRATION_IT 10
#define CALIBRATION_DIV 4

//Odometrie
#ifdef OLDROBOT
    #define NUMBERSTEPBYROTATION 360
    #define DISTANCEWHEEL 300 // + = teat -
    #define DIAMETERWHEELD 41.12                // +1 roue = +25 distance
    #define DIAMETERWHEELG 41.410 // + = y -     // +1 roue = +25 distance

    #define CALIBRATION_START_DISTANCEWHEEL 300
    #define CALIBRATION_START_DIAMETERWHEEL 40.0
    #define CALIBRATION_PLAGE_DISTANCEWHEEL 100.0
    #define CALIBRATION_PLAGE_DIAMETERWHEEL 20.0
#else
    #define NUMBERSTEPBYROTATION 360
    #define DISTANCEWHEEL 258.647754 // + = teat -
    #define DIAMETERWHEELD 41.455078              // +1 roue = +25 distance
    #define DIAMETERWHEELG 41.220703// + = y -     // +1 roue = +25 distance

    #define CALIBRATION_START_DISTANCEWHEEL 250.0
    #define CALIBRATION_START_DIAMETERWHEEL 40.0
    #define CALIBRATION_PLAGE_DISTANCEWHEEL 100.0
    #define CALIBRATION_PLAGE_DIAMETERWHEEL 20.0
#endif

#define PI 3.14159265359
#define DEG_TO_RAD PI/180
#define RAD_TO_DEG 180/PI

enum class BaseCommand {
    ANGULAR_THETA,
    ANGULAR_LOOKAT,
    LINEAR,
    MAX_SPEED_LINEAR,
    MAX_SPEED_ANGULAR,
    SET_POSITION
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



#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define EXIT_TASKNOTFINISH 128


#define LOGBUFFERSIZE 64
#define __I2CBUFFERSIZE__ 256

#ifndef ENABLE_STM32_LOG_I2C
    #define I2CBUFFERSIZE __I2CBUFFERSIZE__
#else
    #define I2CBUFFERSIZE (((__I2CBUFFERSIZE__) < (LOGBUFFERSIZE+1)) ? (LOGBUFFERSIZE+1) : (__I2CBUFFERSIZE__))
#endif

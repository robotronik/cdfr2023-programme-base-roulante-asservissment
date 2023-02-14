#pragma once

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <math.h>

#include "config.h"
#include "uart.h"

#define _BUFFERSIZE 512


#define STEPANGLED 0.10200558
#define STEPANGLEG 0.1013469  
#define STEPAVANCE 0.18135

#define COEFCONVDEGRETORADIAN 3.1415926535/180

typedef enum{fordwardL=0,backwardL,fordwardR,backwardR}odometrieTrigger_t;

void odometrieSetup(void);
void printBuffer(void);
void printPosition(void);
void odometrieLoop(void);
#pragma once

#include "simulation.h"


void I2cSendData (uint8_t command, uint8_t* data, int length);
void I2cReceiveData (uint8_t command, uint8_t* data, int length);
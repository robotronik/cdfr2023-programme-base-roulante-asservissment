#pragma once

#include "i2c_interface.h"
#include "sequence.h"


int linearCalibration(i2c_interface* robotI2cInterface);
int angularCalibration(i2c_interface* robotI2cInterface);

int rotation10(i2c_interface* robotI2cInterface);
int longLine(i2c_interface* robotI2cInterface);
int testloop(i2c_interface* robotI2cInterface);
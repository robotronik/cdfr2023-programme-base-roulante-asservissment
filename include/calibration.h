#pragma once
#include "i2c_interface.h"
#include "clock.h"
#include "odometrie.h"

enum FsmCalibration {
    WAIT_START,
    START,
    INIT,

    STEP1,
    WAIT_END_STEP1,
    WAIT_TIME_STEP1,

    STEP2,
    WAIT_END_STEP2,
    WAIT_TIME_STEP2,

    FINISH,
    COMPUTE,
    END
};

bool loopCalibration(i2c_interface* robot);
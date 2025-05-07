#include "Odometry.h"


#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/cm3/nvic.h>
    #include <libopencm3/stm32/exti.h>
#endif

#include <math.h>

#include "config.h"
#include "clock.h"
#include "uart.h"

void odometrySetup(void){
    // setup the i2c and shit
}

void odometryLoop(position_t &position){
    // get the position from the encoders
    // update the position
    // update the speed
    // update the angle
}
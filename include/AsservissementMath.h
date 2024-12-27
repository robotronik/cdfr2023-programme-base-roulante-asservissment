#pragma once

#include <math.h>
#include "config.h"


#define RAD_TO_DEG 57.29577951
#define DEG_TO_RAD 0.01745329252


double mod_angle(double a);
double calculAngle(double x, double y, position_t actualPostion);



class LowPassFilter {
private:
    double prev_filtered;
    double tau;
    bool first_sample;
    uint32_t prev_time;

public:
    LowPassFilter(double tau): tau(tau){
        reset();
    }

    void reset(void){
        prev_filtered = 0.0;
        first_sample = true;
    }

    double update(double raw_value, uint32_t current_time_us) {
        double delta_t = 0.0;
        if (!first_sample) {
            delta_t = (current_time_us - prev_time)/1000000;
        } else {
            first_sample = false;
        }
        prev_time = current_time_us;

        double alpha = delta_t / (tau + delta_t);

        prev_filtered = alpha * raw_value + (1.0 - alpha) * prev_filtered;

        return prev_filtered;
    }
};

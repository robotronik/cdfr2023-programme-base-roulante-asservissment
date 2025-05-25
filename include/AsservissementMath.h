#pragma once

#include <math.h>
#include "config.h"


double mod_angle(double a);
double calculAngle(double x, double y, position_t actualPostion);
double map(double x, double in_min, double in_max, double out_min, double out_max);



class LowPassFilter {
private:
    double prev_filtered;
    double tau_filter;
    bool first_sample;
    uint32_t prev_time;

public:
    LowPassFilter(double tau): tau_filter(tau){
        reset();
    }

    void reset(void){
        prev_filtered = 0.0;
        first_sample = true;
    }

    double update(double raw_value, uint32_t current_time_ms) {
        double delta_t = 0.0;
        if (!first_sample) {
            delta_t = ((double)(current_time_ms - prev_time))/1000;
        } else {
            first_sample = false;
        }
        prev_time = current_time_ms;

        double alpha = delta_t / (tau_filter + delta_t);

        prev_filtered = alpha * raw_value + (1.0 - alpha) * prev_filtered;

        return prev_filtered;
    }
};

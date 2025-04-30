#pragma once


#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/stm32/rcc.h>
    #include <libopencm3/stm32/gpio.h>
    #include <libopencm3/cm3/nvic.h>
    #include <libopencm3/stm32/timer.h>
    #include <libopencm3/stm32/adc.h>
#endif

#include "config.h"


// Freq = 84Mhz / TIMERPERIOD
// The ideal frequency is arround 8kHz but it creates a noise unpleasant to the ear
// with a period of 4000, this creates a freq of 21kHz, that people cant hear
#define TIMERPERIOD 4000
#define COEFMULT TIMERPERIOD/100
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

class Motor
{
public:
    Motor(int motorID);

    void Setup();
    void setupGPIO();


    // Set the speed of the motor with a value between -100 and 100
    // 0 Stops the motor
    // Negative value are use to backward
    // Don't forget to disable reset, coast and brake
    void SetSpeedSigned(int speed);

    // Set the speed of the motor with a value between 0 and 100
    // 0 Stops the motor
    // Don't forget to disable reset, coast and brake
    void SetSpeedUnsigned(int speed, bool reverse);

    void Brake(bool brake);

    void Disable();
    void Enable();

    // value must be bettween 0 and 100
    void SetMaxTorque(int torque);

    // value must be bettween 0 and 100
    void SetMaxSpeed(int max);

    // Returns the current in A
    double GetCurrent();

    // Print useful values
    void PrintValues();

private:

    // Set motor driver direction
    void SetDirection(bool reverse);

    // Set the speed of the motor with a value between 0 and 100
    // 0 Stops the motor
    // Don't forget to disable reset, coast and brake
    void SetSpeed(int speed);

    // Set the current-decay method.
    void SetMode(int motorMode);

    int id;
    char name;
    int mode;
    uint16_t adc_value = 0;
    int maxTorque = 4096;
    bool motorEn = true;
    int maxSpeed = 100;
    bool doesLimitTorque = false;

    // Periferals

    // ADC

    int adc_channel;

    // Pin definitions

    // Output
    int port_Reset;
    int pin_Reset;
    int port_Coast;
    int pin_Coast;
    int port_Mode;
    int pin_Mode;
    int port_SpeedControl;
    int pin_SpeedControl;
    int port_Direction;
    int pin_Direction;
    int port_Brake;
    int pin_Brake;
    int port_ESF;
    int pin_ESF;

    // Input
    int port_Tacho;
    int pin_Tacho;
    int port_Err1;
    int pin_Err1;
    int port_Err2;
    int pin_Err2;
    int port_InfoDir;
    int pin_InfoDir;
    // int port_Current;
    // int pin_Current;
};

// Create the motor objects

Motor motorA(0);
Motor motorB(1);
Motor motorC(2);
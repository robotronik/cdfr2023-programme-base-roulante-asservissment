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

// typedef Enum containing the different fault actions
typedef enum {
    FAULT_NONE = 0,
    FAULT_UNDERVOLTAGE = 1,
    FAULT_OVERTEMPERATURE = 2,
    FAULT_LOGIC = 3,
    FAULT_SHORT = 4,
    FAULT_LOW_LOAD_CURRENT = 5
} fault_action_t;

// Coast
bool driveEnabled = false;

void DriveDisable();
void DriveEnable();
// Set the current-decay method.
void SetDriveMode(int mode);
// Reset the motor drivers by lowering the reset pin for 10ms
void ResetDrive();
void DriveSetup();


class Motor
{
public:
    Motor(int motorID, double wheelDiameter, double wheelDistance, double wheelAngle, 
        int port_SpeedControl, int pin_SpeedControl,
        int port_Direction, int pin_Direction,
        int port_Brake, int pin_Brake,
        int port_ESF, int pin_ESF,
        int port_Tacho, int pin_Tacho,
        int port_Err1, int pin_Err1,
        int port_Err2, int pin_Err2,
        int port_InfoDir, int pin_InfoDir,
        tim_oc_id oc_id
    );

    void Setup();

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


    // value must be bettween 0 and 100
    void SetMaxTorque(int torque);

    // value must be bettween 0 and 100
    void SetMaxSpeed(int max);

    // Returns the current in A
    double GetCurrent();

    fault_action_t GetFault();

    // Print useful values
    void PrintValues();

private:

    void setupGPIO();

    // Set motor driver direction
    void SetDirection(bool reverse);

    // Set the speed of the motor with a value between 0 and 100
    // 0 Stops the motor
    // Don't forget to disable reset, coast and brake
    void SetSpeed(int speed);


    int id;
    char name;

    double wheelDiameter;
    double wheelDistance;
    double wheelAngle;

    uint16_t adc_value = 0;
    int maxTorque = 4096;
    bool motorEn = true;
    int maxSpeed = 100;
    bool doesLimitTorque = false;

    // Periferals
    // PWM timer
    tim_oc_id oc_id = TIM_OC2; // OC2 for motor A, OC1 for motor B, OC3 for motor C

    // Pin definitions

    // Output
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

Motor motorA(0, DIAMETER_WHEEL, DISTANCE_WHEEL, 0.0, 
    port_SpeedControlA, pin_SpeedControlA,
    port_DirectionA, pin_DirectionA,
    port_BrakeA, pin_BrakeA,
    port_ESFA, pin_ESFA,
    port_TachoA, pin_TachoA,
    port_Err1A, pin_Err1A,
    port_Err2A, pin_Err2A,
    port_InfoDirA, pin_InfoDirA,
    TIM_OC2);
Motor motorB(1, DIAMETER_WHEEL, DISTANCE_WHEEL, 120.0, 
    port_SpeedControlB, pin_SpeedControlB,
    port_DirectionB, pin_DirectionB,
    port_BrakeB, pin_BrakeB,
    port_ESFB, pin_ESFB,
    port_TachoB, pin_TachoB,
    port_Err1B, pin_Err1B,
    port_Err2B, pin_Err2B,
    port_InfoDirB, pin_InfoDirB,
    TIM_OC1);
Motor motorC(2, DIAMETER_WHEEL, DISTANCE_WHEEL, 240.0, 
    port_SpeedControlC, pin_SpeedControlC,
    port_DirectionC, pin_DirectionC,
    port_BrakeC, pin_BrakeC,
    port_ESFC, pin_ESFC,
    port_TachoC, pin_TachoC,
    port_Err1C, pin_Err1C,
    port_Err2C, pin_Err2C,
    port_InfoDirC, pin_InfoDirC,
    TIM_OC3);
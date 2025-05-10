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

// typedef Enum containing the different fault actions
typedef enum {
    FAULT_NONE = 0,
    FAULT_UNDERVOLTAGE = 1,
    FAULT_OVERTEMPERATURE = 2,
    FAULT_LOGIC = 3,
    FAULT_SHORT = 4,
    FAULT_LOW_LOAD_CURRENT = 5
} fault_action_t;

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

    Motor(int motorID, int port_SpeedControl, int pin_SpeedControl,
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

    uint16_t adc_value = 0;
    int maxTorque = 4096;
    int maxSpeed = 100;
    bool doesLimitTorque = false;

    // Periferals

    // Pin definitions

    // Output
    int _port_SpeedControl;
    int _pin_SpeedControl;
    int _port_Direction;
    int _pin_Direction;
    int _port_Brake;
    int _pin_Brake;
    int _port_ESF;
    int _pin_ESF;

    // Input
    int _port_Tacho;
    int _pin_Tacho;
    int _port_Err1;
    int _pin_Err1;
    int _port_Err2;
    int _pin_Err2;
    int _port_InfoDir;
    int _pin_InfoDir;
    // int _port_Current;
    // int _pin_Current;

    // PWM timer
    tim_oc_id _oc_id; // OC2 for motor A, OC1 for motor B, OC3 for motor C
};

// Create the motor objects
extern Motor motorA;
extern Motor motorB;
extern Motor motorC;
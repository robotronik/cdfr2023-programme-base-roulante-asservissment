#pragma once

#include "robot_interface.h"
#include "movement.h"
#include "Motor.h"
#include "led.h"
#include "I2C.h"

class i2c_interface : public Robot_interface
{
private:
    position posRobot;
    movement robotAsserv;
public:
    i2c_interface(position& inRobotPosition, movement& inRobotAsservisement);

    void get_version(uint16_t &part1, uint16_t &part2, uint16_t &part3, uint16_t &part4);
    void set_led_1(bool status);
    void set_led_2(bool status);
    void get_coordinates(int16_t &x, int16_t &y, int16_t &theta);
    void set_coordinates(int16_t x, int16_t y, int16_t theta);
    void stop();
    void pause();
    void resume();
    void go_to_point(int16_t x,int16_t y,Rotation rotation = Rotation::SHORTEST, Direction direction = Direction::FORWARD);
    void go_to_point(int16_t x,int16_t y,int16_t theta, Rotation rotationFirst = Rotation::SHORTEST, Direction direction = Direction::FORWARD, Rotation rotationSecond = Rotation::SHORTEST);
    void consigne_angulaire(int16_t angle, Rotation rotation = Rotation::SHORTEST);
    void consigne_angulaire(int16_t x, int16_t y, Rotation rotation = Rotation::SHORTEST, Direction direction = Direction::FORWARD);
    void set_linear_max_speed(int16_t max_speed, int16_t max_acceleration = 0, int16_t max_deceleration = 0);
    void set_angular_max_speed(int16_t max_speed, int16_t max_acceleration = 0, int16_t max_deceleration = 0);
    int16_t get_braking_distance();
    int16_t get_command_buffer_size();
    Direction get_direction_side();
    Rotation get_rotation_side();
    void get_current_target(int16_t &x, int16_t &y, int16_t &theta);
    bool get_moving_is_done();
    bool get_running_is_done();
    bool get_turning_is_done();
    int16_t get_linear_error();
    int16_t get_angular_error();
    void get_current(int16_t &currentRight, int16_t &currentLeft);
    void get_speed(int16_t &speedRight, int16_t &speedLeft);
    void set_motor_state(bool motorEnable);
    void set_brake_state(bool brakeEnable);
    void set_max_torque(int16_t max_torque);
    void set_linear_position_control(int16_t max_speed_for, int16_t max_speed_back, int16_t max_acceleration_for, int16_t max_acceleration_back, int16_t max_deceleration_for, int16_t max_deceleration_back);
    void set_angular_position_control(int16_t max_speed_clock, int16_t max_speed_anti, int16_t max_acceleration_clock, int16_t max_acceleration_anti, int16_t max_deceleration_clock, int16_t max_deceleration_anti);
    void set_pid_linear_static(int16_t p, int16_t i, int16_t d);
    void set_pid_linear_dynamic(int16_t p, int16_t i, int16_t d);
    void set_pid_angular_static(int16_t p, int16_t i, int16_t d);
    void set_pid_angular_dynamic(int16_t p, int16_t i, int16_t d);
    void set_odometry_metric(int16_t sizeWheelLeft, int16_t sizeWheelRight, int16_t spaceInterWheel);
    int16_t get_max_torque();
    void get_linear_position_control(int16_t &max_speed_for, int16_t &max_speed_back, int16_t &max_acceleration_for, int16_t &max_acceleration_back, int16_t &max_deceleration_for, int16_t &max_deceleration_back);
    void get_angular_position_control(int16_t &max_speed_clock, int16_t &max_speed_anti, int16_t &max_acceleration_clock, int16_t &max_acceleration_anti, int16_t &max_deceleration_clock, int16_t &max_deceleration_anti);
    void get_pid_linear_static(int16_t &p, int16_t &i, int16_t &d);
    void get_pid_linear_dynamic(int16_t &p, int16_t &i, int16_t &d);
    void get_pid_angular_static(int16_t &p, int16_t &i, int16_t &d);
    void get_pid_angular_dynamic(int16_t &p, int16_t &i, int16_t &d);
    void get_odometry_metric(int16_t &sizeWheelLeft, int16_t &sizeWheelRight, int16_t &spaceInterWheel);
    void start_calibration();
    void end_calibration();
    void get_status_calibration();
    void get_all_status();
    void get_all_error();
    void get_all_parameter();
    void set_all_parameter();

    void setReponseBuffer(uint8_t* data, int size);
};



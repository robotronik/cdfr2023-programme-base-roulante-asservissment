#pragma once

#include "robot_interface.h"
#include "movement.h"
#include "motor.h"
#include "led.h"
#include "I2C.h"

class i2c_interface : public Robot_interface
{
private:
    position* robotPosition;
    movement* robotAsservisement;
public:
    i2c_interface(position* inRobotPosition, movement* inRobotAsservisement);

    void set_led_1(bool status);
    void set_led_2(bool status);
    void get_coordinates(int16_t &x, int16_t &y, int16_t &theta);
    void set_coordinates(int16_t x, int16_t y, int16_t theta);
    void stop();
    void pause();
    void resume();
    void go_to_point(int16_t x,int16_t y,Rotation rotation = Rotation::SHORTEST, Direction direction = Direction::FORWARD);
    void go_to_point(int16_t x,int16_t y,int16_t theta, Rotation rotationFirst, Direction direction, Rotation rotationSecond);
    void consigne_angulaire(int16_t angle, Rotation rotation = Rotation::SHORTEST);
    void consigne_angulaire(int16_t x, int16_t y, Rotation rotation = Rotation::SHORTEST, Direction direction = Direction::FORWARD);
    void get_braking_distance(int16_t &dist);
    void get_commande_buffer_size(int16_t &size);
    void get_direction_side(Direction &direction);
    void get_rotation_side(Rotation &rotation);
    void get_current_target(int16_t &x, int16_t &y, int16_t &theta);
    void get_moving_is_done(bool &state);
    void get_running_is_done(bool &state);
    void get_turning_is_done(bool &state);
    void get_linear_error(int16_t &error);
    void get_angular_error(int16_t &error);
    void get_current(int16_t &currentRigth, int16_t &currentLeft);
    void get_speed(int16_t &speedRigth, int16_t &speedLeft);
    void set_motor_state(bool motorEnable);
    void set_brake_state(bool brakeEnable);
    void set_max_torque(int16_t max_torque);
    void set_max_speed_forward(int16_t speed);
    void set_max_speed_backward(int16_t speed);
    void set_max_speed_trigo(int16_t speed);
    void set_max_speed_horloge(int16_t speed);
    void set_pid_linear_static(int16_t p, int16_t i, int16_t d);
    void set_pid_linear_dynamic(int16_t p, int16_t i, int16_t d);
    void set_pid_angular_static(int16_t p, int16_t i, int16_t d);
    void set_pid_angular_dynamic(int16_t p, int16_t i, int16_t d);
    void set_odometry_metric(int16_t sizeWheelLeft, int16_t sizeWheelRigth, int16_t spaceInterWheel);
    void get_max_torque(int16_t max_torque);
    void get_max_speed_forward(int16_t speed);
    void get_max_speed_backward(int16_t speed);
    void get_max_speed_trigo(int16_t speed);
    void get_max_speed_horloge(int16_t speed);
    void get_pid_linear_static(int16_t &p, int16_t &i, int16_t &d);
    void get_pid_linear_dynamic(int16_t &p, int16_t &i, int16_t &d);
    void get_pid_angular_static(int16_t &p, int16_t &i, int16_t &d);
    void get_pid_angular_dynamic(int16_t &p, int16_t &i, int16_t &d);
    void get_odometry_metric(int16_t &sizeWheelLeft, int16_t &sizeWheelRigth, int16_t &spaceInterWheel);
    void start_calibration();
    void end_calibration();
    void get_status_calibration();
    void get_all_status();
    void get_all_error();
    void get_all_parameter();
    void set_all_parameter();

    void setReponseBuffer(uint8_t* data, int size);
};



#include "i2c_interface.h"


void i2c_interface::I2CSetBuffer(uint8_t* data, int size){

}



void i2c_interface::turn_on_LED_1(){

}

void i2c_interface::turn_off_LED_1(){

}

void i2c_interface::turn_on_LED_2(){

}

void i2c_interface::turn_off_LED_2(){

}

void i2c_interface::get_coordinates(int16_t &x, int16_t &y, int16_t &theta){

}

void i2c_interface::set_coordinates(int16_t x, int16_t y, int16_t theta){

}

void i2c_interface::stop(){

}

void i2c_interface::set_consigne_lineaire(int16_t x, int16_t y){

}

void i2c_interface::set_consigne_angulaire(int16_t angle, int16_t rotation){

}

void i2c_interface::set_consigne_lookAt_forward(int16_t x, int16_t y, int16_t rotation){

}

void i2c_interface::set_consigne_lookAt_backward(int16_t x, int16_t y, int16_t rotation){

}

void i2c_interface::robot_moving_is_finish(int16_t &resbool){

}

void i2c_interface::robot_running_is_finish(int16_t &resbool){

}

void i2c_interface::robot_turning_is_finish(int16_t &resbool){

}

void i2c_interface::get_linear_error(int16_t &error){

}

void i2c_interface::get_angular_error(int16_t &error){

}

void i2c_interface::get_braking_distance(int16_t &distance){

}

void i2c_interface::get_robot_running(bool &robot_runnning){

}

void i2c_interface::get_directio_side(Direction &direction_side){

}

void i2c_interface::get_rotation_side(Rotation &rotation_side){

}

void i2c_interface::get_current_consigne(int16_t &x, int16_t &y, int16_t &theta){

}

void i2c_interface::disable_motor(){

}

void i2c_interface::enable_motor(){

}

void i2c_interface::brake_on(){

}

void i2c_interface::brake_off(){

}

void i2c_interface::set_max_torque(int16_t max_torque){

}

void i2c_interface::set_max_speed_forward(int16_t speed){

}

void i2c_interface::set_max_speed_backward(int16_t speed){

}

void i2c_interface::set_max_speed_trigo(int16_t speed){

}

void i2c_interface::set_max_speed_horloge(int16_t speed){

}

void i2c_interface::go_to_point(int16_t x,int16_t y,Rotation rotation, Direction direction){

}

void i2c_interface::go_to_point(int16_t x,int16_t y,int16_t theta, Rotation rotationFirst, Direction direction, Rotation rotationSecond){

}

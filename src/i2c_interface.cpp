#include "i2c_interface.h"

i2c_interface::i2c_interface(position& inRobotPosition, movement& inRobotAsservisement):
posRobot(inRobotPosition),
robotAsserv(inRobotAsservisement){

}

void i2c_interface::get_version(uint16_t &part1, uint16_t &part2, uint16_t &part3, uint16_t &part4) {
    part1 = GIT_COMMIT_SHA_PART1;
    part2 = GIT_COMMIT_SHA_PART2;
    part3 = GIT_COMMIT_SHA_PART3;
    part4 = GIT_COMMIT_SHA_PART4;
}

void i2c_interface::setReponseBuffer(uint8_t* data, int size){
    I2CSetBuffer(data,size);
}


void i2c_interface::set_led_1(bool status){
    if(status)
        gpio_set(port_led1,pin_led1);
    else
        gpio_clear(port_led1,pin_led1);
}



void i2c_interface::set_led_2(bool status){
    if(status)
        gpio_set(port_led2,pin_led2);
    else
        gpio_clear(port_led2,pin_led2);
}


void i2c_interface::get_coordinates(int16_t &x, int16_t &y, int16_t &theta){
    position_t posi = posRobot.getPosition();
    x = posi.x;
    y = posi.y;
    theta = posi.teta;
}

void i2c_interface::set_coordinates(int16_t x, int16_t y, int16_t theta){
    posRobot.setPosition(x,y,theta);
}

void i2c_interface::stop(){
    robotAsserv.setConsigneStop();
}

void i2c_interface::pause(){
    robotAsserv.setConsignePause();
}

void i2c_interface::resume(){
    robotAsserv.setConsigneResume();
}

void i2c_interface::go_to_point(int16_t x,int16_t y,Rotation rotation, Direction direction){
    robotAsserv.goToPoint(x, y, rotation, direction);
}

void i2c_interface::go_to_point(int16_t x,int16_t y,int16_t theta, Rotation rotationFirst, Direction direction, Rotation rotationSecond){
    robotAsserv.goToPoint(x, y, theta, rotationFirst, direction, rotationSecond);
}

void i2c_interface::consigne_angulaire(int16_t angle, Rotation rotation){
    robotAsserv.setConsigneAngulaire(angle,(Rotation)rotation);
}

void i2c_interface::consigne_angulaire(int16_t x, int16_t y, Rotation rotation, Direction direction){
    robotAsserv.setConsigneLookAt(x ,y ,rotation,direction);
}

void i2c_interface::set_linear_max_speed(int16_t max_speed, int16_t max_acceleration, int16_t max_deceleration){
    robotAsserv.setConsigneMaxSpeedLinear(max_speed,max_acceleration,max_deceleration);
}

void i2c_interface::set_angular_max_speed(int16_t max_speed, int16_t max_acceleration, int16_t max_deceleration){
    robotAsserv.setConsigneMaxSpeedAngular(max_speed,max_acceleration,max_deceleration);
}

int16_t i2c_interface::get_braking_distance(){
    return (int16_t)robotAsserv.getBrakingDistance();
}

int16_t i2c_interface::get_command_buffer_size(){
    return robotAsserv.getCommandBufferSize();
}

Direction i2c_interface::get_direction_side(){
   return robotAsserv.getDirectionSide();
}

Rotation i2c_interface::get_rotation_side(){
    return robotAsserv.getRotationSide();
}

void i2c_interface::get_current_target(int16_t &x, int16_t &y, int16_t &theta){
    position_t consigne = robotAsserv.getCurrentConsigne();
    x = consigne.x;
    y = consigne.y;
    theta = consigne.teta;
}

bool i2c_interface::get_moving_is_done(){
    return !robotAsserv.robotMoving();
}

bool i2c_interface::get_running_is_done(){
    return !robotAsserv.robotRunning();
}

bool i2c_interface::get_turning_is_done(){
    return !robotAsserv.robotTurning();
}

int16_t i2c_interface::get_linear_error(){
    return (int16_t)robotAsserv.getLinearError();
}

int16_t i2c_interface::get_angular_error(){
    return (int16_t)robotAsserv.getAngularError();
}

void i2c_interface::get_current(int16_t &currentRight, int16_t &currentLeft){
    // TODO
}

void i2c_interface::get_speed(int16_t &speedRight, int16_t &speedLeft){
    // TODO
}

void i2c_interface::set_motor_state(bool motorEnable){
    if(motorEnable){
        robotAsserv.reset();
        motorA.SetSpeedSigned(0);
        motorB.SetSpeedSigned(0);
        motorC.SetSpeedSigned(0);
        motorA.Enable();
        motorB.Enable();
        motorC.Enable();
    }
    else{
        motorA.Disable();
        motorB.Disable();
        motorC.Disable();
    }
}

void i2c_interface::set_brake_state(bool brakeEnable){
    if(brakeEnable){
        motorA.Brake(true);
        motorB.Brake(true);
        motorC.Brake(true);
    }
    else{
        robotAsserv.reset();
        motorA.SetSpeedSigned(0);
        motorB.SetSpeedSigned(0);
        motorC.SetSpeedSigned(0);
        motorA.Brake(false);
        motorB.Brake(false);
        motorC.Brake(false);
    }
}

void i2c_interface::set_max_torque(int16_t max_torque){
    motorA.SetMaxTorque(max_torque);
    motorB.SetMaxTorque(max_torque);
    motorC.SetMaxTorque(max_torque);
}

void i2c_interface::set_linear_position_control(int16_t max_speed_for, int16_t max_speed_back, int16_t max_acceleration_for, int16_t max_acceleration_back, int16_t max_deceleration_for, int16_t max_deceleration_back){
    robotAsserv.positionControlLineaire.vitesseMaxAv.setRange(0,max_speed_for);
    robotAsserv.positionControlLineaire.vitesseMaxAr.setRange(0,max_speed_back);
    robotAsserv.positionControlLineaire.accelerationMaxAv.setRange(0,max_acceleration_for);
    robotAsserv.positionControlLineaire.accelerationMaxAr.setRange(0,max_acceleration_back);
    robotAsserv.positionControlLineaire.decelerationMaxAv.setRange(0,max_deceleration_for);
    robotAsserv.positionControlLineaire.decelerationMaxAr.setRange(0,max_deceleration_back);
}

void i2c_interface::set_angular_position_control(int16_t max_speed_clock, int16_t max_speed_anti, int16_t max_acceleration_clock, int16_t max_acceleration_anti, int16_t max_deceleration_clock, int16_t max_deceleration_anti){
    robotAsserv.positionControlAngulaire.vitesseMaxAv.setRange(0,max_speed_clock);
    robotAsserv.positionControlAngulaire.vitesseMaxAr.setRange(0,max_speed_anti);
    robotAsserv.positionControlAngulaire.accelerationMaxAv.setRange(0,max_acceleration_clock);
    robotAsserv.positionControlAngulaire.accelerationMaxAr.setRange(0,max_acceleration_anti);
    robotAsserv.positionControlAngulaire.decelerationMaxAv.setRange(0,max_deceleration_clock);
    robotAsserv.positionControlAngulaire.decelerationMaxAr.setRange(0,max_deceleration_anti);
}

void i2c_interface::set_pid_linear_static(int16_t p, int16_t i, int16_t d){
    robotAsserv.pidLineaireBlock.setPID(p,i,d);
}

void i2c_interface::set_pid_linear_dynamic(int16_t p, int16_t i, int16_t d){
    robotAsserv.pidLineaire.setPID(p,i,d);
}

void i2c_interface::set_pid_angular_static(int16_t p, int16_t i, int16_t d){
    robotAsserv.pidAngulaireBlock.setPID(p,i,d);
}

void i2c_interface::set_pid_angular_dynamic(int16_t p, int16_t i, int16_t d){
    robotAsserv.pidAngulaire.setPID(p,i,d);
}

void i2c_interface::set_odometry_metric(int16_t sizeWheelLeft, int16_t sizeWheelRight, int16_t spaceInterWheel){

}

int16_t i2c_interface::get_max_torque(){
    return 0;// TODO
}

void i2c_interface::get_linear_position_control(int16_t &max_speed_for, int16_t &max_speed_back, int16_t &max_acceleration_for, int16_t &max_acceleration_back, int16_t &max_deceleration_for, int16_t &max_deceleration_back){
    max_speed_for = robotAsserv.positionControlLineaire.vitesseMaxAv.getMaxValue();
    max_speed_back = robotAsserv.positionControlLineaire.vitesseMaxAr.getMaxValue();
    max_acceleration_for = robotAsserv.positionControlLineaire.accelerationMaxAv.getMaxValue();
    max_acceleration_back = robotAsserv.positionControlLineaire.accelerationMaxAr.getMaxValue();
    max_deceleration_for = robotAsserv.positionControlLineaire.decelerationMaxAv.getMaxValue();
    max_deceleration_back = robotAsserv.positionControlLineaire.decelerationMaxAr.getMaxValue();
}
void i2c_interface::get_angular_position_control(int16_t &max_speed_clock, int16_t &max_speed_anti, int16_t &max_acceleration_clock, int16_t &max_acceleration_anti, int16_t &max_deceleration_clock, int16_t &max_deceleration_anti){
    max_speed_clock = robotAsserv.positionControlAngulaire.vitesseMaxAv.getMaxValue();
    max_speed_anti = robotAsserv.positionControlAngulaire.vitesseMaxAr.getMaxValue();
    max_acceleration_clock = robotAsserv.positionControlAngulaire.accelerationMaxAv.getMaxValue();
    max_acceleration_anti = robotAsserv.positionControlAngulaire.accelerationMaxAr.getMaxValue();
    max_deceleration_clock = robotAsserv.positionControlAngulaire.decelerationMaxAv.getMaxValue();
    max_deceleration_anti = robotAsserv.positionControlAngulaire.decelerationMaxAr.getMaxValue();
}

void i2c_interface::get_pid_linear_static(int16_t &p, int16_t &i, int16_t &d){
    robotAsserv.pidLineaireBlock.setPID(p,i,d);
}

void i2c_interface::get_pid_linear_dynamic(int16_t &p, int16_t &i, int16_t &d){
    robotAsserv.pidLineaire.setPID(p,i,d);
}

void i2c_interface::get_pid_angular_static(int16_t &p, int16_t &i, int16_t &d){
    robotAsserv.pidAngulaireBlock.setPID(p,i,d);
}

void i2c_interface::get_pid_angular_dynamic(int16_t &p, int16_t &i, int16_t &d){
    robotAsserv.pidAngulaire.setPID(p,i,d);
}

void i2c_interface::get_odometry_metric(int16_t &sizeWheelLeft, int16_t &sizeWheelRight, int16_t &spaceInterWheel){
    // TODO
}

void i2c_interface::start_calibration(){
    // TODO
}

void i2c_interface::end_calibration(){
    // TODO
}

void i2c_interface::get_status_calibration(){
    // TODO
}

void i2c_interface::get_all_status(){
    // TODO
}

void i2c_interface::get_all_error(){
    // TODO
}

void i2c_interface::get_all_parameter(){
    // TODO
}

void i2c_interface::set_all_parameter(){
    // TODO
}

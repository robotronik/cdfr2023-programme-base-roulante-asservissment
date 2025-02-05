#include "i2c_interface.h"

i2c_interface::i2c_interface(position* inRobotPosition, movement* inRobotAsservisement):
robotPosition(inRobotPosition),
robotAsservisement(inRobotAsservisement){

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
    position_t posi = robotPosition->getPosition();
    x = posi.x;
    y = posi.y;
    theta = posi.teta;
}

void i2c_interface::set_coordinates(int16_t x, int16_t y, int16_t theta){
    robotPosition->setPosition(x,y,theta);
}

void i2c_interface::stop(){
    robotAsservisement->setConsigneStop();
}

void i2c_interface::pause(){
    robotAsservisement->setConsigneStop();
}

void i2c_interface::resume(){
    robotAsservisement->setConsigneStop();
}

void i2c_interface::go_to_point(int16_t x,int16_t y,Rotation rotation, Direction direction){
    robotAsservisement->goToPoint(x, y, rotation, direction);
}

void i2c_interface::go_to_point(int16_t x,int16_t y,int16_t theta, Rotation rotationFirst, Direction direction, Rotation rotationSecond){
    robotAsservisement->goToPoint(x, y, theta, rotationFirst, direction, rotationSecond);
}

void i2c_interface::consigne_angulaire(int16_t angle, Rotation rotation){
    robotAsservisement->setConsigneAngulaire(angle,(Rotation)rotation);
}

void i2c_interface::consigne_angulaire(int16_t x, int16_t y, Rotation rotation, Direction direction){
    if(direction == Direction::FORWARD)
        robotAsservisement->setConsigneLookAtForward(x ,y ,rotation);
    else
        robotAsservisement->setConsigneLookAtBackward(x ,y ,rotation);
}

void i2c_interface::set_linear_max_speed(int16_t max_speed, int16_t max_acceleration, int16_t max_deceleration){
    robotAsservisement->setConsigneMaxSpeedLinear(max_speed,max_acceleration,max_deceleration);
}

void i2c_interface::set_angular_max_speed(int16_t max_speed, int16_t max_acceleration, int16_t max_deceleration){
    robotAsservisement->setConsigneMaxSpeedAngular(max_speed,max_acceleration,max_deceleration);
}

int16_t i2c_interface::get_braking_distance(){
    return (int16_t)robotAsservisement->getBrakingDistance();
}

int16_t i2c_interface::get_command_buffer_size(){
    return robotAsservisement->getCommandBufferSize();
}

Direction i2c_interface::get_direction_side(){
   return robotAsservisement->getDirectionSide();
}

Rotation i2c_interface::get_rotation_side(){
    return robotAsservisement->getRotationSide();
}

void i2c_interface::get_current_target(int16_t &x, int16_t &y, int16_t &theta){
    position_t consigne = robotAsservisement->getCurrentConsigne();
    x = consigne.x;
    y = consigne.y;
    theta = consigne.teta;
}

bool i2c_interface::get_moving_is_done(){
    return !robotAsservisement->robotMoving();
}

bool i2c_interface::get_running_is_done(){
    return !robotAsservisement->robotRunning();
}

bool i2c_interface::get_turning_is_done(){
    return !robotAsservisement->robotTurning();
}

int16_t i2c_interface::get_linear_error(){
    return (int16_t)robotAsservisement->getLinearError();
}

int16_t i2c_interface::get_angular_error(){
    return (int16_t)robotAsservisement->getAngularError();
}

void i2c_interface::get_current(int16_t &currentRigth, int16_t &currentLeft){
    // TODO
}

void i2c_interface::get_speed(int16_t &speedRigth, int16_t &speedLeft){
    // TODO
}

void i2c_interface::set_motor_state(bool motorEnable){
    if(motorEnable){
        robotAsservisement->reset();
        motorSpeedSignedL(0);
        motorSpeedSignedR(0);
        enableMotor();
    }
    else{
        disableMotor();
    }
}

void i2c_interface::set_brake_state(bool brakeEnable){
    if(brakeEnable){
        motorBrakeL(true);
        motorBrakeR(true);
    }
    else{
        robotAsservisement->reset();
        motorSpeedSignedL(0);
        motorSpeedSignedR(0);
        motorBrakeL(false);
        motorBrakeR(false);
    }
}

void i2c_interface::set_max_torque(int16_t max_torque){
    setMaxTorqueL(max_torque);
    setMaxTorqueR(max_torque);
}

void i2c_interface::set_linear_position_control(int16_t max_speed_for, int16_t max_speed_back, int16_t max_acceleration_for, int16_t max_acceleration_back, int16_t max_deceleration_for, int16_t max_deceleration_back){
    robotAsservisement->positionControlLineaire.vitesseMaxAv.setMaxValue(max_speed_for);
    robotAsservisement->positionControlLineaire.vitesseMaxAr.setMaxValue(max_speed_back);
    robotAsservisement->positionControlLineaire.accelerationMaxAv.setMaxValue(max_acceleration_for);
    robotAsservisement->positionControlLineaire.accelerationMaxAr.setMaxValue(max_acceleration_back);
    robotAsservisement->positionControlLineaire.decelerationMaxAv.setMaxValue(max_deceleration_for);
    robotAsservisement->positionControlLineaire.decelerationMaxAr.setMaxValue(max_deceleration_back);
}

void i2c_interface::set_angular_position_control(int16_t max_speed_clock, int16_t max_speed_anti, int16_t max_acceleration_clock, int16_t max_acceleration_anti, int16_t max_deceleration_clock, int16_t max_deceleration_anti){
    robotAsservisement->positionControlAngulaire.vitesseMaxAv.setMaxValue(max_speed_clock);
    robotAsservisement->positionControlAngulaire.vitesseMaxAr.setMaxValue(max_speed_anti);
    robotAsservisement->positionControlAngulaire.accelerationMaxAv.setMaxValue(max_acceleration_clock);
    robotAsservisement->positionControlAngulaire.accelerationMaxAr.setMaxValue(max_acceleration_anti);
    robotAsservisement->positionControlAngulaire.decelerationMaxAv.setMaxValue(max_deceleration_clock);
    robotAsservisement->positionControlAngulaire.decelerationMaxAr.setMaxValue(max_deceleration_anti);
}

void i2c_interface::set_pid_linear_static(int16_t p, int16_t i, int16_t d){
    robotAsservisement->pidLineaireBlock.setPID(p,i,d);
}

void i2c_interface::set_pid_linear_dynamic(int16_t p, int16_t i, int16_t d){
    robotAsservisement->pidLineaire.setPID(p,i,d);
}

void i2c_interface::set_pid_angular_static(int16_t p, int16_t i, int16_t d){
    robotAsservisement->pidAngulaireBlock.setPID(p,i,d);
}

void i2c_interface::set_pid_angular_dynamic(int16_t p, int16_t i, int16_t d){
    robotAsservisement->pidAngulaire.setPID(p,i,d);
}

void i2c_interface::set_odometry_metric(int16_t sizeWheelLeft, int16_t sizeWheelRigth, int16_t spaceInterWheel){

}

int16_t i2c_interface::get_max_torque(){
    return 0;// TODO
}

void i2c_interface::get_linear_position_control(int16_t &max_speed_for, int16_t &max_speed_back, int16_t &max_acceleration_for, int16_t &max_acceleration_back, int16_t &max_deceleration_for, int16_t &max_deceleration_back){
    max_speed_for = robotAsservisement->positionControlLineaire.vitesseMaxAv.getMaxValue();
    max_speed_back = robotAsservisement->positionControlLineaire.vitesseMaxAr.getMaxValue();
    max_acceleration_for = robotAsservisement->positionControlLineaire.accelerationMaxAv.getMaxValue();
    max_acceleration_back = robotAsservisement->positionControlLineaire.accelerationMaxAr.getMaxValue();
    max_deceleration_for = robotAsservisement->positionControlLineaire.decelerationMaxAv.getMaxValue();
    max_deceleration_back = robotAsservisement->positionControlLineaire.decelerationMaxAr.getMaxValue();
}
void i2c_interface::get_angular_position_control(int16_t &max_speed_clock, int16_t &max_speed_anti, int16_t &max_acceleration_clock, int16_t &max_acceleration_anti, int16_t &max_deceleration_clock, int16_t &max_deceleration_anti){
    max_speed_clock = robotAsservisement->positionControlAngulaire.vitesseMaxAv.getMaxValue();
    max_speed_anti = robotAsservisement->positionControlAngulaire.vitesseMaxAr.getMaxValue();
    max_acceleration_clock = robotAsservisement->positionControlAngulaire.accelerationMaxAv.getMaxValue();
    max_acceleration_anti = robotAsservisement->positionControlAngulaire.accelerationMaxAr.getMaxValue();
    max_deceleration_clock = robotAsservisement->positionControlAngulaire.decelerationMaxAv.getMaxValue();
    max_deceleration_anti = robotAsservisement->positionControlAngulaire.decelerationMaxAr.getMaxValue();
}

void i2c_interface::get_pid_linear_static(int16_t &p, int16_t &i, int16_t &d){
    robotAsservisement->pidLineaireBlock.setPID(p,i,d);
}

void i2c_interface::get_pid_linear_dynamic(int16_t &p, int16_t &i, int16_t &d){
    robotAsservisement->pidLineaire.setPID(p,i,d);
}

void i2c_interface::get_pid_angular_static(int16_t &p, int16_t &i, int16_t &d){
    robotAsservisement->pidAngulaireBlock.setPID(p,i,d);
}

void i2c_interface::get_pid_angular_dynamic(int16_t &p, int16_t &i, int16_t &d){
    robotAsservisement->pidAngulaire.setPID(p,i,d);
}

void i2c_interface::get_odometry_metric(int16_t &sizeWheelLeft, int16_t &sizeWheelRigth, int16_t &spaceInterWheel){
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

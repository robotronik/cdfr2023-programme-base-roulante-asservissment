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

void i2c_interface::get_braking_distance(int16_t &distance){
    distance = (int16_t)robotAsservisement->getBrakingDistance();
}

void i2c_interface::get_commande_buffer_size(int16_t &size){
    // TODO
}

void i2c_interface::get_direction_side(Direction &direction_side){
    direction_side = robotAsservisement->getDirectionSide();
}

void i2c_interface::get_rotation_side(Rotation &rotation_side){
    rotation_side = robotAsservisement->getRotationSide();
}

void i2c_interface::get_current_target(int16_t &x, int16_t &y, int16_t &theta){
    position_t consigne = robotAsservisement->getCurrentConsigne();
    x = consigne.x;
    y = consigne.y;
    theta = consigne.teta;
}

void i2c_interface::get_moving_is_done(bool &state){
    state = robotAsservisement->robotMovingIsFinish();
}

void i2c_interface::get_running_is_done(bool &state){
    state = robotAsservisement->robotRunningIsFinish();
}

void i2c_interface::get_turning_is_done(bool &state){
    state = robotAsservisement->robotTurningIsFinish();
}

void i2c_interface::get_linear_error(int16_t &error){
    error = (int16_t)robotAsservisement->getLinearError();
}

void i2c_interface::get_angular_error(int16_t &error){
    error = (int16_t)robotAsservisement->getAngularError();
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
        robotAsservisement->reset();
        motorSpeedSignedL(0);
        motorSpeedSignedR(0);
        motorBrakeL(false);
        motorBrakeR(false);
    }
    else{
        motorBrakeL(true);
        motorBrakeR(true);
    }
}

void i2c_interface::set_max_torque(int16_t max_torque){
    setMaxTorqueL(max_torque);
    setMaxTorqueR(max_torque);
}


void i2c_interface::set_max_speed_forward(int16_t speed){
    robotAsservisement->positionControlLineaire.vitesseMaxAv = speed;
}

void i2c_interface::set_max_speed_backward(int16_t speed){
    robotAsservisement->positionControlLineaire.vitesseMaxAr = speed;
}

void i2c_interface::set_max_speed_trigo(int16_t speed){
    robotAsservisement->positionControlAngulaire.vitesseMaxAv = speed;
}

void i2c_interface::set_max_speed_horloge(int16_t speed){
    robotAsservisement->positionControlAngulaire.vitesseMaxAr = speed;
}

void i2c_interface::set_pid_linear_static(int16_t p, int16_t i, int16_t d){

}

void i2c_interface::set_pid_linear_dynamic(int16_t p, int16_t i, int16_t d){

}

void i2c_interface::set_pid_angular_static(int16_t p, int16_t i, int16_t d){

}

void i2c_interface::set_pid_angular_dynamic(int16_t p, int16_t i, int16_t d){

}

void i2c_interface::set_odometry_metric(int16_t sizeWheelLeft, int16_t sizeWheelRigth, int16_t spaceInterWheel){

}

void i2c_interface::get_max_torque(int16_t max_torque){
    // TODO
}

void i2c_interface::get_max_speed_forward(int16_t speed){
    // TODO
}

void i2c_interface::get_max_speed_backward(int16_t speed){
    // TODO
}

void i2c_interface::get_max_speed_trigo(int16_t speed){
    // TODO
}

void i2c_interface::get_max_speed_horloge(int16_t speed){
    // TODO
}

void i2c_interface::get_pid_linear_static(int16_t &p, int16_t &i, int16_t &d){
    // TODO
}

void i2c_interface::get_pid_linear_dynamic(int16_t &p, int16_t &i, int16_t &d){
    // TODO
}

void i2c_interface::get_pid_angular_static(int16_t &p, int16_t &i, int16_t &d){
    // TODO
}

void i2c_interface::get_pid_angular_dynamic(int16_t &p, int16_t &i, int16_t &d){
    // TODO
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

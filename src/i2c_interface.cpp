#include "i2c_interface.h"

i2c_interface::i2c_interface(position* inRobotPosition, movement* inRobotAsservisement):
robotPosition(inRobotPosition),
robotAsservisement(inRobotAsservisement){

}

void i2c_interface::setReponseBuffer(uint8_t* data, int size){
    I2CSetBuffer(data,size);
}


void i2c_interface::turn_on_LED_1(){
    gpio_set(port_led1,pin_led1);
}

void i2c_interface::turn_off_LED_1(){
    gpio_clear(port_led1,pin_led1);
}

void i2c_interface::turn_on_LED_2(){
    gpio_set(port_led2,pin_led2);
}

void i2c_interface::turn_off_LED_2(){
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

void i2c_interface::set_consigne_lineaire(int16_t x, int16_t y){
    robotAsservisement->goToPoint(x, y);
}

void i2c_interface::set_consigne_angulaire(int16_t angle, int16_t rotation){
    robotAsservisement->setConsigneAngulaire(angle,(Rotation)rotation);
}

void i2c_interface::set_consigne_lookAt_forward(int16_t x, int16_t y, int16_t rotation){
    robotAsservisement->setConsigneLookAtForward(x ,y ,(Rotation)rotation);
}

void i2c_interface::set_consigne_lookAt_backward(int16_t x, int16_t y, int16_t rotation){
    robotAsservisement->setConsigneLookAtBackward(x ,y ,(Rotation)rotation);
}

void i2c_interface::robot_moving_is_finish(int16_t &resbool){
    resbool = robotAsservisement->robotMovingIsFinish();
}

void i2c_interface::robot_running_is_finish(int16_t &resbool){
    resbool = robotAsservisement->robotRunningIsFinish();
}

void i2c_interface::robot_turning_is_finish(int16_t &resbool){
    resbool = robotAsservisement->robotTurningIsFinish();
}

void i2c_interface::get_linear_error(int16_t &error){
    error = (int16_t)robotAsservisement->getLinearError();
}

void i2c_interface::get_angular_error(int16_t &error){
    error = (int16_t)robotAsservisement->getAngularError();
}

void i2c_interface::get_braking_distance(int16_t &distance){
    distance = (int16_t)robotAsservisement->getBrakingDistance();
}

void i2c_interface::get_robot_running(bool &robot_runnning){
    robot_runnning = robotAsservisement->commandRun();
}

void i2c_interface::get_directio_side(Direction &direction_side){
    direction_side = robotAsservisement->getDirectionSide();
}

void i2c_interface::get_rotation_side(Rotation &rotation_side){
    rotation_side = robotAsservisement->getRotationSide();
}

void i2c_interface::get_current_consigne(int16_t &x, int16_t &y, int16_t &theta){
    position_t consigne = robotAsservisement->getCurrentConsigne();
    x = consigne.x;
    y = consigne.y;
    theta = consigne.teta;
}

void i2c_interface::disable_motor(){
    disableMotor();
}

void i2c_interface::enable_motor(){
    robotAsservisement->reset();
    motorSpeedSignedL(0);
    motorSpeedSignedR(0);
    enableMotor();
}

void i2c_interface::brake_on(){
    motorBrakeL(true);
	motorBrakeR(true);
}

void i2c_interface::brake_off(){
    robotAsservisement->reset();
    motorSpeedSignedL(0);
    motorSpeedSignedR(0);
    motorBrakeL(false);
	motorBrakeR(false);
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

void i2c_interface::go_to_point(int16_t x,int16_t y,Rotation rotation, Direction direction){
    robotAsservisement->goToPoint(x, y, rotation, direction);
}

void i2c_interface::go_to_point(int16_t x,int16_t y,int16_t theta, Rotation rotationFirst, Direction direction, Rotation rotationSecond){
    robotAsservisement->goToPoint(x, y, theta, rotationFirst, direction, rotationSecond);
}

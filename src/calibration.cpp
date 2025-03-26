#include "calibration.h"

FsmCalibration fsmCalibration;
uint32_t startTime;

bool loopCalibration(i2c_interface* robot){
    switch (fsmCalibration)
    {
    case WAIT_START:
        startTime = get_uptime_ms()+2000;
        fsmCalibration = START;
        break;

    case START:
        if(startTime < get_uptime_ms()){
            fsmCalibration = INIT;
        }
        break;
    case INIT:
        robot->set_coordinates(0,0,0);
        robot->set_motor_state(true);
        startCalibration();
        fsmCalibration = STEP1;
        break;

    case STEP1:
        robot->go_to_point(250,0);
        robot->go_to_point(250,-300,Rotation::ANTICLOCKWISE,Direction::BACKWARD);
        fsmCalibration = WAIT_END_STEP1;
        break;
    case WAIT_END_STEP1:
        if(robot->get_command_buffer_size()==0){
            startTime = get_uptime_ms()+3000;
            fsmCalibration = WAIT_TIME_STEP1;
        }
        break;
    case WAIT_TIME_STEP1:
        if(startTime < get_uptime_ms()){
            fsmCalibration = STEP2;
        }
        break;

    case STEP2:
        sectionCalibration();
        robot->go_to_point(250,1000);
        robot->go_to_point(-50,1000,Rotation::ANTICLOCKWISE,Direction::BACKWARD);
        fsmCalibration = WAIT_END_STEP2;
        break;
    case WAIT_END_STEP2:
        if(robot->get_command_buffer_size()==0){
            startTime = get_uptime_ms()+3000;
            fsmCalibration = WAIT_TIME_STEP2;
        }
        break;
    case WAIT_TIME_STEP2:
        if(startTime < get_uptime_ms()){
            fsmCalibration = STEP3;
        }
        break;

    case STEP3:
        sectionCalibration();
        robot->go_to_point(250,1000);
        robot->go_to_point(250,0,Rotation::CLOCKWISE);
        robot->go_to_point(-50,0,Rotation::ANTICLOCKWISE,Direction::BACKWARD);
        fsmCalibration = WAIT_END_STEP3;
        break;
    case WAIT_END_STEP3:
        if(robot->get_command_buffer_size()==0){
            startTime = get_uptime_ms()+3000;
            fsmCalibration = WAIT_TIME_STEP3;
        }
        break;
    case WAIT_TIME_STEP3:
        if(startTime < get_uptime_ms()){
            fsmCalibration = STEP4;
        }
        break;

    case STEP4:
        sectionCalibration();
        robot->go_to_point(250,0);
        robot->go_to_point(250,-300,Rotation::ANTICLOCKWISE,Direction::BACKWARD);
        fsmCalibration = WAIT_END_STEP4;
        break;
    case WAIT_END_STEP4:
        if(robot->get_command_buffer_size()==0){
            startTime = get_uptime_ms()+3000;
            fsmCalibration = WAIT_TIME_STEP4;
        }
        break;
    case WAIT_TIME_STEP4:
        if(startTime < get_uptime_ms()){
            fsmCalibration = FINISH;
        }
        break;


    case FINISH:
        stopCalibration();
        robot->set_motor_state(false);
        fsmCalibration = COMPUTE;
        break;

    case COMPUTE:
        computeCalibration();
        fsmCalibration = END;
        break;
    default:
        break;
    }
}
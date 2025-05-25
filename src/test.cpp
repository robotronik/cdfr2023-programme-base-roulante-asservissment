#include "test.h"
int linearCalibration(i2c_interface* robotI2cInterface){
    SEQUENCE_BEGIN();

    robotI2cInterface->set_coordinates(0,0,0);
    robotI2cInterface->set_motor_state(true);
	robotI2cInterface->go_to_point(-200,0,Rotation::SHORTEST,Direction::SHORTEST);

    DELAY(3000);

	robotI2cInterface->go_to_point(0,0,Rotation::SHORTEST,Direction::SHORTEST);

    DELAY(3000);

	robotI2cInterface->go_to_point(-600,0,Rotation::SHORTEST,Direction::SHORTEST);

    DELAY(3000);

	robotI2cInterface->go_to_point(0,0,Rotation::SHORTEST,Direction::SHORTEST);

    DELAY(3000);

	robotI2cInterface->go_to_point(-1000,0,Rotation::SHORTEST,Direction::SHORTEST);

    DELAY(5000);

	robotI2cInterface->go_to_point(0,0,Rotation::SHORTEST,Direction::SHORTEST);

    SEQUENCE_END();
    return EXIT_SUCCESS;
}


int angularCalibration(i2c_interface* robotI2cInterface){
    SEQUENCE_BEGIN();

    robotI2cInterface->set_coordinates(0,0,0);
    robotI2cInterface->set_motor_state(true);
	robotI2cInterface->consigne_angulaire(45,Rotation::ANTICLOCKWISE);

    DELAY(3000);

	robotI2cInterface->consigne_angulaire(-45,Rotation::CLOCKWISE);

    DELAY(3000);

	robotI2cInterface->consigne_angulaire(135,Rotation::ANTICLOCKWISE);

    DELAY(3000);

	robotI2cInterface->consigne_angulaire(-135,Rotation::CLOCKWISE);

    DELAY(3000);

	robotI2cInterface->consigne_angulaire(0,Rotation::ANTICLOCKWISE);

    SEQUENCE_END();
    return EXIT_SUCCESS;
}


int testloop(i2c_interface* robotI2cInterface){
    SEQUENCE_BEGIN();

    robotI2cInterface->set_coordinates(0,0,0);
    robotI2cInterface->set_motor_state(true);
	robotI2cInterface->consigne_angulaire(90,Rotation::SHORTEST);

    DELAY(3000);

    robotI2cInterface->consigne_angulaire(0,Rotation::SHORTEST);

    DELAY(3000);

	robotI2cInterface->go_to_point(500,500);

    DELAY(7000);

	robotI2cInterface->go_to_point(0,0,0);

    DELAY(7000);

	robotI2cInterface->go_to_point(500,500,Rotation::ANTICLOCKWISE,Direction::BACKWARD);

    DELAY(7000);

	robotI2cInterface->go_to_point(0,0,0,Rotation::CLOCKWISE,Direction::BACKWARD,Rotation::ANTICLOCKWISE);

    DELAY(1);

    SEQUENCE_END();
    return EXIT_SUCCESS;
}


int rotation10(i2c_interface* robotI2cInterface){
    SEQUENCE_BEGIN();

    robotI2cInterface->set_coordinates(0,0,0);
    robotI2cInterface->set_motor_state(true);

    for(int i = 0; i<10; i++){
        robotI2cInterface->consigne_angulaire(180,Rotation::ANTICLOCKWISE);
        robotI2cInterface->consigne_angulaire(0,Rotation::ANTICLOCKWISE);
    }

    DELAY(1);

    SEQUENCE_END();
    return EXIT_SUCCESS;
}


int longLine(i2c_interface* robotI2cInterface){
    SEQUENCE_BEGIN();

    robotI2cInterface->set_coordinates(0,0,0);
    robotI2cInterface->set_motor_state(true);

    robotI2cInterface->go_to_point(-2500,0,Rotation::SHORTEST,Direction::BACKWARD);

    DELAY(1);

    SEQUENCE_END();
    return EXIT_SUCCESS;
}

int angleTest(i2c_interface* robotI2cInterface){
    SEQUENCE_BEGIN();

    robotI2cInterface->set_coordinates(0,0,0);
    robotI2cInterface->set_motor_state(true);

    //robotI2cInterface->go_to_point(500,0);
    //robotI2cInterface->set_linear_max_speed(100,600,600);
    robotI2cInterface->go_to_point(500,	0);
    robotI2cInterface->go_to_point(543,	2);
    robotI2cInterface->go_to_point(586,	8);
    robotI2cInterface->go_to_point(629,	18);
    robotI2cInterface->go_to_point(671,	31);
    robotI2cInterface->go_to_point(711,	47);
    robotI2cInterface->go_to_point(750,	67);
    robotI2cInterface->go_to_point(786,	91);
    robotI2cInterface->go_to_point(821,	117);
    robotI2cInterface->go_to_point(853,	147);
    robotI2cInterface->go_to_point(883,	179);
    robotI2cInterface->go_to_point(909,	214);
    robotI2cInterface->go_to_point(933,	250);
    robotI2cInterface->go_to_point(953,	289);
    robotI2cInterface->go_to_point(969,	329);
    robotI2cInterface->go_to_point(982,	371);
    robotI2cInterface->go_to_point(992,	414);
    robotI2cInterface->go_to_point(998,	457);
    robotI2cInterface->go_to_point(1000,	500);

    //robotI2cInterface->go_to_point(750,40);
    //robotI2cInterface->go_to_point(1000,80);


    DELAY(10000);

    robotI2cInterface->set_linear_max_speed(700,600,600);
	robotI2cInterface->go_to_point(0,0,0,Rotation::SHORTEST,Direction::SHORTEST);

    DELAY(1);

    SEQUENCE_END();
    return EXIT_SUCCESS;
}
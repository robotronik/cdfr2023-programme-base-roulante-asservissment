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

    robotI2cInterface->set_coordinates(-200,0,180);
    robotI2cInterface->set_motor_state(true);
	robotI2cInterface->consigne_angulaire(90,Rotation::SHORTEST);

    DELAY(3000);

    robotI2cInterface->consigne_angulaire(0,Rotation::SHORTEST);

    DELAY(3000);

	robotI2cInterface->go_to_point(400,400);

    //DELAY(2000);
	//robotI2cInterface->stop();

    DELAY(7000);

	robotI2cInterface->go_to_point(0,0,0);

    DELAY(7000);

	robotI2cInterface->go_to_point(400,400,Rotation::ANTICLOCKWISE,Direction::BACKWARD);

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
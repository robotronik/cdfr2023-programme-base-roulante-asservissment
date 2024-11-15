#include "config.h"
#include "motor.h"
#include "uart.h"
#include "odometrie.h"
#include "clock.h"
#include "I2C.h"
#include "led.h"
#include "sequence.h"
#include "robot.h"
#include "Asservissement.h"


//#define TESTROBOT
//#define TESTMOTOR

#ifdef SIMULATION
	#include "hardware_interface.h"
	#define main stm_main
#endif

bool benableMotorDebug = true;
position_t newPostion;
bool overridePos = false;
int maxTorque = 100;

robot* robotCDFR = new robot();
Asservissement* robotAsservisement = new Asservissement(robotCDFR);

enum CommandNum {
    // LED Commands
    CMD_TURN_ON_LED_1 = 10,
    CMD_TURN_OFF_LED_1 = 11,
    CMD_TURN_ON_LED_2 = 12,
    CMD_TURN_OFF_LED_2 = 13,

    // Coordinate Commands
    CMD_GET_COORDINATES = 20,
    CMD_SET_COORDINATES = 21,

    // Motion Commands
    CMD_STOP = 30,
    CMD_SET_CONSIGNE_LINEAIRE = 31,
    CMD_SET_CONSIGNE_ANGULAIRE = 32,
    CMD_SET_CONSIGNE_LOOKAT_FORWARD = 33,
    CMD_SET_CONSIGNE_LOOKAT_BACKWARD = 34,

    // Robot Status Commands
    CMD_ROBOT_MOVING_IS_FINISH = 40,
    CMD_ROBOT_RUNNING_IS_FINISH = 41,
    CMD_ROBOT_TURNING_IS_FINISH = 42,
    CMD_GET_LINEAR_ERROR = 43,
    CMD_GET_ANGULAR_ERROR = 44,
    CMD_GET_BRAKING_DISTANCE = 45,

    // Motor Commands
    CMD_DISABLE_MOTOR = 50,
    CMD_ENABLE_MOTOR = 51,
	CMD_MOTOR_52 = 52, //TODO : Docs doesnt talk about these !!
	CMD_MOTOR_53 = 53,
	CMD_MOTOR_TORQUE_54 = 54,

    // Speed Commands
    CMD_SET_MAX_SPEED_FORWARD = 60,
    CMD_SET_MAX_SPEED_BACKWARD = 61,
    CMD_SET_MAX_SPEED_TRIGO = 62,
    CMD_SET_MAX_SPEED_HORLOGE = 63
};

void I2CRecieveData(uint8_t* data, int size){
	CommandNum command = (CommandNum)data[0];
	switch (command)
	{
	case CMD_TURN_ON_LED_1:
		gpio_set(port_led1,pin_led1);
		break;
	case CMD_TURN_OFF_LED_1:
		gpio_clear(port_led1,pin_led1);
		break;
	case CMD_TURN_ON_LED_2:
		gpio_set(port_led2,pin_led2);
		break;
	case CMD_TURN_OFF_LED_2:
		gpio_clear(port_led2,pin_led2);
		break;

	case CMD_GET_COORDINATES: {
        position_u posi;
        posi.position.x = robotCDFR->getPosition_X();
        posi.position.y = robotCDFR->getPosition_Y();
        posi.position.theta = robotCDFR->getPosition_theta();
        I2CSetBuffer(posi.tab, 6);
        break;
    }
    case CMD_SET_COORDINATES:
        if (size == 7) {
            newPostion.x = (double)((int16_t)(data[1] << 8 | data[2]));
            newPostion.y = (double)((int16_t)(data[3] << 8 | data[4]));
            newPostion.theta = (double)((int16_t)(data[5] << 8 | data[6]));
            overridePos = true;
        }
        break;

    case CMD_STOP:
        robotAsservisement->setConsigneStop();
        break;
    case CMD_SET_CONSIGNE_LINEAIRE:
        if (size == 5) {
            uintConv x, y;
            x.tab[1] = data[1]; x.tab[0] = data[2];
            y.tab[1] = data[3]; y.tab[0] = data[4];
            robotAsservisement->setProtectedConsigneLineaire((double)x.num, (double)y.num);
        }
        break;
    case CMD_SET_CONSIGNE_ANGULAIRE:
        if (size == 5) {
            uintConv theta, rotation;
            theta.tab[1] = data[1]; theta.tab[0] = data[2];
            rotation.tab[1] = data[3]; rotation.tab[0] = data[4];
            robotAsservisement->setProtectedConsigneAngulaire((double)theta.num, (sensRotation_t)rotation.num);
        }
        break;

	case CMD_SET_CONSIGNE_LOOKAT_FORWARD:
		if (size == 7){
			uintConv x,y,rotation;
			x.tab[1] = data[1]; x.tab[0] = data[2];
			y.tab[1] = data[3]; y.tab[0] = data[4];
			rotation.tab[0] = data[5]; rotation.tab[1] = data[6];
			robotAsservisement->setConsigneLookAtForward((double)x.num,(double)y.num,(sensRotation_t)rotation.num);
		}
		break;
	case CMD_SET_CONSIGNE_LOOKAT_BACKWARD:
		if (size == 7){
			uintConv x,y,rotation;
			x.tab[1] = data[1]; x.tab[0] = data[2];
			y.tab[1] = data[3]; y.tab[0] = data[4];
			rotation.tab[0] = data[5]; rotation.tab[1] = data[6];
			robotAsservisement->setConsigneLookAtBackward((double)x.num,(double)y.num,(sensRotation_t)rotation.num);
		}
		break;
	case CMD_ROBOT_MOVING_IS_FINISH:
    case CMD_ROBOT_RUNNING_IS_FINISH:
    case CMD_ROBOT_TURNING_IS_FINISH:
    case CMD_GET_LINEAR_ERROR:
    case CMD_GET_ANGULAR_ERROR:
    case CMD_GET_BRAKING_DISTANCE: {
        uintConv result;
        switch ((CommandNum)data[0]) {
            case CMD_ROBOT_MOVING_IS_FINISH:
                result.num = robotAsservisement->robotMovingIsFinish();
                break;
            case CMD_ROBOT_RUNNING_IS_FINISH:
                result.num = robotAsservisement->robotRunningIsFinish();
                break;
            case CMD_ROBOT_TURNING_IS_FINISH:
                result.num = robotAsservisement->robotTurningIsFinish();
                break;
            case CMD_GET_LINEAR_ERROR:
                result.num = robotAsservisement->getLinearError();
                break;
            case CMD_GET_ANGULAR_ERROR:
                result.num = robotAsservisement->getAngularError();
                break;
            case CMD_GET_BRAKING_DISTANCE:
                result.num = robotAsservisement->getBrakingDistance();
                break;
			default: break;
        }
        I2CSetBuffer(result.tab, 2);
        break;
    }
    case CMD_DISABLE_MOTOR:
        disableMotor();
        benableMotorDebug = false;
        break;
    case CMD_ENABLE_MOTOR:
        robotAsservisement->reset();
        motorSpeedSignedL(0);
        motorSpeedSignedR(0);
        enableMotor();
        benableMotorDebug = true;
        break;
	case CMD_MOTOR_52:
		motorBrakeL(true);
		motorBrakeR(true);
		break;
	case CMD_MOTOR_53:
		motorBrakeL(false);
		motorBrakeR(false);
		break;
	case CMD_MOTOR_TORQUE_54:
		uintConv max;
		max.tab[1] = data[1]; max.tab[0] = data[2];
		maxTorque = max.num;
		break;

    case CMD_SET_MAX_SPEED_FORWARD:
    case CMD_SET_MAX_SPEED_BACKWARD:
    case CMD_SET_MAX_SPEED_TRIGO:
    case CMD_SET_MAX_SPEED_HORLOGE: {
        uintConv speed;
        speed.tab[1] = data[1];
        speed.tab[0] = data[2];
        switch ((CommandNum)data[0]) {
            case CMD_SET_MAX_SPEED_FORWARD:
                robotAsservisement->positionControlLineaire.vitesseMaxAv = speed.num;
                break;
            case CMD_SET_MAX_SPEED_BACKWARD:
                robotAsservisement->positionControlLineaire.vitesseMaxAr = speed.num;
                break;
            case CMD_SET_MAX_SPEED_TRIGO:
                robotAsservisement->positionControlAngulaire.vitesseMaxAv = speed.num;
                break;
            case CMD_SET_MAX_SPEED_HORLOGE:
                robotAsservisement->positionControlAngulaire.vitesseMaxAr = speed.num;
                break;
			default: break;
        }
        break;
    }
	default:
		break;
	}
}

void testloop(sequence* seq){
	
	seq->start();

	seq->delay([](){
		robotAsservisement->setConsigneAngulaire(90,ROTATION_DIRECT);
	},0);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneAngulaire(-90,ROTATION_TRIGO);
	// },3000);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneAngulaire(90,ROTATION_HORRAIRE);
	// },3000);

	seq->delay([](){
	robotAsservisement->setConsigneAngulaire(0,ROTATION_DIRECT);
	},3000);

	seq->delay([](){
		robotAsservisement->setConsigneLineaire(1000,0);
	},3000);

	seq->delay([](){
		robotAsservisement->setConsigneLineaire(0,0);
	},7000);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneAngulaire(0,ROTATION_DIRECT);
	// },7000);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneLineaire(1000,0);
	// },10000);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneStop();
	// },1500);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneLineaire(0,0);
	// },10000);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneAngulaire(90,ROTATION_TRIGO);
	// },7000);

	// seq->delay([](){
	// 	robotAsservisement->setConsigneAngulaire(0,ROTATION_HORRAIRE);
	// },7000);
}

int main(void)
{

	//SETUP
	clock_setup();
	ledSetup();
	motorSetup();
	usartSetup();
	odometrieSetup();
	i2c_setup();	
	setCallbackReceive(I2CRecieveData);
	


	//WAIT
	delay_ms(3000);
	usartprintf("Start\n");


	//UNBRAKE MOTOR
	motorBrakeR(0);
	motorBrakeL(0);
	motorSetModeR(0);
	motorSetModeL(0);
	disableMotor();


//
// Test motor
// Accelerate Forward -> Decelerate Forward -> Accelerate backward -> Decelerate backward 
//	
#ifdef TESTMOTOR
		enableMotor();
		for (int i = 0; i < 100; i++){
			usartprintf("%d\n",i);
			motorSpeedSignedL(i);
			motorSpeedSignedR(i);
			delay_ms(100);
			usartprintf("Right : %d %d\n",gpio_get(port_info1R,port_info1R),gpio_get(port_info2R,port_info2R));
			usartprintf("Left : %d %d\n\n",gpio_get(port_info1R,port_info1L),gpio_get(port_info2R,port_info2L));
		}
		for (int i = 100; i > -100; i--){
			usartprintf("%d\n",i);
			motorSpeedSignedL(i);
			motorSpeedSignedR(i);
			delay_ms(100);
			usartprintf("Right : %d %d\n",gpio_get(port_info1R,port_info1R),gpio_get(port_info2R,port_info2R));
			usartprintf("Left : %d %d\n\n",gpio_get(port_info1R,port_info1L),gpio_get(port_info2R,port_info2L));
		}

		for (int i = -100; i < 0; i++){
			usartprintf("%d\n",i);
			motorSpeedSignedL(i);
			motorSpeedSignedR(i);
			delay_ms(100);
			usartprintf("Right : %d %d\n",gpio_get(port_info1R,port_info1R),gpio_get(port_info2R,port_info2R));
			usartprintf("Left : %d %d\n\n",gpio_get(port_info1R,port_info1L),gpio_get(port_info2R,port_info2L));
		}
		while (1);		
#endif
	
#ifdef TESTROBOT
//
// Test move on the robot
// Forwarde backwadr and rotation
//	
	sequence mySeq;
	sequence ledToggleSeq;
	robotAsservisement->reset();
	enableMotor();
	while (1){
		//delay_ms(50);
		odometrieLoop(robotCDFR);
		position_t robotPosition = robotCDFR->getPosition();
		usartprintf(">x:%lf\n",robotPosition.x);
		usartprintf(">y:%lf\n",robotPosition.y);
		usartprintf(">theta:%lf\n",robotPosition.theta);
		motorSpeed_t speed = robotAsservisement->asservissementLoop();
		motorSpeedSignedL(speed.L);
		motorSpeedSignedR(speed.R);

		testloop(&mySeq);


		//BLINK LED
		ledToggleSeq.interval([](){
			led1_toggle();
		},1000);
	}


#else

//
//	Main Loop off the robot
//	
	sequence ledToggleSeq;
	robotAsservisement->reset();
	uint32_t nextTime =  get_uptime_ms();

	int LmotorSpeed, RmotorSpeed;

	while (1){
		
		odometrieLoop(robotCDFR);
		if (overridePos) {
			overridePos = false;
			robotCDFR->setPosition(newPostion);
			robotAsservisement->setConsigne(newPostion);
		}
		if (get_uptime_ms() > nextTime) {
			nextTime = get_uptime_ms() + 50; //Loop time in ms
			if (robotAsservisement->asservissementLoop(&LmotorSpeed, &RmotorSpeed)) {
				//usartprintf(">x : %.3lf\n>y : %.3lf\n>theta : %.3lf\n",robotCDFR->getPosition_X(),robotCDFR->getPosition_Y(),robotCDFR->getPosition_theta());
				if (LmotorSpeed > maxTorque)
					LmotorSpeed = maxTorque;
				if (LmotorSpeed < -maxTorque)
					LmotorSpeed = -maxTorque;
				if (RmotorSpeed > maxTorque)
					RmotorSpeed = maxTorque;
				if (RmotorSpeed < -maxTorque)
					RmotorSpeed = -maxTorque;
				//usartprintf("%d %d\n",LmotorSpeed,RmotorSpeed);
				motorSpeedSignedL(LmotorSpeed);
				motorSpeedSignedR(RmotorSpeed);
			}
		}

		//BLINK LED
		ledToggleSeq.interval([](){
			led1_toggle();
		},1000);
	}
#endif
	return 0;
}
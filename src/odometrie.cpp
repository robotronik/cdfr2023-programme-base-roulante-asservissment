#include "odometrie.h"


uint8_t buffer[_BUFFERSIZE];
int size = _BUFFERSIZE;
CircularBufferOdo* circularBufferOdo;



void odometrieSetup(void){
	circularBufferOdo = new CircularBufferOdo(size,buffer);


    rcc_periph_clock_enable(RCC_GPIOD);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_SYSCFG);

    /* Enable EXTI0 interrupt. */
	nvic_enable_irq(NVIC_EXTI2_IRQ);
	nvic_set_priority(NVIC_EXTI2_IRQ, 0);

	/* Set GPIO0 (in GPIO port A) to 'input open-drain'. */
	gpio_mode_setup(port_odometrie1R, GPIO_MODE_INPUT, GPIO_MODE_INPUT , pin_odometrie1R);
	gpio_mode_setup(port_odometrie2R, GPIO_MODE_INPUT, GPIO_MODE_INPUT , pin_odometrie2R);

	/* Configure the EXTI subsystem. */
	exti_select_source(EXTI2, port_odometrie1R);
	exti_set_trigger(EXTI2, EXTI_TRIGGER_RISING);
	exti_enable_request(EXTI2);



    /* Enable EXTI0 interrupt. */
	nvic_enable_irq(NVIC_EXTI4_IRQ);
	nvic_set_priority(NVIC_EXTI4_IRQ, 1);

	/* Set GPIO0 (in GPIO port A) to 'input open-drain'. */
	gpio_mode_setup(port_odometrie1L, GPIO_MODE_INPUT, GPIO_MODE_INPUT , pin_odometrie1L);
	gpio_mode_setup(port_odometrie2L, GPIO_MODE_INPUT, GPIO_MODE_INPUT , pin_odometrie2L);

	/* Configure the EXTI subsystem. */
	exti_select_source(EXTI4, port_odometrie1L);
	exti_set_trigger(EXTI4, EXTI_TRIGGER_RISING);
	exti_enable_request(EXTI4);
}

void exti2_isr(void)
{
    exti_reset_request(EXTI2);
	gpio_toggle(port_led1,pin_led1);
	if(gpio_get (port_odometrie2R,pin_odometrie2R)){
		circularBufferOdo->push(backwardR);
	}
	else{
		//Vers l'avant
		circularBufferOdo->push(fordwardR);
	}
}

void exti4_isr(void)
{
	exti_reset_request(EXTI4);
    gpio_toggle(port_led1,pin_led1);
	if(gpio_get (port_odometrie2L,pin_odometrie2L)){
		//Vers l'avant
		circularBufferOdo->push(fordwardL);
	}
	else{
		circularBufferOdo->push(backwardL);
	}
}


void odometrieLoop(position_t &position){
	while (!circularBufferOdo->isEmpty()){
		switch (circularBufferOdo->pop())
		{
		case fordwardL:
				position.y -= STEPAVANCEG * cos(COEFCONVDEGRETORADIAN*(position.teta+90)); //Voir pour optimisation
				position.x -= STEPAVANCEG * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.teta -= STEPANGLEG;
			break;
		case backwardL:
				position.y += STEPAVANCEG * cos(COEFCONVDEGRETORADIAN*(position.teta+90)); //Voir pour optimisation
				position.x += STEPAVANCEG * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.teta += STEPANGLEG;
			break;
		case fordwardR:
				position.y -= STEPAVANCED * cos(COEFCONVDEGRETORADIAN*(position.teta+90)); //Voir pour optimisation
				position.x -= STEPAVANCED * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.teta += STEPANGLED;
			break;
		case backwardR:
				position.y += STEPAVANCED * cos(COEFCONVDEGRETORADIAN*(position.teta+90)); //Voir pour optimisation
				position.x += STEPAVANCED * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.teta -= STEPANGLED;
			break;
		default:
			break;
		}
	}
}

void odometrieCalc(position_t &position, odometrieTrigger_t typeTrigger, odometrieParam_t param){
    switch (typeTrigger)
    {
    case fordwardL:
            position.y -= param.stepForrwardG * cos(COEFCONVDEGRETORADIAN*(position.teta+90)); //Voir pour optimisation
            position.x -= param.stepForrwardG * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
            position.teta -= param.stepAngleG;
        break;
    case backwardL:
            position.y += param.stepForrwardG * cos(COEFCONVDEGRETORADIAN*(position.teta+90)); //Voir pour optimisation
            position.x += param.stepForrwardG * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
            position.teta += param.stepAngleG;
        break;
    case fordwardR:
            position.y -= param.stepForrwardD * cos(COEFCONVDEGRETORADIAN*(position.teta+90)); //Voir pour optimisation
            position.x -= param.stepForrwardD * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
            position.teta += param.stepAngleD;
        break;
    case backwardR:
            position.y += param.stepForrwardD * cos(COEFCONVDEGRETORADIAN*(position.teta+90)); //Voir pour optimisation
            position.x += param.stepForrwardD * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
            position.teta -= param.stepAngleD;
        break;
    default:
        break;
    }
}


void startCalibration(void){
	circularBufferOdo->startRecording();
}

void sectionCalibration(void){
	circularBufferOdo->addSection();
}

void stopCalibration(void){
	circularBufferOdo->stopRecording();
}

bool computeCalibration(void){
    if(!circularBufferOdo->recordIsValid()){
        usartprintf("Buffer not valid : Overflow\n");
        usartprintf("Create a smaller path to calibrate\n");
        return false;
    }
    odometrieParam_t param;
    position_t positionSection[NB_RECORD_SECTION];

    double bestWheelG;
    double bestWheelD;
    double bestdistance;

    double best = 100000000;
    double Compromis = 1;
    int totalIteration = 0;

    double minWheelG = CALIBRATION_START_DIAMETERWHEEL - CALIBRATION_PLAGE_DIAMETERWHEEL;
    double maxWheelG = CALIBRATION_START_DIAMETERWHEEL + CALIBRATION_PLAGE_DIAMETERWHEEL;
    double minWheelD = CALIBRATION_START_DIAMETERWHEEL - CALIBRATION_PLAGE_DIAMETERWHEEL;
    double maxWheelD = CALIBRATION_START_DIAMETERWHEEL + CALIBRATION_PLAGE_DIAMETERWHEEL;
    double minDistance = CALIBRATION_START_DISTANCEWHEEL - CALIBRATION_PLAGE_DISTANCEWHEEL;
    double maxDistance = CALIBRATION_START_DISTANCEWHEEL + CALIBRATION_PLAGE_DISTANCEWHEEL;

    for(int i = 0; i < circularBufferOdo->getNumberSetion();i++){
        usartprintf("Section %d : start %ld, end %ld\n",i,circularBufferOdo->getSartPointSection(i),circularBufferOdo->getEndPointSection(i));
    }

    param.stepAngleD = COMPUTE_STEPANGLE(DIAMETERWHEELD,DISTANCEWHEEL);
    param.stepAngleG = COMPUTE_STEPANGLE(DIAMETERWHEELG,DISTANCEWHEEL);
    param.stepForrwardD = COMPUTE_STEPAVANCE(DIAMETERWHEELD);
    param.stepForrwardG = COMPUTE_STEPAVANCE(DIAMETERWHEELG);
    position_t position2;

    circularBufferOdo->resetPopRecord();
    for(int i = 0; i < circularBufferOdo->getNumberSetion();i++){
        for(long int j = circularBufferOdo->getSartPointSection(i); j < circularBufferOdo->getEndPointSection(i);j++){
            odometrieTrigger_t typeTrigger = (odometrieTrigger_t)circularBufferOdo->popRecod();
            odometrieCalc(position2,typeTrigger,param);
        }
        position2.teta = mod_angle(position2.teta);
        usartprintf("x : %lf, y : %lf, theta : %lf\n",position2.x,position2.y,position2.teta);
    }

    for(int a = 0; a < CALIBRATION_IT;a++){
        for(int wg = 0; wg < CALIBRATION_DIV; wg++){
            for(int wd = 0; wd < CALIBRATION_DIV; wd++){
                for(int dist = 0; dist < CALIBRATION_DIV; dist++){
                    position_t position;

                    double wheelG = minWheelG + (wg+0.5) * ((maxWheelG - minWheelG) / CALIBRATION_DIV);
                    double wheelD = minWheelD + (wd+0.5) * ((maxWheelD - minWheelD) / CALIBRATION_DIV);
                    double distance = minDistance + (dist+0.5) * ((maxDistance - minDistance) / CALIBRATION_DIV);

                    param.stepAngleD = COMPUTE_STEPANGLE(wheelD,distance);
                    param.stepAngleG = COMPUTE_STEPANGLE(wheelG,distance);
                    param.stepForrwardD = COMPUTE_STEPAVANCE(wheelD);
                    param.stepForrwardG = COMPUTE_STEPAVANCE(wheelG);
                    position.teta = 0;
                    position.x = 0;
                    position.y = 0;

                    circularBufferOdo->resetPopRecord();
                    for(int i = 0; i < circularBufferOdo->getNumberSetion();i++){
                        for(long int j = circularBufferOdo->getSartPointSection(i); j < circularBufferOdo->getEndPointSection(i);j++){
                            odometrieTrigger_t typeTrigger = (odometrieTrigger_t)circularBufferOdo->popRecod();
                            odometrieCalc(position,typeTrigger,param);
                        }
                        position.teta = mod_angle(position.teta);
                        positionSection[i] = position;
                    }

                    double quality = abs(positionSection[0].teta-90) * Compromis +
                                     abs(positionSection[1].teta) * Compromis + abs(positionSection[1].x) +
                                     abs(positionSection[2].teta) * Compromis + abs(positionSection[2].x) +
                                     abs(positionSection[0].teta-90) * Compromis + abs(positionSection[1].y - positionSection[3].y);

                    if(quality < best){
                        best = quality;
                        bestWheelG = wheelG;
                        bestWheelD = wheelD;
                        bestdistance = distance;
                    }

                    usartprintf("%d,%lf,%lf,%lf,%lf,%lf,\n",totalIteration,position.teta,position.x,wheelD,wheelG,distance);
                    totalIteration++;

                }
            }
        }
        minWheelG  = bestWheelG - (CALIBRATION_START_DIAMETERWHEEL/(CALIBRATION_DIV*pow(2,a)));
        maxWheelG  = bestWheelG + (CALIBRATION_START_DIAMETERWHEEL/(CALIBRATION_DIV*pow(2,a)));
        minWheelD  = bestWheelD - (CALIBRATION_START_DIAMETERWHEEL/(CALIBRATION_DIV*pow(2,a)));
        maxWheelD  = bestWheelD + (CALIBRATION_START_DIAMETERWHEEL/(CALIBRATION_DIV*pow(2,a)));
        minDistance = bestdistance - (CALIBRATION_START_DISTANCEWHEEL/(CALIBRATION_DIV*pow(2,a)));
        maxDistance = bestdistance + (CALIBRATION_START_DISTANCEWHEEL/(CALIBRATION_DIV*pow(2,a)));
    }
    usartprintf("\n\nBEST :\n\twheelG : %lf\n\twheelD : %lf\n\tdistance : %lf\n",bestWheelG,bestWheelD,bestdistance);
    return true;
}


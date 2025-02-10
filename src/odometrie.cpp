#include "odometrie.h"


void odometrieCalc(position_t &position, odometrieTrigger_t typeTrigger, odometrieParam_t param);
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


void exti2_isr(void){
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


void exti4_isr(void){
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
    odometrieParam_t param;
    param.stepAngleD = COMPUTE_STEPANGLE(DIAMETERWHEELD,DISTANCEWHEEL)*DEG_TO_RAD;
    param.stepAngleG = COMPUTE_STEPANGLE(DIAMETERWHEELG,DISTANCEWHEEL)*DEG_TO_RAD;
    param.stepForrwardD = COMPUTE_STEPAVANCE(DIAMETERWHEELD);
    param.stepForrwardG = COMPUTE_STEPAVANCE(DIAMETERWHEELG);
	while (!circularBufferOdo->isEmpty()){
		odometrieCalc(position, (odometrieTrigger_t)circularBufferOdo->pop(), param);
	}
}


void odometrieCalc(position_t &position, odometrieTrigger_t typeTrigger, odometrieParam_t param){
    switch (typeTrigger)
    {
    case fordwardL:
				position.y += param.stepForrwardG * sin(position.teta); //Voir pour optimisation
				position.x += param.stepForrwardG * cos(position.teta); //Voir pour optimisation
				position.teta -= param.stepAngleG;
			break;
		case backwardL:
				position.y -= param.stepForrwardG * sin(position.teta); //Voir pour optimisation
				position.x -= param.stepForrwardG * cos(position.teta); //Voir pour optimisation
				position.teta += param.stepAngleG;
			break;
		case fordwardR:
				position.y += param.stepForrwardD * sin(position.teta); //Voir pour optimisation
				position.x += param.stepForrwardD * cos(position.teta); //Voir pour optimisation
				position.teta += param.stepAngleD;
			break;
		case backwardR:
				position.y -= param.stepForrwardD * sin(position.teta); //Voir pour optimisation
				position.x -= param.stepForrwardD * cos(position.teta); //Voir pour optimisation
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


void computeCalibration(void){
    if(!circularBufferOdo->recordIsValid()){
        usartprintf("Buffer not valid : Overflow\n");
        usartprintf("Create a smaller path to calibrate\n");
        return;
    }
    odometrieParam_t param;
    position_t positionSection[NB_RECORD_SECTION];

    double bestWheelG = 0;
    double bestWheelD = 0;
    double bestdistance = 0;
    double bestx = 0;
    double bestTeta = 0;
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

    circularBufferOdo->resetPopRecord();
    for(int i = 0; i < circularBufferOdo->getNumberSetion();i++){
        for(long int j = circularBufferOdo->getSartPointSection(i); j <= circularBufferOdo->getEndPointSection(i);j++){
            odometrieTrigger_t typeTrigger = (odometrieTrigger_t)circularBufferOdo->popRecod();
            usartprintf("%d,%d\n",j,typeTrigger);
        }
    }

    for(int a = 0; a < CALIBRATION_IT;a++){
        double best = 100000000;
        for(int wg = 0; wg < CALIBRATION_DIV; wg++){
            for(int wd = 0; wd < CALIBRATION_DIV; wd++){
                for(int dist = 0; dist < CALIBRATION_DIV; dist++){
                    position_t position;

                    double wheelG = minWheelG + (wg+0.5) * ((maxWheelG - minWheelG) / CALIBRATION_DIV);
                    double wheelD = minWheelD + (wd+0.5) * ((maxWheelD - minWheelD) / CALIBRATION_DIV);
                    double distance = minDistance + (dist+0.5) * ((maxDistance - minDistance) / CALIBRATION_DIV);

                    param.stepAngleD = COMPUTE_STEPANGLE(wheelD,distance)*DEG_TO_RAD;
                    param.stepAngleG = COMPUTE_STEPANGLE(wheelG,distance)*DEG_TO_RAD;
                    param.stepForrwardD = COMPUTE_STEPAVANCE(wheelD);
                    param.stepForrwardG = COMPUTE_STEPAVANCE(wheelG);
                    position.teta = 0;
                    position.x = 0;
                    position.y = 0;

                    circularBufferOdo->resetPopRecord();
                    for(int i = 0; i < circularBufferOdo->getNumberSetion();i++){
                        for(long int j = circularBufferOdo->getSartPointSection(i); j <= circularBufferOdo->getEndPointSection(i);j++){
                            odometrieTrigger_t typeTrigger = (odometrieTrigger_t)circularBufferOdo->popRecod();
                            odometrieCalc(position,typeTrigger,param);
                        }
                        position_t tmpPos = position;
                        tmpPos.teta = mod_angle(tmpPos.teta*RAD_TO_DEG);
                        positionSection[i] = tmpPos;
                    }

                    double quality = abs(positionSection[0].teta) * Compromis + abs(positionSection[0].x - (2998-250-200)) +
                                     abs(positionSection[1].teta) * Compromis + abs(positionSection[1].x);

                    if(quality < best){
                        best = quality;
                        bestWheelG = wheelG;
                        bestWheelD = wheelD;
                        bestdistance = distance;
                        bestx = abs(positionSection[0].x - (2998-250-200)) + abs(positionSection[1].x);
                        bestTeta = abs(positionSection[0].teta) * Compromis + abs(positionSection[1].teta) * Compromis;
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
        usartprintf("\n\nBEST : \n\tbest : %lf\n\tbestX : %lf\n\tbestTeta : %lf\n\twheelG : %lf\n\twheelD : %lf\n\tdistance : %lf\n",best,bestx,bestTeta,bestWheelG,bestWheelD,bestdistance);
    }
}


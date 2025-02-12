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
    position_t position;
    odometrieTrigger_t typeTrigger;

    double bestWheelG = -1;
    double bestWheelD = -1;
    double bestdistance  = -1;

    double best = 100000000;
    double Compromis = 1;

    int totalIteration = 0;
    // int lastPourcent = 0;

    for(double wheelG = DIAMETERWHEELG - 1; wheelG < DIAMETERWHEELG + 1; wheelG+=0.2){
        for(double wheelD = DIAMETERWHEELD - 1; wheelD < DIAMETERWHEELD + 1; wheelD+=0.2){
            for(double distance = DISTANCEWHEEL - 1; distance < DISTANCEWHEEL + 1; distance+=0.2){

                // int currentPourcent = (double)totalIteration/(20*20*20);
                // if(lastPourcent != currentPourcent){
                //     lastPourcent = currentPourcent;
                //     usartprintf("[%d%] Compute best param Odo \twheelG : %.5lf wheelD : %.5lf distance : %.5lf\n",currentPourcent,bestWheelG,bestWheelD,bestdistance);
                // }
                totalIteration++;

                circularBufferOdo->resetPopRecord();
                param.stepAngleD = COMPUTE_STEPANGLE(wheelD,distance);
                param.stepAngleG = COMPUTE_STEPANGLE(wheelG,distance);
                param.stepForrwardD = COMPUTE_STEPAVANCE(wheelD);
                param.stepForrwardG = COMPUTE_STEPAVANCE(wheelG);
                position.teta = 0;
                position.x = 0;
                position.y = 0;

                while (!circularBufferOdo->recordIsEmpty()){
                    typeTrigger = (odometrieTrigger_t)circularBufferOdo->popRecod();
                    odometrieCalc(position,typeTrigger,param);
                }

                position.teta = mod_angle(position.teta);

                usartprintf("%d,%lf,%lf,%lf,%lf,%lf,\n",totalIteration,position.teta,position.x,wheelD,wheelG,distance);

                double quality = abs(position.teta) * Compromis + abs(position.x);
                if(quality < best){
                    best = quality;
                    bestWheelG = wheelG;
                    bestWheelD = wheelD;
                    bestdistance = distance;
                }

            }
        }
    }
    usartprintf("\n\nBEST :\n\twheelG : %lf\n\twheelD : %lf\n\tdistance : %lf\n",bestWheelG,bestWheelD,bestdistance);
    return true;
}


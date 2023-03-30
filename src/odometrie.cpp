#include "odometrie.h"


static double mod_angle(double a);



position_t position;
odometrieTrigger_t buffer[_BUFFERSIZE];
int endBuffer = 0;
int startBuffer =0;


void odometrieSetup(void){
    position.x = 0;
    position.y = 0;
    position.teta = 0;

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
	if(gpio_get (GPIOB,GPIO3)){
		buffer[startBuffer]=backwardR;
		startBuffer++;
		if(startBuffer>=_BUFFERSIZE){
			startBuffer = 0;
		}
	}
	else{
		//Vers l'avant
		buffer[startBuffer]=fordwardR;
		startBuffer++;
		if(startBuffer>=_BUFFERSIZE){
			startBuffer = 0;
		}
	}
}

void exti4_isr(void)
{
	exti_reset_request(EXTI4);
    gpio_toggle(port_led2,pin_led2);
	if(gpio_get (GPIOB,GPIO5)){
		//Vers l'avant
		buffer[startBuffer]=fordwardL;
		startBuffer++;
		if(startBuffer>=_BUFFERSIZE){
			startBuffer = 0;
		}
	}
	else{
		buffer[startBuffer]=backwardL;
		startBuffer++;
		if(startBuffer>=_BUFFERSIZE){
			startBuffer = 0;
		}
	}
}

void printBuffer(void){
	for(int i =0; i<_BUFFERSIZE;i++){
		usartprintf("%d",buffer[i]);
	}
	usartprintf("\n");
}

void odometrieLoop(void){
	int i =0;
	while (endBuffer != startBuffer){
		switch (buffer[endBuffer])
		{
		case fordwardL:
				position.y -= STEPAVANCEG * cos(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.x -= STEPAVANCEG * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.teta += STEPANGLEG;
			break;
		case backwardL:
				position.y += STEPAVANCEG * cos(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.x += STEPAVANCEG * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.teta -= STEPANGLEG;
			break;
		case fordwardR:
				position.y -= STEPAVANCED * cos(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.x -= STEPAVANCED * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.teta -= STEPANGLED;
			break;
		case backwardR:
				position.y += STEPAVANCED * cos(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.x += STEPAVANCED * sin(COEFCONVDEGRETORADIAN*(position.teta-90)); //Voir pour optimisation
				position.teta += STEPANGLED;
			break;
		default:
			break;
		}
		i++;
		endBuffer++;
		if(endBuffer>=_BUFFERSIZE){
			endBuffer = 0;
		}	
	}
}

void printPosition(void){
	usartprintf(">x:%lf\n>y:%lf\n>teta:%lf\n",position.x,position.y,position.teta);
}

// odometrieGetPosition(position_t positionSet){
// 	positionSet.teta = position.teta;
// 	positionSet.y = position.y;
// 	positionSet.x = position.x;
// }

position_t odometrieGetPosition(void){
	position.teta = mod_angle(position.teta);
	return position;
}


position_u odometrieGetPositionInt(void){
	position_u positionUnion;
	positionUnion.position.teta = (int)(mod_angle(position.teta));
	positionUnion.position.y = (int)(position.y);
	positionUnion.position.x = (int)(position.x);
	return positionUnion;
}

void odometrieSetPosition(position_u positionUnion){
	position.teta = (double)positionUnion.position.teta;
	position.x = (double)positionUnion.position.x;
	position.y = (double)positionUnion.position.y;
}

static double mod_angle(double a){
	a = fmod(a,360);
	if(a>180){
		a -=360;
	}
	else if(a<-180){
		a +=360;
	}
	return a;
}
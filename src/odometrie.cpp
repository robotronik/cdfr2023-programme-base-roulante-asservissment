#include "odometrie.h"


odometrieTrigger_t buffer[_BUFFERSIZE];
int endBuffer = 0;
int startBuffer =0;


void odometrieSetup(void){

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
    gpio_toggle(port_led1,pin_led1);
	if(gpio_get (port_odometrie2L,pin_odometrie2L)){
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

void odometrieLoop(robot* robot){
	position_t position = robot->getPosition();
	while (endBuffer != startBuffer){
		switch (buffer[endBuffer])
		{
		case fordwardL:
				position.y -= STEPAVANCEG * cos(COEFCONVDEGRETORADIAN*(position.theta+90)); //Voir pour optimisation
				position.x -= STEPAVANCEG * sin(COEFCONVDEGRETORADIAN*(position.theta-90)); //Voir pour optimisation
				position.theta -= STEPANGLEG;
			break;
		case backwardL:
				position.y += STEPAVANCEG * cos(COEFCONVDEGRETORADIAN*(position.theta+90)); //Voir pour optimisation
				position.x += STEPAVANCEG * sin(COEFCONVDEGRETORADIAN*(position.theta-90)); //Voir pour optimisation
				position.theta += STEPANGLEG;
			break;
		case fordwardR:
				position.y -= STEPAVANCED * cos(COEFCONVDEGRETORADIAN*(position.theta+90)); //Voir pour optimisation
				position.x -= STEPAVANCED * sin(COEFCONVDEGRETORADIAN*(position.theta-90)); //Voir pour optimisation
				position.theta += STEPANGLED;
			break;
		case backwardR:
				position.y += STEPAVANCED * cos(COEFCONVDEGRETORADIAN*(position.theta+90)); //Voir pour optimisation
				position.x += STEPAVANCED * sin(COEFCONVDEGRETORADIAN*(position.theta-90)); //Voir pour optimisation
				position.theta -= STEPANGLED;
			break;
		default:
			break;
		}
		endBuffer++;
		if(endBuffer>=_BUFFERSIZE){
			endBuffer = 0;
		}	
	}
	position.time = get_uptime_ms();
	robot->updatePostion(position);
}


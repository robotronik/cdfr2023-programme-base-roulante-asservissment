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
				position.y += STEPAVANCEG * sin(position.teta); //Voir pour optimisation
				position.x += STEPAVANCEG * cos(position.teta); //Voir pour optimisation
				position.teta -= STEPANGLEG*DEG_TO_RAD;
			break;
		case backwardL:
				position.y -= STEPAVANCEG * sin(position.teta); //Voir pour optimisation
				position.x -= STEPAVANCEG * cos(position.teta); //Voir pour optimisation
				position.teta += STEPANGLEG*DEG_TO_RAD;
			break;
		case fordwardR:
				position.y += STEPAVANCED * sin(position.teta); //Voir pour optimisation
				position.x += STEPAVANCED * cos(position.teta); //Voir pour optimisation
				position.teta += STEPANGLED*DEG_TO_RAD;
			break;
		case backwardR:
				position.y -= STEPAVANCED * sin(position.teta); //Voir pour optimisation
				position.x -= STEPAVANCED * cos(position.teta); //Voir pour optimisation
				position.teta -= STEPANGLED*DEG_TO_RAD;
			break;
		default:
			break;
		}
	}
}
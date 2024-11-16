#include "odometrie.h"


odometrieTrigger_t buffer[_BUFFERSIZE];
int bufferIdx =0;


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
		buffer[bufferIdx]=backwardR;
		bufferIdx++;
		if(bufferIdx>=_BUFFERSIZE){
			bufferIdx = 0;
		}
	}
	else{
		//Vers l'avant
		buffer[bufferIdx]=fordwardR;
		bufferIdx++;
		if(bufferIdx>=_BUFFERSIZE){
			bufferIdx = 0;
		}
	}
}

void exti4_isr(void)
{
	exti_reset_request(EXTI4);
    gpio_toggle(port_led1,pin_led1);
	if(gpio_get (port_odometrie2L,pin_odometrie2L)){
		//Vers l'avant
		buffer[bufferIdx]=fordwardL;
		bufferIdx++;
		if(bufferIdx>=_BUFFERSIZE){
			bufferIdx = 0;
		}
	}
	else{
		buffer[bufferIdx]=backwardL;
		bufferIdx++;
		if(bufferIdx>=_BUFFERSIZE){
			bufferIdx = 0;
		}
	}
}

void printBuffer(void){
	for(int i =0; i<_BUFFERSIZE;i++){
		usartprintf("%d",buffer[i]);
	}
	usartprintf("\n");
}

void odometrieLoop(position_t position, position_t* updatedPos){
	int i = 0;
	while (i < bufferIdx){
		double a = DEG_TO_RAD*(position.theta);
		switch (buffer[i])
		{
			//This could be improved, needs to be tested
		case fordwardL:
				position.y += STEP_FWD_L * sin(a);
				position.x += STEP_FWD_L * cos(a);
				position.theta -= STEP_ANGLE_L;
			break;
		case backwardL:
				position.y -= STEP_FWD_L * sin(a);
				position.x -= STEP_FWD_L * cos(a);
				position.theta += STEP_ANGLE_L;
			break;
		case fordwardR:
				position.y += STEP_FWD_R * sin(a);
				position.x += STEP_FWD_R * cos(a);
				position.theta += STEP_ANGLE_R;
			break;
		case backwardR:
				position.y -= STEP_FWD_R * sin(a);
				position.x -= STEP_FWD_R * cos(a);
				position.theta -= STEP_ANGLE_R;
			break;
		default:
			break;
		}
		i++;
	}
	bufferIdx = 0;
	position.time = get_uptime_ms();
	updatedPos = &position;
}


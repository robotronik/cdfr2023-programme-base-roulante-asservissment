#include "odometrie.h"
#define _GNU_SOURCE
#include "math.h"

static double mod_angle(double a);



positionD position;
static odometrieTrigger buffer[_BUFFERSIZE];
static int ReadBuffer = 0;
static int WriteBuffer = 0;


void odometrieSetup(void){
	position.x = 0;
	position.y = 0;
	position.theta = 0;

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
	//gpio_toggle(port_led1,pin_led1);
	if(gpio_get (GPIOB,GPIO3)){
		buffer[WriteBuffer]=odometrieTrigger::backwardR;
		WriteBuffer++;
		if(WriteBuffer>=_BUFFERSIZE){
			WriteBuffer = 0;
		}
	}
	else{
		//Vers l'avant
		buffer[WriteBuffer]=odometrieTrigger::fordwardR;
		WriteBuffer++;
		if(WriteBuffer>=_BUFFERSIZE){
			WriteBuffer = 0;
		}
	}
}

void exti4_isr(void)
{
	exti_reset_request(EXTI4);
	//gpio_toggle(port_led2,pin_led2);
	if(gpio_get (GPIOB,GPIO5)){
		//Vers l'avant
		buffer[WriteBuffer]=odometrieTrigger::fordwardL;
		WriteBuffer++;
		if(WriteBuffer>=_BUFFERSIZE){
			WriteBuffer = 0;
		}
	}
	else{
		buffer[WriteBuffer]=odometrieTrigger::backwardL;
		WriteBuffer++;
		if(WriteBuffer>=_BUFFERSIZE){
			WriteBuffer = 0;
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
	while (ReadBuffer != WriteBuffer){
		double sv, cv;
		double anglerad = position.theta*DEG2RAD;
		sv = sin(anglerad);
		cv = cos(anglerad);
		//sincos(position.theta*DEG2RAD, &sv, &cv);
		switch (buffer[ReadBuffer])
		{
		case odometrieTrigger::fordwardL:
				position.y -= STEPAVANCEG * sv;
				position.x += STEPAVANCEG * cv;
				position.theta += STEPANGLEG;
			break;
		case odometrieTrigger::backwardL:
				position.y += STEPAVANCEG * sv;
				position.x -= STEPAVANCEG * cv;
				position.theta -= STEPANGLEG;
			break;
		case odometrieTrigger::fordwardR:
				position.y -= STEPAVANCED * sv;
				position.x += STEPAVANCED * cv;
				position.theta -= STEPANGLED;
			break;
		case odometrieTrigger::backwardR:
				position.y += STEPAVANCED * sv;
				position.x -= STEPAVANCED * cv;
				position.theta += STEPANGLED;
			break;
		default:
			break;
		}
		i++;
		ReadBuffer++;
		if(ReadBuffer>=_BUFFERSIZE){
			ReadBuffer = 0;
		}	
	}
}

void printPosition(void){
	usartprintf(">x:%lf\n>y:%lf\n>teta:%lf\n",position.x,position.y,position.theta);
}

// odometrieGetPosition(positionD positionSet){
// 	positionSet.theta = position.theta;
// 	positionSet.y = position.y;
// 	positionSet.x = position.x;
// }

positionD odometrieGetPosition(void){
	position.theta = mod_angle(position.theta);
	return position;
}


positionSI odometrieGetPositionInt(void){
	positionSI positioni;
	positioni.theta = (int)(mod_angle(position.theta));
	positioni.y = (int)(position.y);
	positioni.x = (int)(position.x);
	return positioni;
}

void odometrieSetPosition(positionSI positioni){
	position.theta = (double)positioni.theta;
	position.x = (double)positioni.x;
	position.y = (double)positioni.y;
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
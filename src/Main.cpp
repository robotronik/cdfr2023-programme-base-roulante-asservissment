#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>

#include "config.h"
#include "motor.h"
#include "uart.h"
#include "odometrie.h"
#include "clock.h"
#include "I2C.h"
#include "asservissement.h"


static void ledSetup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(port_led1, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led1);
	gpio_mode_setup(port_led2, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led2);

	//gpio_mode_setup(GPIOA,GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO5);
}

enum class I2CCommands : uint8_t
{
	LED1ON=10,
	LED1OFF,
	LED2ON,
	LED2OFF,

	GetPositionInt=20,
	SetPositionInt,

	SetTargetPosition=30,
	SetTargetAngle,
	Brake,
	StopAsservissement,

	SetPIDLinearPos=40,
	SetPIDLinearVel,
	SetPIDAngularPos,
	SetPIDAngularVel
};


void I2CRecieveData(uint8_t* data, int size){
	uint8_t* payloadptr = &data[1];
	switch (data[0])
	{
	case (uint8_t)I2CCommands::LED1ON :
		gpio_set(port_led1,pin_led1);
		break;
	case (uint8_t)I2CCommands::LED1OFF :
		gpio_clear(port_led1,pin_led1);
		break;
	case (uint8_t)I2CCommands::LED2ON :
		gpio_set(port_led2,pin_led2);
		break;
	case (uint8_t)I2CCommands::LED2OFF :
		gpio_clear(port_led2,pin_led2);
		break;
	case (uint8_t)I2CCommands::GetPositionInt :
		{
			positionSI posi = odometrieGetPositionInt();
			I2CSetBuffer((uint8_t*)&posi,sizeof(posi));
		}
		break;
	case (uint8_t)I2CCommands::SetPositionInt :
		if (size >= sizeof(positionSI)+1)
		{
			positionSI payload;
			memcpy(&payload, payloadptr, sizeof(payload));
			odometrieSetPosition(payload);
			asservissementSetup();
		}
		break;
	case (uint8_t)I2CCommands::SetTargetPosition :
		if (size >= sizeof(positionSI)+1)
		{
			struct posconv
			{
				int16_t x, y, arriere;
			};
			posconv payload;
			memcpy(&payload, payloadptr, sizeof(payload));
			asservissementSetup();
			setLinearAsservissement(payload.x, payload.y, payload.arriere);
		}
		break;
	case (uint8_t)I2CCommands::SetTargetAngle :
		if (size >= sizeof(int16_t)+1)
		{
			int16_t theta;
			memcpy(&theta, payloadptr, sizeof(theta));
			asservissementSetup();
			setAngularAsservissement(theta);
		}
		break;
	case (uint8_t)I2CCommands::Brake :
		asservissementBrake();
		break;
	case (uint8_t)I2CCommands::StopAsservissement :
		asservissementStop();
		break;

	case (uint8_t)I2CCommands::SetPIDLinearPos :
	case (uint8_t)I2CCommands::SetPIDLinearVel :
	case (uint8_t)I2CCommands::SetPIDAngularPos :
	case (uint8_t)I2CCommands::SetPIDAngularVel :
		if (size >= sizeof(float)*3+1)
		{
			struct pidvalues
			{
				float kp, ki, kd;
			};
			pidvalues payload;
			memcpy(&payload, payloadptr, sizeof(payload));
			SetPIDValues(data[0]-(uint8_t)I2CCommands::SetPIDLinearPos, payload.kp, payload.ki, payload.kd);
		}
	default:
		usartprintf("Received unknown command : %d\n", data[0]);
		break;
	}
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
	asservissementSetup();



	//WAIT
	for (int i = 0; i < 100000000; i++) {	/* Wait a bit. */
		__asm__("nop");
	}
	usartprintf("Start\n");

	//UNBRAKE MOTOR
	motorBrakeR(0);
	motorBrakeL(0);

	motorSetModeR(0);
	motorSetModeL(0);

		//TEST MOTOR
		// motorSpeedSignedL(5);
		// motorSpeedSignedR(5);
		// while (1);

		// for (int i = 0; i < 100; i++){
		// 	motorSpeedSignedL(i);
		// 	motorSpeedSignedR(i);
		// 	delay_ms(50);
		// }
		// for (int i = 100; i > -100; i--){
		// 	motorSpeedSignedL(i);
		// 	motorSpeedSignedR(i);
		// 	delay_ms(50);
		// }

		// for (int i = -100; i < 0; i++){
		// 	motorSpeedSignedL(i);
		// 	motorSpeedSignedR(i);
		// 	delay_ms(50);
		// }


		
	
	//LOOP
	uint32_t PrintTime =  get_uptime_ms()+500;
	uint32_t changePointTime =  get_uptime_ms()+2000;
	//uint32_t changePointTime2 =  get_uptime_ms()+2000;
	//uint32_t changePointTime3 =  get_uptime_ms()+7000;
	
	

	
	

	while (1) {
		odometrieLoop();
		asservissementLoop();

		//PRINT DEBUG
		if(PrintTime<get_uptime_ms()){
			PrintTime =  get_uptime_ms()+500;
			//printPosition();
			//printAllInformation();
		}

		//BLINK LED
		if(changePointTime < get_uptime_ms()){
			changePointTime = get_uptime_ms() + 1000;
			gpio_toggle(port_led1,pin_led1);
		}

		//TEST ASSERVISSEMENT
		// if(changePointTime2 < get_uptime_ms()){
		// 	changePointTime2 = get_uptime_ms() + 5000000;
		// 	setLinearAsservissement(1000,0,false);
		// 	//setAngularAsservissement(200);
		// }

		// if(changePointTime3 < get_uptime_ms()){
		// 	changePointTime3 = get_uptime_ms() + 5000000;
		// 	setLinearAsservissement(0,0,true);
		// 	//setAngularAsservissement(0);
		// }
		
	}

	return 0;
}
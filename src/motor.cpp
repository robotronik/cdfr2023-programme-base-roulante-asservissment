#include "Motor.h"
#include "uart.h"
#include "clock.h"
#include "math.h"

// BLDC motor driver
// https://datasheet.datasheetarchive.com/originals/distributors/Datasheets-DGA5/483784.pdf

// TODO
// Count rising edges on a pin using a timer in external clock mode for tacho

//local fonctions
static uint8_t current_channel_indx = 0;
static uint8_t adc_channels[] = {adc_channelA, adc_channelB, adc_channelC};
static uint16_t * adc_values_registers[] = {NULL, NULL, NULL};
void setuptimer(void);
void adc_setup(void);

void setupDriveGPIO();

void DriveDisable(){
	gpio_clear(port_CoastDrive, pin_CoastDrive);
	driveEnabled = false;
}
void DriveEnable(){
	gpio_set(port_CoastDrive, pin_CoastDrive);
	driveEnabled = true;
}

/*
Sets the current-decay method. Referring to table 3, when
in slow-decay mode, MODE = 1, only the high-side MOSFET
is switched off during a PWM-off cycle. In the fast-decay mode,
MODE = 0, the device switches both the high-side and low-side
MOSFETs. 
*/
void SetDriveMode(int mode){
	if (mode == 1)
		gpio_set(port_ModeDrive, pin_ModeDrive);
	else
		gpio_clear(port_ModeDrive, pin_ModeDrive);
}

// Reset the motor drivers by lowering the reset pin for 10ms
void ResetDrive(){
	gpio_clear(port_ResetDrive, pin_ResetDrive);
	delay_ms(10);
	gpio_set(port_ResetDrive, pin_ResetDrive);
	delay_ms(1);
}

void DriveSetup(){
	setupDriveGPIO();
	motorA.Setup();
	motorB.Setup();
	motorC.Setup();
	setuptimer();
	adc_setup();
}

Motor::Motor(int motorID, double wheelDiameter, double wheelDistance, double wheelAngle, 
	int port_SpeedControl, int pin_SpeedControl,
	int port_Direction, int pin_Direction,
	int port_Brake, int pin_Brake,
	int port_ESF, int pin_ESF,
	int port_Tacho, int pin_Tacho,
	int port_Err1, int pin_Err1,
	int port_Err2, int pin_Err2,
	int port_InfoDir, int pin_InfoDir,
	tim_oc_id oc_id) :
	id(motorID), wheelDiameter(wheelDiameter), wheelDistance(wheelDistance), wheelAngle(wheelAngle),
	port_SpeedControl(port_SpeedControl), pin_SpeedControl(pin_SpeedControl),
	port_Direction(port_Direction), pin_Direction(pin_Direction),
	port_Brake(port_Brake), pin_Brake(pin_Brake),
	port_ESF(port_ESF), pin_ESF(pin_ESF),
	port_Tacho(port_Tacho), pin_Tacho(pin_Tacho),
	port_Err1(port_Err1), pin_Err1(pin_Err1),
	port_Err2(port_Err2), pin_Err2(pin_Err2),
	port_InfoDir(port_InfoDir), pin_InfoDir(pin_InfoDir),
	oc_id(oc_id)
{
	name = 'A' + motorID;
	adc_values_registers[id] = &adc_value;
}

void Motor::Setup(){
	setupGPIO();
}

void Motor::SetSpeedSigned(int speed){
	SetSpeedUnsigned(abs(speed), speed<0);
}

void Motor::SetSpeedUnsigned(int speed, bool reverse){
	SetDirection(reverse);
	SetSpeed(speed);
}

void Motor::SetDirection(bool reverse){
	if (reverse)
		gpio_set(port_Direction,pin_Direction);
	else
		gpio_clear(port_Direction,pin_Direction);
}

void Motor::Brake(bool brake){
	if (brake)
		gpio_clear(port_Brake,pin_Brake);	
	else
		gpio_set(port_Brake,pin_Brake);
}


void Motor::SetSpeed(int speed){
	speed = CLAMP(speed,0,maxSpeed);
	int pwmVal = speed * COEFMULT; // (speed/2 + 50)

	timer_set_oc_value(TIM1, oc_id, pwmVal);
}

void Motor::SetMaxSpeed(int max){
	maxSpeed = CLAMP(max,0,100);
}

void Motor::SetMaxTorque(int torque){
	maxTorque = (torque*4096)/100;
	if (maxTorque > 4095) {
		doesLimitTorque = false;
	}
	else {
		doesLimitTorque = true;
		adc_start_conversion_regular(ADC1);
	}
}

// Returns the current in A
double Motor::GetCurrent() {
	// ADC value between 0 and 4095
	// Resistor value = 0.018 Ohm
	// Vref = 3.3V

	// Convert ADC value to voltage
	double voltage = (double)(adc_value) / 4095.0 * 3.3;
	double CSOutVoltage = voltage * (4.7+2.2) / 2.2; // Voltage divider

	// VCSOUT ≈ (ILOAD × AV × RSENSE) + VOOS
	double current = (CSOutVoltage - 0.320) / (19 * 0.018);

    return current;
}

fault_action_t Motor::GetFault(){
	int err1 = gpio_get(port_Err1, pin_Err1);
	int err2 = gpio_get(port_Err2, pin_Err2);

	switch (err1 + err2 << 1) {
		case 0b00: 
			return FAULT_UNDERVOLTAGE;
		case 0b01:
			return FAULT_SHORT;
		case 0b10:
			return FAULT_LOW_LOAD_CURRENT;
		case 0b11:
			return FAULT_NONE;
	}
	return FAULT_NONE;
}

void Motor::PrintValues() {
    usartprintf(">ADC of %c: %4d /4095\n", adc_value);
    usartprintf(">Current of %c: %lf A\n", GetCurrent());
}

void setupDriveGPIO(void){
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);

	gpio_mode_setup(port_ResetDrive, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_ResetDrive);
	gpio_mode_setup(port_CoastDrive, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_CoastDrive);
	gpio_mode_setup(port_ModeDrive, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_ModeDrive);
	
	gpio_set(port_ResetDrive, port_ResetDrive);
	SetDriveMode(0);
	DriveDisable();
}

void Motor::setupGPIO(void){
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_ADC1);

	gpio_mode_setup(port_Direction, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_Direction);
	gpio_mode_setup(port_Brake, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_Brake);
	gpio_mode_setup(port_ESF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_ESF);

	gpio_mode_setup(port_Err1, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin_Err1);
	gpio_mode_setup(port_Err2, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin_Err2);
    gpio_mode_setup(port_Tacho, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin_Tacho);
	gpio_mode_setup(port_InfoDir, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin_InfoDir);

	Brake(false);
	SetDirection(false);

	// Disable ESF
	gpio_clear(port_ESF, pin_ESF);

	// setup PWM AF for the speed control
	gpio_mode_setup(port_SpeedControl, GPIO_MODE_AF, GPIO_PUPD_NONE, pin_SpeedControl);
	gpio_set_af(port_SpeedControl, GPIO_AF1, pin_SpeedControl);
	gpio_set_output_options(port_SpeedControl, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, pin_SpeedControl);
	SetSpeed(0);
}

void setuptimer(void){

	// Enable TIM1 clock.
	rcc_periph_clock_enable(RCC_TIM1);

	/* Reset TIM1 peripheral. */
	rcc_periph_reset_pulse(RST_TIM1);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 */
	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT,TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/* Reset prescaler value. */
	timer_set_prescaler(TIM1, 0);

	/* Reset repetition counter value. */
	timer_set_repetition_counter(TIM1, 0);

	/* Enable preload. */
	timer_enable_preload(TIM1);

	/* Continuous mode. */
	timer_continuous_mode(TIM1);

	//depend of the value of the compnents on the PCB
	timer_set_period(TIM1, TIMERPERIOD);

	/* -- OC3 configuration -- */
	/* Disable outputs. */
	timer_disable_oc_output(TIM1, TIM_OC2N);
	timer_disable_oc_output(TIM1, TIM_OC1N);
	timer_disable_oc_output(TIM1, TIM_OC3N);

	/* Configure global mode of line 3. */
	timer_disable_oc_clear(TIM1, TIM_OC2);
	timer_enable_oc_preload(TIM1, TIM_OC2);
	timer_set_oc_slow_mode(TIM1, TIM_OC2);
	timer_set_oc_mode(TIM1, TIM_OC2, TIM_OCM_PWM1);

	timer_disable_oc_clear(TIM1, TIM_OC1);
	timer_enable_oc_preload(TIM1, TIM_OC1);
	timer_set_oc_slow_mode(TIM1, TIM_OC1);
	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);

	timer_disable_oc_clear(TIM1, TIM_OC3);
	timer_enable_oc_preload(TIM1, TIM_OC3);
	timer_set_oc_slow_mode(TIM1, TIM_OC3);
	timer_set_oc_mode(TIM1, TIM_OC3, TIM_OCM_PWM1);


	/* Configure OC3N. */
	timer_set_oc_polarity_high(TIM1, TIM_OC2N);
	timer_set_oc_polarity_high(TIM1, TIM_OC1N);
	timer_set_oc_polarity_high(TIM1, TIM_OC3N);

	/* Set the capture compare value to 0 when the robot booting */
	timer_set_oc_value(TIM1, TIM_OC2, 0);
	timer_set_oc_value(TIM1, TIM_OC1, 0);
	timer_set_oc_value(TIM1, TIM_OC3, 0);


	/* Reenable outputs. */
	timer_enable_oc_output(TIM1, TIM_OC2N);
	timer_enable_oc_output(TIM1, TIM_OC1N);
	timer_enable_oc_output(TIM1, TIM_OC3N);

	/* ARR reload enable. */
	timer_enable_preload(TIM1);

	/* Enable outputs in the break subsystem. */
	timer_enable_break_main_output(TIM1);

	/* Counter enable. */
	timer_enable_counter(TIM1);
}

void adc_setup(void) {

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_ADC1);

    gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1); // A - PA1 = ADC_Channel_1
    gpio_mode_setup(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO3); // B - PC3 = ADC_Channel_11
    gpio_mode_setup(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0); // C - PC0 = ADC_Channel_10


    adc_power_off(ADC1);
    adc_set_single_conversion_mode(ADC1);
    adc_disable_external_trigger_regular(ADC1);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_480CYC);
    adc_enable_eoc_interrupt(ADC1);
    nvic_enable_irq(NVIC_ADC_IRQ);
	nvic_set_priority(NVIC_ADC_IRQ, 10);

    adc_power_on(ADC1);

    //delay_ms(1);

    // Démarrer la première conversion sur le canal 2
    adc_set_regular_sequence(ADC1, 1, adc_channels);
    adc_start_conversion_regular(ADC1);
}

// Fonction d'interruption pour l'ADC
void adc_isr(void) {
    if (adc_eoc(ADC1)) { // Vérifier si la conversion est terminée
		if (adc_values_registers[current_channel_indx] != NULL) {
			*adc_values_registers[current_channel_indx] = adc_read_regular(ADC1);
		}
        // Configurer le prochain canal et relancer la conversion
		current_channel_indx++;
		if (current_channel_indx >= sizeof(adc_channels)/sizeof(adc_channels[0])) 
			current_channel_indx = 0;
		
        adc_set_regular_sequence(ADC1, 1, adc_channels + current_channel_indx);
		adc_start_conversion_regular(ADC1);
    }
}
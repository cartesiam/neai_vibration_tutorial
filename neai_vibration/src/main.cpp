/**
*******************************************************************************
* @file   main.cpp
* @brief  Main program body
*******************************************************************************
* Main program for collecting data and testing NanoEdge AI solution
*
* Compiler Flags
* -DDATA_LOGGING : data logging mode for collecting data
* -DNEAI_EMU     : test mode with NanoEdge AI Emulator 
* -DNEAI_LIB     : test mode with NanoEdge AI Library
*
* @note   if no compiler flag then data logging mode by default
*******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "mbed.h"
#include "bmi160.h"
#ifndef DATA_LOGGING
#include "NanoEdgeAI.h"
#endif

/* Defines -------------------------------------------------------------------*/
#if !defined(DATA_LOGGING) && !defined(NEAI_EMU) && !defined(NEAI_LIB)
#define DATA_LOGGING
#endif
#ifdef DATA_LOGGING
#define DATA_INPUT_USER 512
#define AXIS_NUMBER 3
#define LOG_NUMBER 100
#else
#define LEARNING_NUMBER 90 /* Number of learning signals */
#endif

/* Objects -------------------------------------------------------------------*/
Serial pc(USBTX, USBRX);
Ticker toggle_led_ticker;
DigitalOut myled(LED2);
I2C i2c(I2C_SDA, I2C_SCL);
BMI160_I2C imu(i2c, BMI160_I2C::I2C_ADRS_SDO_HI);
BMI160::AccConfig accConfig;
BMI160::SensorData accData;
InterruptIn mybutton(USER_BUTTON);

/* Variables -----------------------------------------------------------------*/
float acc_x = 0.F;
float acc_y = 0.F;
float acc_z = 0.F;
float last_acc_x = 0.F;
float last_acc_y = 0.F;
float last_acc_z = 0.F;
#ifndef DATA_LOGGING
uint8_t similarity = 0;
uint16_t learn_cpt = 0;
#endif

/* Buffer with accelerometer values for x-, y- and z-axis --------------------*/
float acc_buffer[DATA_INPUT_USER * AXIS_NUMBER] = {0.F};

/* Functions prototypes ------------------------------------------------------*/
#ifdef DATA_LOGGING
void data_logging_mode(void);
#endif
#ifdef NEAI_EMU
void neai_emulator_test_mode(void);
#endif
#ifdef NEAI_LIB
void neai_library_test_mode(void);
#endif
void init(void);
void init_bmi160(void);
void toggle_led(void);
void fill_acc_buffer(void);
void get_acc_values(void);

/* BEGIN CODE-----------------------------------------------------------------*/
/**
  * @brief  The application entry point
  *
  * @retval int
  */
int main()
{
	/* Initialization */
	init();
	
#ifdef DATA_LOGGING
	/* Data logging mode */
	/* Compiler flag: -DDATA_LOGGING */
	data_logging_mode();
#endif

#ifdef NEAI_EMU
	/* NanoEdge AI Emulator test mode */
	/* Compiler flag -DNEAI_EMU */
	neai_emulator_test_mode();
#endif
	
#ifdef NEAI_LIB
	/* NanoEdge AI Library test mode */
	/* Compiler flag -DNEAI_LIB */
	neai_library_test_mode();
#endif
}

/* Functions definition ------------------------------------------------------*/
#ifdef DATA_LOGGING
/**
 * @brief  Data logging process
 *
 * @param  None
 * @retval None
 */
void data_logging_mode()
{
	while(1) {
		/* Press user button to start a logging process */
		/* Blink LED: running logging process */		
		/* Process is repeat each time you push "User" button */
		if (mybutton == 0) {
			/* Blink LED  during logging process */
			toggle_led_ticker.attach(&toggle_led, 0.1);

			/* Wait one seconds before launching logging process */
			thread_sleep_for(1000);

			/* Logging process */
			for (uint8_t ilog = 0; ilog < LOG_NUMBER; ilog++) {
				fill_acc_buffer();
			}

			/* Stop blink LED (end of logging process) */
			toggle_led_ticker.detach();
			myled = 0;
		}
	}
}
#endif

#ifdef NEAI_EMU
/**
 * @brief  Testing process with NanoEdge AI emulator
 *
 * @param  None
 * @retval None
 */
void neai_emulator_test_mode()
{
	/* Press user button to start */
	while(1) {
		if (mybutton == 0) {
			pc.printf("%d\n", LEARNING_NUMBER);
			while(1) {
				fill_acc_buffer();
			}
		}
	}
}
#endif

#ifdef NEAI_LIB
/**
 * @brief  Testing process with NanoEdge AI library
 *
 * @param  None
 * @retval None
 */
void neai_library_test_mode()
{
	/* Learning process */
	/* Press user button to start the learning process */
	while (learn_cpt < LEARNING_NUMBER) {
		if (mybutton == 0) {
			/* Wait one seconds before starting learning process */
			thread_sleep_for(1000);

			/* Learning process for one speed */
			for (uint16_t i = 0; i < LEARNING_NUMBER; i++) {
				fill_acc_buffer();
				similarity = NanoEdgeAI_learn(acc_buffer);
				pc.printf("%d\n", (int)(learn_cpt * 100) / LEARNING_NUMBER);
				learn_cpt++;
			}
		}
	}
		
	/* Detection process */
	/* LED off: nominal signal */
	/* LED on: anomaly is detected */
	myled = 0;
	while(1) {
		fill_acc_buffer();
		similarity = NanoEdgeAI_detect(acc_buffer);
		pc.printf("%d\n", similarity + 100);
		if (similarity < 90) {
			myled = 1; /* Anomaly: turn on LED */
		} else {
			myled = 0; /* Nominal: turn off LED */
		}
	}
}
#endif

/**
 * @brief  Initialization (baud rate, accelerometer sensor, etc.)
 *
 * @param  None
 * @retval None
 */
void init()
{
	pc.baud(115200);
	init_bmi160();
#ifdef NEAI_LIB
	NanoEdgeAI_initialize();
#endif
}

/**
 * @brief  Initialization of accelerometer sensor (range, frequency, etc.)
 *
 * @param  None
 * @retval None
 */
void init_bmi160()
{
	imu.setSensorPowerMode(BMI160::ACC, BMI160::NORMAL);
	thread_sleep_for(10);
	accConfig.range = BMI160::SENS_2G; /* Accelerometer range +-2G */
	accConfig.us = BMI160::ACC_US_OFF;
	accConfig.bwp = BMI160::ACC_BWP_2;
	accConfig.odr = BMI160::ACC_ODR_11; /* Accelerometer output data rate < 800Hz */
	imu.setSensorConfig(accConfig);
	thread_sleep_for(100);
}

/**
 * @brief  Toggle the user LED state
 *
 * @param  None
 * @retval None
 */
void toggle_led(void)
{
	myled = !myled;
}

/**
 * @brief  Fill accelerometer buffer
 * acc_buffer[] = [ax0, ay0, az0, ax1, ay1, az1, ...]
 *
 * @param  None
 * @retval None
 */
void fill_acc_buffer()
{
	for (uint16_t i = 0; i < DATA_INPUT_USER; i++) {
		get_acc_values();
		acc_buffer[AXIS_NUMBER * i] = acc_x;
		acc_buffer[AXIS_NUMBER * i + 1] = acc_y;
		acc_buffer[AXIS_NUMBER * i + 2] = acc_z;
	}
#ifndef NEAI_LIB
	/* Print accelerometer buffer for data logging and neai emulator test modes */
	for (uint16_t isample = 0; isample < AXIS_NUMBER * DATA_INPUT_USER - 1; isample++) {
		pc.printf("%.4f ", acc_buffer[isample]);
	}
	pc.printf("%.4f\n", acc_buffer[AXIS_NUMBER * DATA_INPUT_USER - 1]);
#endif	
}

/**
 * @brief  Get accelerometer values (x-, y- and z-axis)
 *
 * @param  None
 * @retval None
 */
void get_acc_values()
{
	/* Polling method to get a complete buffer */
	do {
		imu.getSensorXYZ(accData, accConfig.range);
		acc_x = accData.xAxis.scaled;
		acc_y = accData.yAxis.scaled;
		acc_z = accData.zAxis.scaled;
	}
	while (acc_x == last_acc_x && acc_y == last_acc_y && acc_z == last_acc_z);
	
	last_acc_x = acc_x;
	last_acc_y = acc_y;
	last_acc_z = acc_z;
}
/* END CODE-------------------------------------------------------------------*/

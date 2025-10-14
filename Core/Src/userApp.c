/*
 * userApp.c
 *
 *  Created on: Dec 8, 2023
 *      Author: Niall.OKeeffe@atu.ie
 */

#include "userApp.h"
#include <stdio.h>

//Add the relevant FreeRTOS header files here
#include "FreeRTOS.h"
#include "task.h"

//--------------------------------------------------------------
//used for real time stats, do not delete code from this section
extern TIM_HandleTypeDef htim7;
extern volatile unsigned long ulHighFrequencyTimerTicks;
void configureTimerForRunTimeStats(void)
{
    ulHighFrequencyTimerTicks = 0;
    HAL_TIM_Base_Start_IT(&htim7);
}

unsigned long getRunTimeCounterValue(void)
{
	return ulHighFrequencyTimerTicks;
}
//end of real time stats code
//----------------------------------------------------------------

extern UART_HandleTypeDef huart1;


static void task1(void * pvParameters);

// _write function used for printf
int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
	return len;
}


void userApp() {
	printf("Starting application\r\n\n");

	xTaskCreate(task1, "Task 1", 200, NULL, 2, NULL);
	vTaskStartScheduler();

	while(1) {
	}
}

void task1(void * pvParameters) {
	printf("Starting task 1\r\n\n");
	while(1) {
		printf("LED2 flash...\r\n");
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}



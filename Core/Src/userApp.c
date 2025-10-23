/*
 * userApp.c
 *
 *  Created on: Dec 8, 2023
 *      Author: Niall.OKeeffe@atu.ie
 */

#include "userApp.h"
#include <stdio.h>
#include "main.h"
//Add the relevant FreeRTOS header files here
#include "FreeRTOS.h"
#include "task.h"
#include "stm32l4xx_hal.h"

CAN_TxHeaderTypeDef   TxHeader; /* Header containing the information of the transmitted frame */
CAN_RxHeaderTypeDef   RxHeader; ; /* Header containing the information of the received frame */
uint8_t               TxData[8] = {0};  /* Buffer of the data to send */
uint8_t               RxData[8]; /* Buffer of the received data */
uint32_t              TxMailbox;  /* The number of the mail box that transmitted the Tx message */
//--------------------------------------------------------------
//used for real time stats, do not delete code from this section
extern TIM_HandleTypeDef htim7;
extern volatile unsigned long ulHighFrequencyTimerTicks;
extern CAN_HandleTypeDef hcan1;
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


//static void task1(void * pvParameters);

// _write function used for printf
int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
	return len;
}

void userApp() {
	printf("Starting application\r\n\n");

	//xTaskCreate(task1, "Task 1", 200, NULL, 2, NULL);
	//vTaskStartScheduler();
	TxHeader.StdId = 0x123;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 8;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxData[0] = 0;
	TxData[7] = 0xFF;

	while(1) {
		 TxData[0] ++; /* Increment the first byte */
			  TxData[7] --; /* Increment the last byte */

		      /* It's mandatory to look for a free Tx mail box */
			  while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0); /* Wait till a Tx mailbox is free. Using while loop instead of HAL_Delay() */
		      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
		      {
		        /* Transmission request Error */
		    	  printf("send error\r\n\n");
		        Error_Handler();
		      }
		      else
		      {
		    	  printf("message added\r\n\n");
		    	  //HAL_Delay(10);  // small delay
		    	  uint32_t fifoLevel = HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0);
		    	  printf("FIFO0 fill level: %lu\r\n", fifoLevel);

		      }

		      uint32_t fifoLevel = HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0);
		      while(fifoLevel > 0)
		      {
		          if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
		          {
		              printf("Received: ");
		              for(uint8_t i=0; i<RxHeader.DLC; i++)
		                  printf("%02X ", RxData[i]);
		              printf("\r\n");
		          }
		          fifoLevel--;
		      }

	}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get RX message */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }
  else{
  printf("Received: ");
  for (uint8_t i = 0; i < RxHeader.DLC; i++)
  {
      printf("%02X ", RxData[i]);
  }
  printf("\r\n");
  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
}
}

/*
void task1(void * pvParameters) {
	printf("Starting task 1\r\n\n");
	while(1) {
		printf("LED2 flash...\r\n");
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
*/


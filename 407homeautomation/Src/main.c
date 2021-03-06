/**
 ******************************************************************************
 * File Name          : main.c
 * Date               : 03/01/2015 13:48:23
 * Description        : Main program body
 ******************************************************************************
 *
 * COPYRIGHT(c) 2015 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#include "stm32f4_discovery.h"
#include "dht22multi.h"
#include "pwmdma.h"
#include "ili9341.h"
#include "gfx.h"
#include "lcdBaseGFX.h"
#include "DefaultFonts.h"
#include "terminal.h"
#include "homecontrol.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;

/* USER CODE BEGIN PV */
extern void DALI_StateMachineWrite();
extern uint16_t DHT22_CRCERROR;
extern uint16_t DHT22_TIMINGERROR;
#define I2C_ADDRESS        0x55
uint8_t aTxBuffer[] = { 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t bTxBuffer[] = { 0x00, 0x00, 0x00, 0x00 };
CAN_FilterConfTypeDef sFilterConfig;
static CanTxMsgTypeDef TxMessage;
static CanRxMsgTypeDef RxMessage;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM6_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
uint16_t values[] = { 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000,
		0x8000 };

static void Error_Handler(void) {
	/* Turn LED3 on */
	BSP_LED_On(LED_RED);
	while (1) {
	}
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan) {
	if (hcan1.pRxMsg->StdId != 0x321) {
		Error_Handler();
	}

	if (hcan1.pRxMsg->IDE != CAN_ID_STD) {
		Error_Handler();
	}

	if (hcan1.pRxMsg->DLC != 4) {
		Error_Handler();
	}

	if (hcan1.pRxMsg->Data[0] == 0xFF) {
		BSP_LED_On(LED_GREEN);
	} else {
		BSP_LED_Off(LED_GREEN);
	}
	/* Receive */
	hcan->Lock = HAL_UNLOCKED;
	  if(HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK)
	  {
	    /* Reception Error */
	    Error_Handler();
	  }
}

HAL_StatusTypeDef CAN_Polling(void) {
	CAN_FilterConfTypeDef sFilterConfig;
	hcan1.pTxMsg = &TxMessage;
	hcan1.pRxMsg = &RxMessage;

	/* Set CAN error code to none */
	hcan1.ErrorCode = HAL_CAN_ERROR_NONE;
	/* Initialize the CAN state */
	hcan1.State = HAL_CAN_STATE_READY;

//  if(HAL_CAN_Init(&hcan1) != HAL_OK)
//  {
//    /* Initialization Error */
//    Error_Handler();
//  }

	/*##-2- Configure the CAN Filter ###########################################*/
	sFilterConfig.FilterNumber = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = 0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.BankNumber = 14;

	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK) {
		/* Filter configuration Error */
		Error_Handler();
	}

	/*##-3- Start the Transmission process #####################################*/
	hcan1.pTxMsg->StdId = 0x321;
	hcan1.pTxMsg->ExtId = 0x01;
	hcan1.pTxMsg->RTR = CAN_RTR_DATA;
	hcan1.pTxMsg->IDE = CAN_ID_STD;
	hcan1.pTxMsg->DLC = 4;

	if (HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0) != HAL_OK) {
		/* Reception Error */
		Error_Handler();
	}

	while (1) {
		hcan1.pTxMsg->Data[0] = hcan1.pTxMsg->Data[1] = hcan1.pTxMsg->Data[2] =
				hcan1.pTxMsg->Data[3] = 0xFF;
		if (HAL_CAN_Transmit(&hcan1, 10) != HAL_OK) {
			/* Transmition Error */
			Error_Handler();
		}
		HAL_Delay(500);
		hcan1.pTxMsg->Data[0] = hcan1.pTxMsg->Data[1] = hcan1.pTxMsg->Data[2] =
				hcan1.pTxMsg->Data[3] = 0x00;
		if (HAL_CAN_Transmit(&hcan1, 10) != HAL_OK) {
			/* Transmition Error */
			Error_Handler();
		}
		HAL_Delay(500);
	}

	return HAL_OK; /* Test Passed */
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		DHT22MULTI_StateMachineTimIT();
	}
	if (htim->Instance == TIM4) {
		DALI_StateMachineWrite();
	}
}
/* USER CODE END 0 */

int main(void) {

	/* USER CODE BEGIN 1 */
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET); //RELEASE
	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_CAN1_Init();
	MX_I2C1_Init();
	MX_SPI2_Init();
	MX_TIM4_Init();
	MX_TIM6_Init();

	/* USER CODE BEGIN 2 */
	//PWMDMA_Init();
	//DHT22MULTI_Init();
	//HAL_Delay(3000);
	//int16_t tempera = DHT22MULTI_GetTempInTenthDegrees(0);
	//ILI9341_Init(LCD_Orientation_Portrait);
	//GFX_FillScreen(ILI9341_COLOR_BLUE);
	/*
	 TERMINAL_Init(BigFont, ILI9341_COLOR_GREEN, ILI9341_COLOR_BLACK, LCD_Orientation_Landscape);
	 int i;
	 for(i=0;i<10;i++){
	 printf("Hallo %d\n", i);
	 }
	 */

	if (CAN_Polling() == HAL_OK) { /* OK */
		BSP_LED_On(LED_GREEN);
	} else { /* KO */
		BSP_LED_On(LED_ORANGE);
	}
	while (1) {
	};

	HOMECONTROL_Eternity();
	/* USER CODE END 2 */

	/* USER CODE BEGIN 3 */
	/* Infinite loop */
	int cnt = 0;
	while (1) {
		HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) I2C_ADDRESS,
				(uint8_t*) aTxBuffer, 4, 10000);
		HAL_Delay(300);
		HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) I2C_ADDRESS,
				(uint8_t*) bTxBuffer, 4, 10000);
		HAL_Delay(300);

		/*
		 for(cnt=0;cnt<100;cnt++){
		 PWMDMA_SetPWM(12, cnt);
		 HAL_Delay(20);
		 }
		 for(cnt=99;cnt>=0;cnt--){
		 PWMDMA_SetPWM(12, cnt);
		 HAL_Delay(20);
		 }
		 */
		/*
		 if(DHT22_CRCERROR || DHT22_TIMINGERROR){
		 while(1){}
		 }
		 */
	}
	/* USER CODE END 3 */

}

/** System Clock Configuration
 */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	__PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1
			| RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

}

/* CAN1 init function */
void MX_CAN1_Init(void) {

	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 21;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SJW = CAN_SJW_1TQ;
	hcan1.Init.BS1 = CAN_BS1_13TQ;
	hcan1.Init.BS2 = CAN_BS2_2TQ;
	hcan1.Init.TTCM = DISABLE;
	hcan1.Init.ABOM = DISABLE;
	hcan1.Init.AWUM = DISABLE;
	hcan1.Init.NART = DISABLE;
	hcan1.Init.RFLM = DISABLE;
	hcan1.Init.TXFP = DISABLE;
	HAL_CAN_Init(&hcan1);

}

/* I2C1 init function */
void MX_I2C1_Init(void) {

	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	HAL_I2C_Init(&hi2c1);

}

/* SPI2 init function */
void MX_SPI2_Init(void) {

	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLED;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	HAL_SPI_Init(&hspi2);

}

/* TIM4 init function */
void MX_TIM4_Init(void) {

	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_IC_InitTypeDef sConfigIC;

	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 83;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 1000;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&htim4);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig);

	HAL_TIM_IC_Init(&htim4);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig);

	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_2);

}

/* TIM6 init function */
void MX_TIM6_Init(void) {

	TIM_MasterConfigTypeDef sMasterConfig;

	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 1344;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 62500;
	HAL_TIM_Base_Init(&htim6);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);

}

/** Configure pins as 
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 PC3   ------> I2S2_SD
 PA4   ------> I2S3_WS
 PA5   ------> SPI1_SCK
 PA6   ------> SPI1_MISO
 PA7   ------> SPI1_MOSI
 PB10   ------> I2S2_CK
 PC7   ------> I2S3_MCK
 PA9   ------> USB_OTG_FS_VBUS
 PA10   ------> USB_OTG_FS_ID
 PA11   ------> USB_OTG_FS_DM
 PA12   ------> USB_OTG_FS_DP
 PC10   ------> I2S3_CK
 PC12   ------> I2S3_SD
 */
void MX_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__GPIOE_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOH_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();

	/*Configure GPIO pin : PE3 */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : PC0 */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PC3 */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PA0 */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PA1 PA2 PA3 */
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PA4 */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PA5 PA6 PA7 */
	GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PC4 */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PB2 */
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : PB10 */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PB11 PB12 PB14 */
	GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PD12 PD13 PD14 PD15
	 PD4 */
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
			| GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pins : PC7 PC10 PC12 */
	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PA9 */
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PA10 PA11 PA12 */
	GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PD5 */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin : PB8 */
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : PE1 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

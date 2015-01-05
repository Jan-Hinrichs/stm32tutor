#include "stm32f10x.h"
#include "stm32f10x_it.h"

extern CanRxMsg RxMessage;

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void) {
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void) {
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void) {
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void) {
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void) {
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void) {
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void) {
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void) {
}


/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
 * @brief  This function handles CAN1 Handler.
 * @param  None
 * @retval None
 */

void USB_LP_CAN1_RX0_IRQHandler(void) {
	static uint8_t *tRxBuffer;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	if ((RxMessage.StdId == 0x321) && (RxMessage.IDE == CAN_ID_STD)
			&& (RxMessage.DLC == 4)) {
		tRxBuffer = RxMessage.Data;
		//0. Byte:
		//b10-15, a8-9

		//1. Byte
		//a10-12, a15, b4-7, b3 wird ausgelassen und auf die "andere Seite" geführt

		//2. Byte
		//c13-15 a0-a4

		//3. Byte
		//a5-a7, b0-b3 (hier nur 7 elemente)

		//SCL b8, sda B9

		//Bit 6 und 7 müssen an Position 8 und 9
		//0..2 an 10..12 und 3 an 15
		//3..7 an 0..4
		//0..2 an 5..7
		GPIOA->ODR = ((tRxBuffer[0] & 0xC0) << 2)
				| ((tRxBuffer[1] & 0x07) << 10)
				| ((tRxBuffer[1] & 0x08) << 12)
				| ((tRxBuffer[2] & 0xF8) >> 3)
				| ((tRxBuffer[3] & 0x07) << 5);

		//0..5 an 10..15
		//4..7 an 4..7
		//---
		//3..6 an 0..3
		GPIOB->ODR = ((tRxBuffer[0] & 0x3F) << 10)
				| ((tRxBuffer[1] & 0xF0) << 0)
				| ((tRxBuffer[3] & 0x78) >> 3);

		//---
		//---
		//---
		//0..2 an 13..15
		GPIOC->ODR = ((tRxBuffer[2] & 0x07) << 13);

		/* Reinitialize RXBuffer */
		tRxBuffer[0] = tRxBuffer[1] = tRxBuffer[2] = tRxBuffer[3] = 0;
	}
}


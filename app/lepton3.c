#include "main.h"
#include <string.h>
#include <stdio.h>

#define MAX_LINES 	300

static int dma_xfer = 0;
static int dma_complete = 0;
static uint8_t buf[164 * MAX_LINES];

void show_error(int count)
{
	for (int i = 0; i < count; i++)
	{
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		HAL_Delay(100);
	}
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	dma_xfer++;
	dma_complete = 1;
}


int run(SPI_HandleTypeDef * hspi, UART_HandleTypeDef * huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	int xfers = 0;
	GPIO_PinState prev_button = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

	//HAL_SPI_RegisterCallback(hspi, HAL_SPI_TX_RX_COMPLETE_CB_ID, TxRxCpltCallback);

	while (1)
	{
		GPIO_PinState button = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
		if ((button == GPIO_PIN_SET) && (prev_button == GPIO_PIN_RESET))
		{
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
			// Low to high transition take a picture
			HAL_StatusTypeDef status;
			xfers = 0;
			while (xfers < MAX_LINES)
			{
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
			  //if (HAL_SPI_Receive(hspi, packet, 164, 100) != HAL_OK)
			  dma_complete = 0;
			  status = HAL_SPI_Receive_DMA(hspi, &buf[164*xfers], 164);
			  //status = HAL_SPI_Receive_DMA(hspi, packet, 164);
			  //status = HAL_SPI_Receive_IT(hspi, packet, 164);
			  if (status != HAL_OK)
			  {
				  show_error(5);
			  }
			  while (dma_complete == 0) {};

			  uint16_t packet_num = ((uint16_t)buf[164*xfers+0] << 8) | buf[164*xfers+1];
			  if ((packet_num & 0x0F00) != 0x0F00)
			  {
				  xfers++;
			  }
			}
			// Transfer all of the data
			if (HAL_UART_Transmit(huart, buf, 164*MAX_LINES, 5000) != HAL_OK)
			{
				show_error(10);
			}
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		}
		prev_button = button;
	}
}

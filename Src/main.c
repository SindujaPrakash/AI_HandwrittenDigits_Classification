/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_lcd_bsp.h"
#include "touch_panel.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

SPI_HandleTypeDef hspi3;

UART_HandleTypeDef huart1;

SRAM_HandleTypeDef hsram1;

/* USER CODE BEGIN PV */
uint16_t original_image[168*168];
uint8_t grayscale_image[28*28];
uint16_t resized_image[28*28];
uint16_t *ptToImage = &original_image;
uint16_t drawMenuNeeded = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_FSMC_Init(void);
static void MX_SPI3_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void loadImageIntoMemory();
void convert_8Bit_Grayscale(uint16_t* buffer);
void ImageResize(uint8_t *srcImage, uint32_t srcW, uint32_t srcH,
                 uint32_t pixelSize, uint32_t roiX, uint32_t roiY,
                 uint32_t roiW, uint32_t roiH,  uint8_t *dstImage,
                 uint32_t dstW, uint32_t dstH);
void drawMenu();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint16_t linenum = 0;
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_FSMC_Init();
  MX_SPI3_Init();
  MX_USART1_UART_Init();
  MX_X_CUBE_AI_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize the LCD */
  BSP_LCD_Init();
    /* Enable the LCD */
  BSP_LCD_DisplayOn();
    /* Touch Panel configuration */
  printf("LCD is on");
  TouchPanel_Calibrate();
  drawMenu();
  printf("touch calibrated");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */

	  	  	getDisplayPoint(&display, Read_Ads7846(), &matrix );
	  	    if(((display.y < 190) && (display.y >= 2)))
	  	    {
	  	      if((display.x >= 318) || (display.x < 2))
	  	      {}
	  	      else
	  	      {
	  	        BSP_LCD_FillCircle(display.x, display.y, 2);
	  	      }
	  	    }

	  	    if((HAL_GPIO_ReadPin(Push_button_GPIO_Port, Push_button_Pin))==GPIO_PIN_RESET)
	  	    {
	  	    	loadImageIntoMemory();
	  	    	ImageResize((uint8_t*)original_image, 168, 168, 2, 0, 0, 0, 0,(uint8_t*)resized_image, 28, 28);
	  	    	convert_8Bit_Grayscale((uint16_t*)resized_image);
	  	    	MX_X_CUBE_AI_Process();
	  	    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	 RCC_OscInitTypeDef RCC_OscInitStruct;
	  RCC_ClkInitTypeDef RCC_ClkInitStruct;

	    /**Configure the main internal regulator output voltage
	    */
	  __HAL_RCC_PWR_CLK_ENABLE();

	  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	    /**Initializes the CPU, AHB and APB busses clocks
	    */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	  RCC_OscInitStruct.HSICalibrationValue = 16;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	  RCC_OscInitStruct.PLL.PLLM = 8;
	  RCC_OscInitStruct.PLL.PLLN = 168;
	  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	  RCC_OscInitStruct.PLL.PLLQ = 4;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    Error_Handler();
	  }

	    /**Initializes the CPU, AHB and APB busses clocks
	    */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	  {
	    Error_Handler();
	  }

	    /**Configure the Systick interrupt time
	    */
	  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	    /**Configure the Systick
	    */
	  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	  /* SysTick_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
	 hspi3.Instance = SPI3;
	  hspi3.Init.Mode = SPI_MODE_MASTER;
	  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
	  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
	  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
	  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
	  hspi3.Init.NSS = SPI_NSS_SOFT;
	  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
	  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
	  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	  hspi3.Init.CRCPolynomial = 10;
	  if (HAL_SPI_Init(&hspi3) != HAL_OK)
	  {
	    Error_Handler();
	  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

	  huart1.Instance = USART1;
	  huart1.Init.BaudRate = 115200;
	  huart1.Init.WordLength = UART_WORDLENGTH_8B;
	  huart1.Init.StopBits = UART_STOPBITS_1;
	  huart1.Init.Parity = UART_PARITY_NONE;
	  huart1.Init.Mode = UART_MODE_TX_RX;
	  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	  if (HAL_UART_Init(&huart1) != HAL_OK)
	  {
	    Error_Handler();
	  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;

	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOE_CLK_ENABLE();
	  __HAL_RCC_GPIOF_CLK_ENABLE();
	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOD_CLK_ENABLE();
	  __HAL_RCC_GPIOG_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(A23_GPIO_Port, A23_Pin, GPIO_PIN_SET);

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(BL_PWM_GPIO_Port, BL_PWM_Pin, GPIO_PIN_SET);

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(LCDTP_CS_GPIO_Port, LCDTP_CS_Pin, GPIO_PIN_RESET);

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(A24_GPIO_Port, A24_Pin, GPIO_PIN_SET);

	  /*Configure GPIO pin : PtPin */
	  GPIO_InitStruct.Pin = A23_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(A23_GPIO_Port, &GPIO_InitStruct);

	  /*Configure GPIO pin : PtPin */
	  GPIO_InitStruct.Pin = BL_PWM_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(BL_PWM_GPIO_Port, &GPIO_InitStruct);

	  /*Configure GPIO pin : PtPin */
	  GPIO_InitStruct.Pin = LCDTP_IRQ_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(LCDTP_IRQ_GPIO_Port, &GPIO_InitStruct);

	  /*Configure GPIO pin : PtPin */
	  GPIO_InitStruct.Pin = LCDTP_CS_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(LCDTP_CS_GPIO_Port, &GPIO_InitStruct);

	  /*Configure GPIO pins : PGPin PGPin */
	  GPIO_InitStruct.Pin = LCD_CS_Pin|A24_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* FSMC initialization function */
static void MX_FSMC_Init(void)
{

  /* USER CODE BEGIN FSMC_Init 0 */

  /* USER CODE END FSMC_Init 0 */

	  FSMC_NORSRAM_TimingTypeDef Timing;

	  /** Perform the SRAM1 memory initialization sequence
	  */
	  hsram1.Instance = FSMC_NORSRAM_DEVICE;
	  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
	  /* hsram1.Init */
	  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
	  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
	  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
	  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
	  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
	  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
	  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
	  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
	  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
	  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
	  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
	  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
	  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
	  hsram1.Init.PageSize = FSMC_PAGE_SIZE_NONE;
	  /* Timing */
	  Timing.AddressSetupTime = 4;
	  Timing.AddressHoldTime = 15;
	  Timing.DataSetupTime = 40;
	  Timing.BusTurnAroundDuration = 0;
	  Timing.CLKDivision = 16;
	  Timing.DataLatency = 17;
	  Timing.AccessMode = FSMC_ACCESS_MODE_A;
	  /* ExtTiming */

	  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
	  {
	    Error_Handler();
	  }

  /* USER CODE BEGIN FSMC_Init 2 */

  /* USER CODE END FSMC_Init 2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* write a character to the uart1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

/**
  * @brief  Performs image (or selected Region Of Interest) resizing using bilinear interpolation
  * @param  srcImage     Pointer to source image buffer
  * @param  srcW         Source image width
  * @param  srcH         Source image height
  * @param  pixelSize    Number of bytes per pixel
  * @param  roiX         Region Of Interest x starting location
  * @param  roiY         Region Of Interest y starting location
  * @param  roiW         Region Of Interest width
  * @param  roiH         Region Of Interest height
  * @param  dstImage     Pointer to destination image buffer
  * @param  dstW         Destination image width
  * @param  dstH         Destination image height
  * @retval void         None
  */
void ImageResize(uint8_t *srcImage, uint32_t srcW, uint32_t srcH,
                 uint32_t pixelSize, uint32_t roiX, uint32_t roiY,
                 uint32_t roiW, uint32_t roiH,  uint8_t *dstImage,
                 uint32_t dstW, uint32_t dstH)
{
  int32_t srcStride;
  float widthRatio;
  float heightRatio;

  int32_t maxWidth;
  int32_t maxHeight;

  float srcX, srcY, dX1, dY1, dX2, dY2;
  int32_t dstX1, srcY1, dstX2, srcY2;

  uint8_t *tmp1, *tmp2;
  uint8_t *p1, *p2, *p3, *p4;

  int32_t offset1;
  int32_t offset2;

  srcStride = pixelSize * srcW;

  widthRatio = ((roiW ? roiW : srcW) / (float) dstW);
  heightRatio = ((roiH ? roiH : srcH) / (float) dstH);

  /* Get horizontal and vertical limits. */
  maxWidth = (roiW ? roiW : srcW) - 1;
  maxHeight = (roiH ? roiH : srcH) - 1;

  for (int32_t y = 0; y < dstH; y++)
  {
    /* Get Y from source. */
    srcY = ((float) y * heightRatio) + roiY;
    srcY1 = (int32_t) srcY;
    srcY2 = (srcY1 == maxHeight) ? srcY1 : srcY1 + 1;
    dY1 = srcY - (float) srcY1;
    dY2 = 1.0f - dY1;

    /* Calculates the pointers to the two needed lines of the source. */
    tmp1 = srcImage + srcY1 * srcStride;
    tmp2 = srcImage + srcY2 * srcStride;

    for (int32_t x = 0; x < dstW; x++)
    {
      /* Get X from source. */
      srcX = x * widthRatio + roiX;
      dstX1 = (int32_t) srcX;
      dstX2 = (dstX1 == maxWidth) ? dstX1 : dstX1 + 1;
      dX1 = srcX - /*(float32)*/dstX1;
      dX2 = 1.0f - dX1;

      /* Calculates the four points (p1,p2, p3, p4) of the source. */
      offset1 = dstX1 * pixelSize;
      offset2 = dstX2 * pixelSize;
      p1 = tmp1 + offset1;
      p2 = tmp1 + offset2;
      p3 = tmp2 + offset1;
      p4 = tmp2 + offset2;
      /* For each channel, interpolate the four points. */
      for (int32_t ch = 0; ch < pixelSize; ch++, dstImage++, p1++, p2++, p3++, p4++)
      {
        *dstImage = (uint8_t)(dY2 * (dX2 * (*p1) + dX1 * (*p2)) + dY1 * (dX2 * (*p3) + dX1 * (*p4)));
      }
    }
  }
}

// Convert to 8 bit grayscale
void convert_8Bit_Grayscale(uint16_t* buffer)
{
	for(int16_t i=0; i< 784; i++)
		      {
		   	   uint16_t pixel = *buffer;
		   	   uint8_t red = ((pixel & 0xF800)>>11);
		   	   uint8_t green = ((pixel & 0x07E0)>>5);
		   	   uint8_t blue = (pixel & 0x001F);
		   	   grayscale_image[i] = (0.2126 * red) + (0.7152 * green / 2.0) + (0.0722 * blue);
		   	   if(grayscale_image[i] == 0x00)
		   		   grayscale_image[i] = 0XFF;
		   	   else if(grayscale_image[i]==0x1F)
		   		   grayscale_image[i]=0X00;
		   	   buffer++;
		      }
}

// Draw the Menu screen
void drawMenu()
{
	  BSP_LCD_Clear(LCD_COLOR_WHITE);
	  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	  BSP_LCD_DrawHLine(50,30,170);
	  BSP_LCD_DrawHLine(50,200,170);
	  BSP_LCD_DrawVLine(50, 30, 170);
	  BSP_LCD_DrawVLine(220, 30, 170);
	  BSP_LCD_SetFont(&Font24);
}

// Load LCD Pixel values to Memory
void loadImageIntoMemory()
{
	for(uint16_t i=31; i<199; i++)
	{
		for(uint16_t j=51;j<219;j++)
		{
			*ptToImage = ili9325_ReadPixel(j, i);
			ptToImage++;
		}
	}
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font24);
	ili9325_SetCursor(0, 0);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Target.h"
#include "led.h"
#include "buzzer.h"
#include "w25qxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
W25QXX_HandleTypeDef w25qxx; // Handler for all w25qxx operations!

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//#define DEBUG
#ifdef DEBUG
#define DBG(...)    printf(__VA_ARGS__)
#else
#define DBG(...)
#endif
#define PAGE_SIZE 4096
// Dump hex to serial console
void dump_hex(char *header, uint32_t start, uint8_t *buf, uint32_t len) {
    uint32_t i = 0;

    printf("%s\n", header);

    for (i = 0; i < len; ++i) {

        if (i % 16 == 0) {
            printf("0x%08lx: ", start);
        }

        printf("%02x ", buf[i]);

        if ((i + 1) % 16 == 0) {
            printf("\n");
        }

        ++start;
    }
}

void fill_buffer(uint8_t pattern, uint8_t *buf, uint32_t len) {
    switch (pattern) {
    case 0:
        memset(buf, 0, len);
        break;
    case 1:
        memset(buf, 0xaa, len); // 10101010
        break;
    case 2:
        for (uint32_t i = 0; i < len; ++i)
            buf[i] = i % 256;
        break;
    default:
        DBG("Programmer is a moron");
    }
}

uint8_t check_buffer(uint8_t pattern, uint8_t *buf, uint32_t len) {

    uint8_t ret = 1;

    switch (pattern) {
    case 0:
        for (uint32_t i = 0; i < len; ++i) {
            if (buf[i] != 0)
                ret = 0;
        }
        break;
    case 1:
        for (uint32_t i = 0; i < len; ++i) {
            if (buf[i] != 0xaa)
                ret = 0;
        }
        break;
    case 2:
        for (uint32_t i = 0; i < len; ++i) {
            if (buf[i] != i % 256)
                ret = 0;
        }
        break;
    default:
        DBG("Programmer is a moron");
    }

    return ret;
}

uint32_t get_sum(uint8_t *buf, uint32_t len) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < len; ++i) {
        sum += buf[i];
    }
    return sum;
}

void w25_test(void)
{
	W25QXX_result_t res;

  HAL_Delay(100); // Wait a bit to make sure the w25qxx is ready

  res = w25qxx_init(&w25qxx, &hspi2, SPI_CS_GPIO_Port, SPI_CS_Pin);
  if (res == W25QXX_Ok) {
      printf("W25QXX successfully initialized\n");
      printf("Manufacturer       = 0x%2x\n", w25qxx.manufacturer_id);
      printf("Device             = 0x%4x\n", w25qxx.device_id);
      printf("Block size         = 0x%04lx (%lu)\n", w25qxx.block_size, w25qxx.block_size);
      printf("Block count        = 0x%04lx (%lu)\n", w25qxx.block_count, w25qxx.block_count);
      printf("Sector size        = 0x%04lx (%lu)\n", w25qxx.sector_size, w25qxx.sector_size);
      printf("Sectors per block  = 0x%04lx (%lu)\n", w25qxx.sectors_in_block, w25qxx.sectors_in_block);
      printf("Page size          = 0x%04lx (%lu)\n", w25qxx.page_size, w25qxx.page_size);
      printf("Pages per sector   = 0x%04lx (%lu)\n", w25qxx.pages_in_sector, w25qxx.pages_in_sector);
      printf("Total size (in kB) = 0x%04lx (%lu)\n", (w25qxx.block_count * w25qxx.block_size) / 1024, (w25qxx.block_count * w25qxx.block_size) / 1024);
  } else {
  	printf("Unable to initialize w25qxx\n");
  }

  HAL_Delay(2000);

  uint8_t buf[PAGE_SIZE]; // Buffer the size of a page

  for (uint8_t run = 0; run <= 2; ++run) {

      DBG("\n-------------\nRun %d\n", run);

      DBG("Reading first page");

      res = w25qxx_read(&w25qxx, 0, (uint8_t*) &buf, sizeof(buf));
      if (res == W25QXX_Ok) {
          dump_hex("First page at start", 0, (uint8_t*) &buf, sizeof(buf));
      } else {
          DBG("Unable to read w25qxx\n");
      }

      DBG("Erasing first page");
      if (w25qxx_erase(&w25qxx, 0, sizeof(buf)) == W25QXX_Ok) {
          DBG("Reading first page\n");
          if (w25qxx_read(&w25qxx, 0, (uint8_t*) &buf, sizeof(buf)) == W25QXX_Ok) {
              dump_hex("After erase", 0, (uint8_t*) &buf, sizeof(buf));
          }
      }

      // Create a well known pattern
      fill_buffer(run, buf, sizeof(buf));

      // Write it to device
      DBG("Writing first page\n");
      if (w25qxx_write(&w25qxx, 0, (uint8_t*) &buf, sizeof(buf)) == W25QXX_Ok) {
          // now read it back
          DBG("Reading first page\n");
          if (w25qxx_read(&w25qxx, 0, (uint8_t*) &buf, sizeof(buf)) == W25QXX_Ok) {
              //DBG("  - sum = %lu", get_sum(buf, 256));
              dump_hex("After write", 0, (uint8_t*) &buf, sizeof(buf));
          }
      }
  }

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_DMA_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

	/* SWD DAP usDelay(): TIM1 */
	HAL_TIM_Base_Start(&htim1);

	/* buzzer sound timer pwm start: TIM2 */
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

	/* Set programmer LED & Buzzer state */
	Target_LedSet(TARGET_LED_STAT_BOOT);
	Buzzer_SetState(BUZZER_BOOT);

	/* w25 serial flash test */
	//w25_test();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  	Target_MainLoop();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_4B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* I don't know why... */
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;

  /* USER CODE END SDIO_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 120-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 59;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
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
  * Enable DMA controller clock
  */
void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SWD_IO_Pin|SWD_CLK_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, LED_RED_Pin|LED_ORANGE_Pin|LED_GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SPI_CS_Pin|TARGET_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SWD_IO_Pin SWD_CLK_Pin */
  GPIO_InitStruct.Pin = SWD_IO_Pin|SWD_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_RED_Pin LED_ORANGE_Pin LED_GREEN_Pin */
  GPIO_InitStruct.Pin = LED_RED_Pin|LED_ORANGE_Pin|LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI_CS_Pin TARGET_RST_Pin */
  GPIO_InitStruct.Pin = SPI_CS_Pin|TARGET_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PROGRAM_BTN_Pin */
  GPIO_InitStruct.Pin = PROGRAM_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PROGRAM_BTN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PI13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : OPTION_PROG_DIP_SW_Pin */
  GPIO_InitStruct.Pin = OPTION_PROG_DIP_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OPTION_PROG_DIP_SW_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	printf("Error_Handle()\n");
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

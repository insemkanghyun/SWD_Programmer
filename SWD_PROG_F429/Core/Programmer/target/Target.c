/*
 * Target.c
 *
 *  Created on: Jul 5, 2024
 *      Author: kangh
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "Target.h"
#include "fatfs.h"
#include "swd\dap.h"
#include "swd\errors.h"
#include "swd\utils.h"
#include "swd\delay.h"
#include "util\ihex_parser.h"
#include "stm32c0_flash.h"

#define PRINTF_REDIRECTION	int __io_putchar(int ch)
Target_InfoTypeDef target;
volatile uint8_t u8_ButtonPushed = 0;
extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart1;
static void Target_Classify(Target_InfoTypeDef *target);
bool Target_ProgramCallback_STM32C0(uint32_t addr, const uint8_t *buf, uint8_t bufsize);

void Target_Main(void)
{
	HAL_TIM_Base_Start(&htim1);

  if(u8_ButtonPushed)
  {
  	u8_ButtonPushed = 0;
    Target_Probe();
    Target_MassErase();
    Target_Program();
  }
}


void Target_Probe(void)
{
  uint32_t	retry = CONNECT_RETRY_COUNT;
  printf("SELECT: Connect\n");
  hardResetTarget();
  delayMs(50);

  /* Try connecting several times */
	while ( retry-- > 0 )
  {
		TRY
			connectToTarget(&target);
			Target_Classify(&target);
    	/* Stop retrying */
    	return;
		CATCH
    	printf("SWD Error: %s\n", getErrorString(errorCode));
    	printf("Failed to connect. Retrying...\n");
    	delayUs(200);
    ENDTRY
  }
  printf("Failed to connect after %d retries\n", CONNECT_RETRY_COUNT);
  Error_Handler();
}

void Target_Program(void)
{
		uint8_t fbuf[256];
    size_t readcount = 0;
    FRESULT res;
    FIL HexFile;
    ihex_set_callback_func((ihex_callback_fp)*Target_ProgramCallback_STM32C0);

    res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
    if(res != FR_OK)
    {
    	printf("Error - f_mount()\n");
    	Error_Handler();
    }

    res =  f_open(&HexFile, FIRMWARE_FILENAME, FA_READ);
    if(res != FR_OK)
    {
    	printf("Error - f_open()\n");
    	Error_Handler();
    }

    while (1)
    {
    	res = f_read(&HexFile, fbuf, sizeof(fbuf), &readcount);
      if(res != FR_OK)
      {
      	printf("Error - f_read()\n");
      	Error_Handler();
      }

    	if(readcount ==  0)
    	{
    		f_close(&HexFile);
    		break;
    	}
    	else
    	{
    		if(readcount < sizeof(fbuf))
    		{
    			fbuf[readcount] = '\0';     // Add null teminated char
    		}
    		if (!ihex_parser(fbuf, sizeof(fbuf)))
    		{
    				printf("Error - ihex_parser()\n");
    				Error_Handler();
    		}
    	}
    }
}

void Target_MassErase(void)
{
	switch(target.TargetFamily)
	{
		case TARGET_STM32C0:
			Stm32c0_Flash_Unlock();
			Stm32c0_Flash_MassErase();
			Stm32c0_Flash_Lock();
			break;
		default:
			break;
	}
}





static void Target_Classify(Target_InfoTypeDef *target)
{
	uint32_t tmp = 0;

	/* Target Family Classify & Read Device ID */
	if(target->TargetDpId == STM32C0_SWDP_ID)
	{
		target->TargetFamily = TARGET_STM32C0;
		tmp = readMem(STM32C0_REG_DEVICE_ID);

	}
	else if(target->TargetDpId == STM32H7_SWDP_ID)
	{
		target->TargetFamily = TARGET_STM32H7;
		tmp = readMem(STM32H7_REG_DEVICE_ID);
	}
	else
	{
		/* Do Nothing */
	}

	target->TargetDevId = tmp & 0xFFF;
	target->TargetRevId = tmp >> 16;

	switch(target->TargetDevId)
	{
		case STM32C0_DEV_ID1:
			printf("STM32C011xx\n");
			break;
		case STM32C0_DEV_ID2:
			printf("STM32C031xx\n");
			break;
		case STM32H7_DEV_ID1:
			printf("STM32H7Rx/7Sx\n");
			break;
		case STM32H7_DEV_ID2:
			printf("STM32H7A3/7B3/7B0\n");
			break;
		case STM32H7_DEV_ID3:
			printf("STM32H72x, STM32H73x\n");
			break;
		case STM32H7_DEV_ID4:
			printf("STM32H742, STM32H743/753 and STM32H750\n");
			printf("STM32H745/755 and STM32H747/757\n");
			break;
		default:
			printf("Unknown.\n");
			break;
	}

}

bool Target_ProgramCallback_STM32C0(uint32_t addr, const uint8_t *buf, uint8_t bufsize)
{
	uint64_t tmp[2];
	Target_StatusTypeDef status = 0;

	memset(&tmp, 0xFF, sizeof(uint64_t));

  for (int i = 0; i < 2; i++) {
  	tmp[i] = ((uint64_t*)buf)[i];
  }

  Stm32c0_Flash_Unlock();

  for(uint32_t i = 0; i < 2; i++)
	{
		status = Stm32c0_Flash_Program(addr + (i*8), tmp[i]);
		if(bufsize < 9)
			break;
	}

  Stm32c0_Flash_Lock();

	if(status == TARGET_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Target_Verfify(void)
{

}

PRINTF_REDIRECTION
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}


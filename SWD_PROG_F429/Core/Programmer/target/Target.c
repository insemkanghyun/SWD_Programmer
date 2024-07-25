#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Target.h"
#include "fatfs.h"
#include "swd\dap.h"
#include "swd\errors.h"
#include "swd\utils.h"
#include "swd\delay.h"
#include "util\ihex_parser.h"
#include "stm32c0_flash.h"
#include "led.h"

#define PRINTF_REDIRECTION	int __io_putchar(int ch)
#define TO_BE_IMPLEMENT_CALLBACK 0

Target_InfoTypeDef target;
volatile uint8_t u8_ButtonPushed = 0;
volatile uint8_t u8_ButtonLock = 0;
int u32_StartTime = 0;
int u32_ElasedTime = 0;

extern UART_HandleTypeDef huart1;
static void Target_Classify(Target_InfoTypeDef *target);


/* iHex_Parser Callback */
bool Target_VerifyCallback(uint32_t addr, const uint8_t *buf, uint8_t bufsize);
bool Target_ProgramCallback_STM32C0(uint32_t addr, const uint8_t *buf, uint8_t bufsize);

bool (*Target_ProgramCallback[])(uint32_t addr, const uint8_t *buf, uint8_t bufsize)={\
					TO_BE_IMPLEMENT_CALLBACK,\
					TO_BE_IMPLEMENT_CALLBACK,\
					TO_BE_IMPLEMENT_CALLBACK,\
					TO_BE_IMPLEMENT_CALLBACK,\
					TO_BE_IMPLEMENT_CALLBACK,\
					Target_ProgramCallback_STM32C0};

void Target_MainLoop(void)
{
	Target_StatusTypeDef status = TARGET_ERROR;


	/* Button programming start */
  if(u8_ButtonPushed)
  {
  	Target_LedSet(TARGET_LED_STAT_PROGRAMMING);

  	u32_StartTime = HAL_GetTick();
    /* Reset button status for next programming */
  	u8_ButtonPushed = 0;
  	printf("Target Button Pushed\n");

  	/* Set button lock flag for block when target flash operation  */
  	u8_ButtonLock = 1;
  	printf("Target Button Locked\n");

  	/* Target flash operation */
  	status = Target_Connect();
  	if(status != TARGET_OK)
  	{
  		printf("Target Connect Error\n");
  		Target_LedSet(TARGET_LED_STAT_FAILED);
  		u8_ButtonLock = 0;
  		return;
  	}

  	status = Target_MassErase();
  	if(status != TARGET_OK)
  	{
  		printf("Target MassErase Error\n");
  		Target_LedSet(TARGET_LED_STAT_FAILED);
  		u8_ButtonLock = 0;
  		return;
  	}

  	status = Target_Program();
  	if(status != TARGET_OK)
  	{
  		printf("Target Program Error\n");
  		Target_LedSet(TARGET_LED_STAT_FAILED);
  		u8_ButtonLock = 0;
  		return;
  	}

  	status = Target_Verfify();
  	if(status != TARGET_OK)
  	{
  		printf("Target Verify Error\n");
  		Target_LedSet(TARGET_LED_STAT_FAILED);
  		u8_ButtonLock = 0;
  		return;
  	}
    /* Target flash operation completed & button lock flag release  */
    u8_ButtonLock = 0;
    printf("Target Button Unlocked\n");
    printf("Target program completed\n");
    u32_ElasedTime = HAL_GetTick() - u32_StartTime;
    printf("Total Elapsed Programming Time: %d ms\n", u32_ElasedTime);
    Target_LedSet(TARGET_LED_STAT_COMPLETE);
  }
}

Target_StatusTypeDef Target_Connect(void)
{
  uint32_t	retry = CONNECT_RETRY_COUNT;

  printf("Target Connect\n");
  hardResetTarget();
  delayMs(50);

  /* Try connecting several times */
	while ( retry-- > 0 )
  {
		TRY
			connectToTarget(&target);
			Target_Classify(&target);

			/* Reset swdErrorIndex, it cause f_open(fatfs) hang. */
			swdErrorIndex = 0;

    	/* Stop retrying */
    	return TARGET_OK;
		CATCH
    	printf("SWD Error: %s\n", getErrorString(errorCode));
    	printf("Failed to connect. Retrying...\n");
    	//delayUs(200);
    	delayMs(200);
    ENDTRY
  }

  printf("Target Probe Error\n");
  return TARGET_ERROR;
}

Target_StatusTypeDef Target_Program(void)
{
		uint8_t fbuf[256];
    size_t readcount = 0;
    FRESULT res;
    FIL HexFile;

    printf("Target Program\n");

    /* Hex parser callback registeration */
    ihex_set_callback_func((ihex_callback_fp)*Target_ProgramCallback[target.TargetFamily]);

    /* File open */
    res =  f_open(&HexFile, FIRMWARE_FILENAME, FA_READ);
    if(res != FR_OK)
    {
    	printf("f_open error\n");
    	return TARGET_ERROR;
    }

    while (1)
    {
    	/* File read */
    	res = f_read(&HexFile, fbuf, sizeof(fbuf), &readcount);
      if(res != FR_OK)
      {
      	printf("f_read error\n");
      	return TARGET_ERROR;
      }

      /* If there is no data to read, end programming loop*/
    	if(readcount ==  0)
    	{
    		res = f_close(&HexFile);
        if(res != FR_OK)
        {
        	printf("f_close error\n");
        	return TARGET_ERROR;
        }
    		break;
    	}
    	/* If data is available, hex parsing & swd flash write */
    	else
    	{
    		/* If readcount smaller then sizeof(buf), add null termination character end of buffer */
    		if(readcount < sizeof(fbuf))
    		{
    			fbuf[readcount] = '\0';
    		}

    		/* Hex parsing & swd flash write */
    		if (!ihex_parser(fbuf, sizeof(fbuf)))
    		{
    			printf("ihex_parser error\n");
    			return TARGET_ERROR;
    		}
    	}
   }
    return TARGET_OK;
}

Target_StatusTypeDef Target_MassErase(void)
{
	printf("Target MassErase\n");
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
	return TARGET_OK;
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

	printf("Target Device is ");
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
			Error_Handler();
			break;
	}
}

bool Target_ProgramCallback_STM32C0(uint32_t addr, const uint8_t *buf, uint8_t bufsize)
{
	uint64_t tmp[2];
	Target_StatusTypeDef status = TARGET_OK;

	/* Convert Hex parsed data (uint8_t, Byte) to uint64_t(Double Word) */
	/* STM32C0 Flash Double Word Program Support(Standard) */
  for (int i = 0; i < 2; i++) {
  	tmp[i] = ((uint64_t*)buf)[i];
  }

  /*Flash unlock before programming */
  status = Stm32c0_Flash_Unlock();
  if(status != TARGET_OK)
  	return false;

  /*Flash programming double word */
  for(uint32_t i = 0; i < 2; i++)
	{
		status = Stm32c0_Flash_Program(addr + (i*8), tmp[i]);
		if(status != TARGET_OK)
			return false;

		/* If bufsize only one Double word, quit loop */
		if(bufsize <= 8)
			break;
	}

  /*Flash lock after programming */
  status = Stm32c0_Flash_Lock();
	if(status != TARGET_OK)
		return false;

	return true;
}

bool Target_VerifyCallback(uint32_t addr, const uint8_t *buf, uint8_t bufsize)
{
	uint8_t tmp[16];
	uint32_t u32_ReadData[4];

	/* Read 4-word from target flash memory */
	for(int i = 0; i < 4; i++)
	{
		u32_ReadData[i] = readMem(addr + (i*4));
	}

	/* Convert uint32_t to uint8_t */
  for (int i = 0; i < 4; i++) {
  	tmp[4 * i]     = u32_ReadData[i] & 0xFF;
  	tmp[4 * i + 1] = (u32_ReadData[i] >> 8) & 0xFF;
  	tmp[4 * i + 2] = (u32_ReadData[i] >> 16) & 0xFF;
  	tmp[4 * i + 3] = (u32_ReadData[i] >> 24) & 0xFF;
  }

  /* Compare Hex & Flash Data */
	for(int i = 0; i < bufsize; i++)
	{
		if(buf[i] != tmp[i])
		{
			printf("Verification failed at address 0x%08"PRIX32"\n", (addr + i));
			printf("Value is 0x%02"PRIX16", should have been 0x%02"PRIX16"\n", tmp[i], buf[i]);
			return false;
		}
	}
	return true;
}

Target_StatusTypeDef Target_Verfify(void)
{
	uint8_t fbuf[256];
  size_t readcount = 0;
  FRESULT res;
  FIL HexFile;

  printf("Target Verify\n");

  /* Hex parser callback registeration */
  ihex_set_callback_func((ihex_callback_fp)*Target_VerifyCallback);

  /* File open */
  res =  f_open(&HexFile, FIRMWARE_FILENAME, FA_READ);
  if(res != FR_OK)
  {
  	printf("f_open error\n");
  	return TARGET_ERROR;
  }

  while (1)
  {
  	/* File read */
  	res = f_read(&HexFile, fbuf, sizeof(fbuf), &readcount);
    if(res != FR_OK)
    {
    	printf("f_read error\n");
    	return TARGET_ERROR;
    }

    /* If there is no data to read, end verfify loop*/
  	if(readcount ==  0)
  	{
  		res = f_close(&HexFile);
      if(res != FR_OK)
      {
      	printf("f_close error\n");
      	return TARGET_ERROR;
      }

  		break;
  	}
  	else
  	{
  		/* If readcount smaller then sizeof(buf), add null termination character end of buffer */
  		if(readcount < sizeof(fbuf))
  		{
  			fbuf[readcount] = '\0';
  		}

  		/* Hex parsing & swd flash verify */
  		if (!ihex_parser(fbuf, sizeof(fbuf)))
  		{
  				printf("ihex_parser error\n");
  				return TARGET_ERROR;
  		}
  	}
  }
  return TARGET_OK;
}

void Target_BuutonPush(void)
{
  if(u8_ButtonLock == 0)
  {
  	u8_ButtonPushed = 1;
  }
  else
  {
  	/* Do Nothing */
  }
}


void Target_LedSet(LedStatus status)
{
	LED_SetState(status);
}

PRINTF_REDIRECTION
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}


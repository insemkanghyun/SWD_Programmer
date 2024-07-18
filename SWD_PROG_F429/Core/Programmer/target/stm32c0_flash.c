/*
 * stm32c0_flash.c
 *
 *  Created on: Jul 5, 2024
 *      Author: kangh
 */
#include <stdint.h>
#include "main.h"
#include "stm32c0_flash.h"
#include "target.h"
#include "utils.h"

Target_StatusTypeDef Stm32c0_Flash_MassErase(void)
{
	Target_StatusTypeDef status;
	uint32_t tmp = 0;
	status = Stm32c0_Flash_WaitOperation(STM32C0_FLASH_TIMEOUT_VALUE);

	if(status == TARGET_OK)
	{
	  /* Set the Mass Erase Bit and start bit */
		tmp = readMem(STM32C0_FLASH_CR);
		writeMem(STM32C0_FLASH_CR, (STM32C0_FLASH_CR_STRT | STM32C0_FLASH_CR_MER1)|tmp);
		status = Stm32c0_Flash_WaitOperation(STM32C0_FLASH_TIMEOUT_VALUE);
	}
	return  status;
}

Target_StatusTypeDef Stm32c0_Flash_Program(uint32_t Address, uint64_t Data)
{
		Target_StatusTypeDef status;
		uint32_t tmp = 0;

		/* Wait for last operation to be completed */
	  status = Stm32c0_Flash_WaitOperation(STM32C0_FLASH_TIMEOUT_VALUE);

	  if (status == TARGET_OK)
	  {
	    /* Set PG bit */
	  	tmp = readMem(STM32C0_FLASH_CR);
	  	writeMem(STM32C0_FLASH_CR, STM32C0_FLASH_CR_PG|tmp);

	  	/* Program first word */
	  	writeMem(Address, 	(uint32_t)Data);

	  	/* Program second word */
	  	writeMem(Address+4, (uint32_t)(Data>>32));
	  }

  	/* Wait for last operation to be completed */
  	status = Stm32c0_Flash_WaitOperation(STM32C0_FLASH_TIMEOUT_VALUE);

    /* If the program operation is completed, disable the PG or FSTPG Bit */
  	tmp = readMem(STM32C0_FLASH_CR);
  	writeMem(STM32C0_FLASH_CR, tmp & (~STM32C0_FLASH_CR_PG));

	/* return status */
	return status;
}


Target_StatusTypeDef Stm32c0_Flash_Unlock(void)
{
	Target_StatusTypeDef status = TARGET_OK;
	uint32_t tmp = 0;

	tmp = readMem(STM32C0_FLASH_CR)& STM32C0_FLASH_CR_LOCK;
	if(tmp != 0x00U)
	{
    /* Authorize the FLASH Registers access */
		writeMem(STM32C0_FLASH_KEYR, STM32C0_FLASH_KEY1);
		writeMem(STM32C0_FLASH_KEYR, STM32C0_FLASH_KEY2);

		/* verify Flash is unlock */
		tmp = (readMem(STM32C0_FLASH_CR)& STM32C0_FLASH_CR_LOCK);
		if(tmp != 0x00U)
    {
      status = TARGET_ERROR;
    }
	}
	return status;
}

Target_StatusTypeDef Stm32c0_Flash_Lock(void)
{
	Target_StatusTypeDef status = TARGET_OK;
	uint32_t tmp = 0;

	/* Set the LOCK Bit to lock the FLASH Registers access */
	writeMem(STM32C0_FLASH_CR, STM32C0_FLASH_CR_LOCK);

	/* verify Flash is locked */
	tmp = readMem(STM32C0_FLASH_CR)& STM32C0_FLASH_CR_LOCK;
	if(tmp != 0x00U)
  {
    status = TARGET_OK;
  }
	return status;
}

Target_StatusTypeDef Stm32c0_Flash_WaitOperation(uint32_t Timeout)
{
  uint32_t error;
  uint32_t tmp = 0;

  /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
     Even if the FLASH operation fails, the BUSY flag will be reset and an error
     flag will be set */
  uint32_t timeout = HAL_GetTick() + Timeout;

  /* Wait if any operation is ongoing */
  tmp = readMem(STM32C0_FLASH_SR) & STM32C0_FLASH_FLAG_BSY;
  while(tmp != 0x00U)
  {
    if (HAL_GetTick() >= timeout)
    {
      return TARGET_TIMEOUT;
    }
    tmp = readMem(STM32C0_FLASH_SR) & STM32C0_FLASH_FLAG_BSY;
  }

  /* check flash errors */
  error = readMem(STM32C0_FLASH_SR) & STM32C0_FLASH_FLAG_SR_ERROR;

  /* clear error flags */
  writeMem(STM32C0_FLASH_SR, error);

  /* Wait for control register to be written */
  timeout = HAL_GetTick() + Timeout;
  tmp = readMem(STM32C0_FLASH_SR) & STM32C0_FLASH_FLAG_CFGBSY;
  while(tmp != 0x00U)
  {
    if (HAL_GetTick() >= timeout)
    {
      return TARGET_TIMEOUT;
    }
    tmp = readMem(STM32C0_FLASH_SR) & STM32C0_FLASH_FLAG_CFGBSY;
  }

  return TARGET_OK;
}


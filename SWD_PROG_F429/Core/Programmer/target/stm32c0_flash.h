/*
 * stm32c0_flash.h
 *
 *  Created on: Jul 5, 2024
 *      Author: kangh
 */

#ifndef STM32C0_FLASH_H_
#define STM32C0_FLASH_H_
#include "target.h"

#define STM32C0_PERIPH_BASE										(0x40000000UL)
#define STM32C0_AHBPERIPH_BASE								(STM32C0_PERIPH_BASE + 0x00020000UL)
#define STM32C0_FLASH_R_BASE									(STM32C0_AHBPERIPH_BASE + 0x00002000UL)
#define STM32C0_FLASH               					(STM32C0_FLASH_R_BASE)

/**
  * @brief FLASH Registers
  */
#define STM32C0_FLASH_CR					(STM32C0_FLASH_R_BASE) + 0x14
#define STM32C0_FLASH_SR					(STM32C0_FLASH_R_BASE) + 0x10
#define STM32C0_FLASH_KEYR				(STM32C0_FLASH_R_BASE) + 0x08


#define STM32C0_FLASH_TIMEOUT_VALUE             1000U          /*!< FLASH Execution Timeout, 1 s */

/** @defgroup FLASH_Exported_Constants FLASH Exported Constants
  * @{
  */
/** @defgroup FLASH_Keys FLASH Keys
  * @{
  */
#define STM32C0_FLASH_KEY1                      0x45670123U   /*!< Flash key1 */
#define STM32C0_FLASH_KEY2                      0xCDEF89ABU   /*!< Flash key2: used with FLASH_KEY1
                                                           to unlock the FLASH registers access */

/*******************  Bits definition for FLASH_CR register  ******************/
#define STM32C0_FLASH_CR_PG_Pos                        (0U)
#define STM32C0_FLASH_CR_PG_Msk                        (0x1UL << STM32C0_FLASH_CR_PG_Pos)          /*!< 0x00000001 */
#define STM32C0_FLASH_CR_PG                            STM32C0_FLASH_CR_PG_Msk
#define STM32C0_FLASH_CR_PER_Pos                       (1U)
#define STM32C0_FLASH_CR_PER_Msk                       (0x1UL << STM32C0_FLASH_CR_PER_Pos)         /*!< 0x00000002 */
#define STM32C0_FLASH_CR_PER                           STM32C0_FLASH_CR_PER_Msk
#define STM32C0_FLASH_CR_MER1_Pos                      (2U)
#define STM32C0_FLASH_CR_MER1_Msk                      (0x1UL << STM32C0_FLASH_CR_MER1_Pos)        /*!< 0x00000004 */
#define STM32C0_FLASH_CR_MER1                          STM32C0_FLASH_CR_MER1_Msk
#define STM32C0_FLASH_CR_PNB_Pos                       (3U)
#define STM32C0_FLASH_CR_PNB_Msk                       (0xFUL << STM32C0_FLASH_CR_PNB_Pos)        /*!< 0x000001F8 */
#define STM32C0_FLASH_CR_PNB                           STM32C0_FLASH_CR_PNB_Msk
#define STM32C0_FLASH_CR_STRT_Pos                      (16U)
#define STM32C0_FLASH_CR_STRT_Msk                      (0x1UL << STM32C0_FLASH_CR_STRT_Pos)        /*!< 0x00010000 */
#define STM32C0_FLASH_CR_STRT                          STM32C0_FLASH_CR_STRT_Msk
#define STM32C0_FLASH_CR_OPTSTRT_Pos                   (17U)
#define STM32C0_FLASH_CR_OPTSTRT_Msk                   (0x1UL << STM32C0_FLASH_CR_OPTSTRT_Pos)     /*!< 0x00020000 */
#define STM32C0_FLASH_CR_OPTSTRT                       STM32C0_FLASH_CR_OPTSTRT_Msk
#define STM32C0_FLASH_CR_FSTPG_Pos                     (18U)
#define STM32C0_FLASH_CR_FSTPG_Msk                     (0x1UL << STM32C0_FLASH_CR_FSTPG_Pos)       /*!< 0x00040000 */
#define STM32C0_FLASH_CR_FSTPG                         STM32C0_FLASH_CR_FSTPG_Msk
#define STM32C0_FLASH_CR_EOPIE_Pos                     (24U)
#define STM32C0_FLASH_CR_EOPIE_Msk                     (0x1UL << STM32C0_FLASH_CR_EOPIE_Pos)       /*!< 0x01000000 */
#define STM32C0_FLASH_CR_EOPIE                         STM32C0_FLASH_CR_EOPIE_Msk
#define STM32C0_FLASH_CR_ERRIE_Pos                     (25U)
#define STM32C0_FLASH_CR_ERRIE_Msk                     (0x1UL << STM32C0_FLASH_CR_ERRIE_Pos)       /*!< 0x02000000 */
#define STM32C0_FLASH_CR_ERRIE                         STM32C0_FLASH_CR_ERRIE_Msk
#define STM32C0_FLASH_CR_RDERRIE_Pos                   (26U)
#define STM32C0_FLASH_CR_RDERRIE_Msk                   (0x1UL << STM32C0_FLASH_CR_RDERRIE_Pos)     /*!< 0x04000000 */
#define STM32C0_FLASH_CR_RDERRIE                       STM32C0_FLASH_CR_RDERRIE_Msk
#define STM32C0_FLASH_CR_OBL_LAUNCH_Pos                (27U)
#define STM32C0_FLASH_CR_OBL_LAUNCH_Msk                (0x1UL << STM32C0_FLASH_CR_OBL_LAUNCH_Pos)  /*!< 0x08000000 */
#define STM32C0_FLASH_CR_OBL_LAUNCH                    STM32C0_FLASH_CR_OBL_LAUNCH_Msk
#define STM32C0_FLASH_CR_SEC_PROT_Pos                  (28U)
#define STM32C0_FLASH_CR_SEC_PROT_Msk                  (0x1UL << STM32C0_FLASH_CR_SEC_PROT_Pos)    /*!< 0x10000000 */
#define STM32C0_FLASH_CR_SEC_PROT                      STM32C0_FLASH_CR_SEC_PROT_Msk
#define STM32C0_FLASH_CR_OPTLOCK_Pos                   (30U)
#define STM32C0_FLASH_CR_OPTLOCK_Msk                   (0x1UL << STM32C0_FLASH_CR_OPTLOCK_Pos)     /*!< 0x40000000 */
#define STM32C0_FLASH_CR_OPTLOCK                       STM32C0_FLASH_CR_OPTLOCK_Msk
#define STM32C0_FLASH_CR_LOCK_Pos                      (31U)
#define STM32C0_FLASH_CR_LOCK_Msk                      (0x1UL << STM32C0_FLASH_CR_LOCK_Pos)        /*!< 0x80000000 */
#define STM32C0_FLASH_CR_LOCK                          STM32C0_FLASH_CR_LOCK_Msk


/*******************  Bits definition for FLASH_SR register  ******************/
#define STM32C0_FLASH_SR_EOP_Pos                       (0U)
#define STM32C0_FLASH_SR_EOP_Msk                       (0x1UL << STM32C0_FLASH_SR_EOP_Pos)         /*!< 0x00000001 */
#define STM32C0_FLASH_SR_EOP                           STM32C0_FLASH_SR_EOP_Msk
#define STM32C0_FLASH_SR_OPERR_Pos                     (1U)
#define STM32C0_FLASH_SR_OPERR_Msk                     (0x1UL << STM32C0_FLASH_SR_OPERR_Pos)       /*!< 0x00000002 */
#define STM32C0_FLASH_SR_OPERR                         STM32C0_FLASH_SR_OPERR_Msk
#define STM32C0_FLASH_SR_PROGERR_Pos                   (3U)
#define STM32C0_FLASH_SR_PROGERR_Msk                   (0x1UL << STM32C0_FLASH_SR_PROGERR_Pos)     /*!< 0x00000008 */
#define STM32C0_FLASH_SR_PROGERR                       STM32C0_FLASH_SR_PROGERR_Msk
#define STM32C0_FLASH_SR_WRPERR_Pos                    (4U)
#define STM32C0_FLASH_SR_WRPERR_Msk                    (0x1UL << STM32C0_FLASH_SR_WRPERR_Pos)      /*!< 0x00000010 */
#define STM32C0_FLASH_SR_WRPERR                        STM32C0_FLASH_SR_WRPERR_Msk
#define STM32C0_FLASH_SR_PGAERR_Pos                    (5U)
#define STM32C0_FLASH_SR_PGAERR_Msk                    (0x1UL << STM32C0_FLASH_SR_PGAERR_Pos)      /*!< 0x00000020 */
#define STM32C0_FLASH_SR_PGAERR                        STM32C0_FLASH_SR_PGAERR_Msk
#define STM32C0_FLASH_SR_SIZERR_Pos                    (6U)
#define STM32C0_FLASH_SR_SIZERR_Msk                    (0x1UL << STM32C0_FLASH_SR_SIZERR_Pos)      /*!< 0x00000040 */
#define STM32C0_FLASH_SR_SIZERR                        STM32C0_FLASH_SR_SIZERR_Msk
#define STM32C0_FLASH_SR_PGSERR_Pos                    (7U)
#define STM32C0_FLASH_SR_PGSERR_Msk                    (0x1UL << STM32C0_FLASH_SR_PGSERR_Pos)      /*!< 0x00000080 */
#define STM32C0_FLASH_SR_PGSERR                        STM32C0_FLASH_SR_PGSERR_Msk
#define STM32C0_FLASH_SR_MISERR_Pos                    (8U)
#define STM32C0_FLASH_SR_MISERR_Msk                    (0x1UL << STM32C0_FLASH_SR_MISERR_Pos)      /*!< 0x00000100 */
#define STM32C0_FLASH_SR_MISERR                        STM32C0_FLASH_SR_MISERR_Msk
#define STM32C0_FLASH_SR_FASTERR_Pos                   (9U)
#define STM32C0_FLASH_SR_FASTERR_Msk                   (0x1UL << STM32C0_FLASH_SR_FASTERR_Pos)     /*!< 0x00000200 */
#define STM32C0_FLASH_SR_FASTERR                       STM32C0_FLASH_SR_FASTERR_Msk
#define STM32C0_FLASH_SR_RDERR_Pos                     (14U)
#define STM32C0_FLASH_SR_RDERR_Msk                     (0x1UL << STM32C0_FLASH_SR_RDERR_Pos)       /*!< 0x00004000 */
#define STM32C0_FLASH_SR_RDERR                         STM32C0_FLASH_SR_RDERR_Msk
#define STM32C0_FLASH_SR_OPTVERR_Pos                   (15U)
#define STM32C0_FLASH_SR_OPTVERR_Msk                   (0x1UL << STM32C0_FLASH_SR_OPTVERR_Pos)     /*!< 0x00008000 */
#define STM32C0_FLASH_SR_OPTVERR                       STM32C0_FLASH_SR_OPTVERR_Msk
#define STM32C0_FLASH_SR_BSY1_Pos                      (16U)
#define STM32C0_FLASH_SR_BSY1_Msk                      (0x1UL << STM32C0_FLASH_SR_BSY1_Pos)        /*!< 0x00010000 */
#define STM32C0_FLASH_SR_BSY1                          STM32C0_FLASH_SR_BSY1_Msk
#define STM32C0_FLASH_SR_CFGBSY_Pos                    (18U)
#define STM32C0_FLASH_SR_CFGBSY_Msk                    (0x1UL << STM32C0_FLASH_SR_CFGBSY_Pos)      /*!< 0x00040000 */
#define STM32C0_FLASH_SR_CFGBSY                        STM32C0_FLASH_SR_CFGBSY_Msk


/** @defgroup FLASH_Flags FLASH Flags Definition
  * @{
  */
#define STM32C0_FLASH_FLAG_EOP                  STM32C0_FLASH_SR_EOP      /*!< FLASH End of operation flag */
#define STM32C0_FLASH_FLAG_OPERR                STM32C0_FLASH_SR_OPERR    /*!< FLASH Operation error flag */
#define STM32C0_FLASH_FLAG_PROGERR              STM32C0_FLASH_SR_PROGERR  /*!< FLASH Programming error flag */
#define STM32C0_FLASH_FLAG_WRPERR               STM32C0_FLASH_SR_WRPERR   /*!< FLASH Write protection error flag */
#define STM32C0_FLASH_FLAG_PGAERR               STM32C0_FLASH_SR_PGAERR   /*!< FLASH Programming alignment error flag */
#define STM32C0_FLASH_FLAG_SIZERR               STM32C0_FLASH_SR_SIZERR   /*!< FLASH Size error flag  */
#define STM32C0_FLASH_FLAG_PGSERR               STM32C0_FLASH_SR_PGSERR   /*!< FLASH Programming sequence error flag */
#define STM32C0_FLASH_FLAG_MISERR               STM32C0_FLASH_SR_MISERR   /*!< FLASH Fast programming data miss error flag */
#define STM32C0_FLASH_FLAG_FASTERR              STM32C0_FLASH_SR_FASTERR  /*!< FLASH Fast programming error flag */
#define STM32C0_FLASH_FLAG_RDERR                STM32C0_FLASH_SR_RDERR    /*!< FLASH PCROP read error flag */
#define STM32C0_FLASH_FLAG_OPTVERR              STM32C0_FLASH_SR_OPTVERR  /*!< FLASH Option validity error flag */
#define STM32C0_FLASH_FLAG_BSY                  STM32C0_FLASH_SR_BSY1     /*!< FLASH Operation Busy flag */
#define STM32C0_FLASH_FLAG_CFGBSY               STM32C0_FLASH_SR_CFGBSY   /*!< FLASH Configuration Busy flag */
#define STM32C0_FLASH_FLAG_PESD                 STM32C0_FLASH_SR_PESD     /*!< FLASH Programming/erase operation suspended */

#define STM32C0_FLASH_FLAG_ALL_ERRORS           (STM32C0_FLASH_FLAG_OPERR   | STM32C0_FLASH_FLAG_PROGERR | STM32C0_FLASH_FLAG_WRPERR | \
                                         STM32C0_FLASH_FLAG_PGAERR  | STM32C0_FLASH_FLAG_SIZERR  | STM32C0_FLASH_FLAG_PGSERR | \
                                         STM32C0_FLASH_FLAG_MISERR  | STM32C0_FLASH_FLAG_FASTERR | STM32C0_FLASH_FLAG_RDERR  | \
                                         STM32C0_FLASH_FLAG_OPTVERR)


#define STM32C0_FLASH_FLAG_SR_ERROR             (STM32C0_FLASH_FLAG_OPERR  | STM32C0_FLASH_FLAG_PROGERR | STM32C0_FLASH_FLAG_WRPERR |  \
		STM32C0_FLASH_FLAG_PGAERR | STM32C0_FLASH_FLAG_SIZERR  | STM32C0_FLASH_FLAG_PGSERR |   \
		STM32C0_FLASH_FLAG_MISERR | STM32C0_FLASH_FLAG_FASTERR | STM32C0_FLASH_FLAG_RDERR |   \
		STM32C0_FLASH_FLAG_OPTVERR)     /*!< All SR error flags */

Target_StatusTypeDef Stm32c0_Flash_MassErase(void);
Target_StatusTypeDef Stm32c0_Flash_Program(uint32_t Address, uint64_t Data);
Target_StatusTypeDef Stm32c0_Flash_Unlock(void);
Target_StatusTypeDef Stm32c0_Flash_Lock(void);
Target_StatusTypeDef Stm32c0_Flash_WaitOperation(uint32_t Timeout);

#endif /* STM32C0_FLASH_H_ */

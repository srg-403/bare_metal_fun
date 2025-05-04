/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Generic application start

*/

#include <stdint.h>
#include "helper.h"

void flash_led(uint8_t state){
    volatile uint32_t *RCC_AHB1ENR = (uint32_t*)0x40023830;
    *RCC_AHB1ENR |= (1U << 0);
    #define GPIOA_MODER  (*(uint32_t*)(0x40020000 + 0x00))
    #define GPIOA_ODR    (*(uint32_t*)(0x40020000 + 0x14))
    GPIOA_MODER = (GPIOA_MODER & ~(3U << (10)))
                 |  (1U << (10));         
    GPIOA_ODR |= (state << 5);
}

void unlock_flash_memory(void){
  if (*FLASH_CR_REGISTER_STM32F411 & FLASH_CR_LOCK_MASK) {
    *FLASH_KEYR_STM32F411 = ULOCK_KEY_1;
    *FLASH_KEYR_STM32F411 = ULOCK_KEY_2;
    flash_led(0);
  }
  else {
  flash_led(1);
  }
}

void flash_wait_ready (void){
  while(*FLASH_SR_REGISTER_STM32F411 & FLASH_SR_BSY_MASK);
}



/*
#########################
Flash memory erase STM32F4x
#########################

Sector 0  0x0800 0000 - 0x0800 3FFF 16 Kbytes
Sector 1  0x0800 4000 - 0x0800 7FFF 16 Kbytes
Sector 2  0x0800 8000 - 0x0800 BFFF 16 Kbytes
Sector 3  0x0800 C000 - 0x0800 FFFF 16 Kbytes
Sector 4  0x0801 0000 - 0x0801 FFFF 64 Kbytes
Sector 5  0x0802 0000 - 0x0803 FFFF 128 Kbytes
Sector 6  0x0804 0000 - 0x0805 FFFF 128 Kbytes
Sector 7  0x080E 0000 - 0x080F FFFF 128 Kbytes


1. Check that no flash memory operation is ongoing by checking the BSY bit in the
    FLASH_SR register
2. Set the SER bit and select the sector out of the 7 sectors in the main 
    memory block you wish to erase (SNB) in the FLASH_CR register
3. Set the STRT bit in the FLASH_CR register
4. Wait for the BSY bit to be cleared
*/
uint8_t erase_flash_memory_sector_stm32f41(uint8_t start_sector, uint8_t sector_count){
  uint8_t actual_sector = start_sector;
  unlock_flash_memory();
  flash_wait_ready();                                            
  *FLASH_CR_REGISTER_STM32F411 &= ~FLASH_CR_SER_MASK;
  *FLASH_CR_REGISTER_STM32F411 &= ~FLASH_CR_MER_MASK;
  *FLASH_CR_REGISTER_STM32F411 |= FLASH_CR_SER_MASK;
  for (uint8_t i = 0; i<=sector_count - 1; i++) {
    if (actual_sector <= SECTOR_07) {
      *FLASH_CR_REGISTER_STM32F411 &= ~SECTOR_CLEARING_MASK;
      *FLASH_CR_REGISTER_STM32F411 |= actual_sector;
      actual_sector += SECTOR_OFFSET;
      *FLASH_CR_REGISTER_STM32F411 |= FLASH_CR_STRT_MASK;
      flash_wait_ready();
    }
    else {
      flash_led(1);
      return 1;
    }
  }
  actual_sector = start_sector;
  return 0;
}


/*
#########################
Mass erase
#########################
The Mass erase command can be used to completely erase the user pages of the flash
memory. The information block is unaffected by this procedure. The following sequence is
recommended:
1. Check that no flash memory operation is ongoing by checking the BSY bit in the
FLASH_SR register
2. Set the MER bit in the FLASH_CR register
3. Set the STRT bit in the FLASH_CR register
4. Wait for the BSY bit to be reset
5. Check the EOP flag in the FLASH_SR register (it is set when the erase operation has
succeeded), and then clear it by software.
6. Clear the EOP flag.
*/
uint8_t erase_flash_memory_mass(void){
  unlock_flash_memory();
  flash_wait_ready();  
  *FLASH_CR_REGISTER_STM32F411 |= FLASH_CR_MER_MASK;
  *FLASH_CR_REGISTER_STM32F411 |= FLASH_CR_STRT_MASK;
  flash_wait_ready();
  if (*FLASH_SR_REGISTER_STM32F411 & FLASH_SR_EOP_MASK) {
  *FLASH_SR_REGISTER_STM32F411 |= FLASH_SR_EOP_MASK;
  }
  return 0;
}



/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/
int main(void) {
  //erase sector 1 test
  //erase_flash_memory_sector_stm32f41(SECTOR_04,7);
  erase_flash_memory_mass();
  


  while (1);
}

/*************************** End of file ****************************/


/*
1. task
  create erase sectors function

2. write some data into a certain page

3. task
  mass erase
*/
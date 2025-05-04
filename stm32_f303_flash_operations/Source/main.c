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

void unlock_flash_memory(void){
  if (*FLASH_CR_REGISTER & FLASH_CR_LOCK_MASK) {
    *FLASH_KEYR_STM32F411 = ULOCK_KEY_1;
    *FLASH_KEYR_STM32F411 = ULOCK_KEY_2;
  }
}

static void flash_lock(void){
    *FLASH_CR_REGISTER |= FLASH_CR_LOCK_MASK;
}

 void flash_wait_ready(void){
    while (*FLASH_SR_REGISTER & FLASH_SR_BSY_MASK);
}

void flash_status_set_sr_flags(uint8_t flag_mask){
  *FLASH_SR_REGISTER |= flag_mask;  
}
void flash_status_clear_cr_flags(uint8_t flag_mask){
  *FLASH_CR_REGISTER &= ~flag_mask;  
}
void flash_status_set_cr_flags(uint8_t flag_mask){
  *FLASH_CR_REGISTER |= flag_mask;  
}

uint8_t flash_program_halfword(uint32_t address, uint16_t data){
    flash_wait_ready();
    unlock_flash_memory();
    flash_status_set_sr_flags(FLASH_SR_PGERR_MASK);
    flash_status_set_sr_flags(FLASH_SR_EOP_MASK);
    flash_status_set_sr_flags(FLASH_SR_WRPRTERR_MASK);
    
    flash_status_set_cr_flags(FLASH_CR_PG_MASK);  


    *(volatile uint16_t*)address = data;

    flash_wait_ready();

    flash_status_clear_cr_flags(FLASH_CR_PG_MASK);

    if (*FLASH_SR_REGISTER & FLASH_SR_EOP_MASK) {
        flash_status_set_sr_flags(FLASH_SR_EOP_MASK);
        flash_lock();
        return 0;
    }
    flash_lock();
    return 1;
}

uint8_t erase_flash_memory_page_stm32f3(uint32_t flash_page){
  flash_wait_ready();
  *FLASH_CR_REGISTER |= FLASH_CR_PER_MASK;
  *FLASH_AR_REGISTER  = flash_page;
  *FLASH_CR_REGISTER |= FLASH_CR_STRT_MASK;
  flash_wait_ready();
  if (*FLASH_SR_REGISTER & FLASH_SR_EOP_MASK)
  {
    *FLASH_SR_REGISTER |= FLASH_SR_EOP_MASK;
    return 0;  
  }

  return 1;
}

uint8_t erase_flash_memory_sector_stm32f41(uint32_t start_sector, uint8_t sector_count){
  uint32_t current_sector_erase;
  flash_wait_ready();
  *FLASH_CR_REGISTER |= FLASH_CR_PER_MASK;
  
  if(SECTOR_AMOUNT - sector_count >= 0 || SECTOR_11 > start_sector){
      current_sector_erase = start_sector + SECTOR_ERASE_START_OP;
      for(uint8_t i = sector_count; i<=SECTOR_AMOUNT; i++){
        *FLASH_CR_REGISTER = current_sector_erase;
        current_sector_erase += SECTOR_OFFSET;
        flash_wait_ready();
      }
    }

/*if (*FLASH_SR_REGISTER & FLASH_SR_EOP_MASK){
    *FLASH_SR_REGISTER |= FLASH_SR_EOP_MASK;
    return 0;}
*/
  return 0;
}

uint8_t erase_flash_memory_mass(void){
  flash_wait_ready();
  *FLASH_CR_REGISTER |= FLASH_CR_MER_MASK;
  *FLASH_CR_REGISTER |= FLASH_CR_STRT_MASK;
  flash_wait_ready();
  if (*FLASH_SR_REGISTER & FLASH_SR_EOP_MASK)
  {
    *FLASH_SR_REGISTER |= FLASH_SR_EOP_MASK;
    return 0;
  }
  return 1;
}



/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/
int main(void) {
    unlock_flash_memory();
    if (erase_flash_memory_sector_stm32f41(SECTOR_00,2)) {
      return 1;
    }
    while (1) {
    
    }
    return 0;

    
}

/*************************** End of file ****************************/


/*
1. task
  create erase sectors function
  something like this:
    erase_page(start_sector, sector_count)
1. solution
  a. list of sectors as a mask for the flash_cr_register:
    0x8
    0x10
    0x18
    0x20
    0x28
    0x30
    0x38
    0x40
    0x48
    0x50
    0x58 = SECTOR_11
  b. add to the mask value 0x10002, so turn on sector erase and start the operation -> #define SECTOR_ERASE_START_OP 0x10002
  c. #define SECTOR_OFFSET 0x8, #define SECTOR_AMOUNT 11U
  d. erase the sectors in a for loop manner
    if(SECTOR_AMOUNT - sector_count >= 0 || SECTOR_11 < start_sector){
      current_sector_erase = start_sector+SECTOR_ERASE_START_OP;
      for(sector_count; sector_count<=SECTOR_AMOUNT; sector_count++){
        *FLASH_CR=current_sector_erase;
        current_sector_erase + SECTOR_OFFSET;
      }
    }




2. write some data into a certain page

3. task
  mass erase
*/
#ifndef HELPER_H
#define HELPER_H

#define FLASH_PAGE0_ADDR              (0x08000000)
#define FLASH_PAGE_COUNT              (255)
#define ULOCK_KEY_1                   (0x45670123)
#define ULOCK_KEY_2                   (0xCDEF89AB)
#define FLASH_KEYR_STM32F3            ((volatile uint32_t*)0x40022004)
#define FLASH_BASE_STM32F411          (0x40023C00)
#define FLASH_KEYR_STM32F411          ((volatile uint32_t*)(FLASH_BASE_STM32F411 + 0x04))
#define FLASH_AR_REGISTER             ((volatile uint32_t*)0x40022014)  
#define FLASH_CR_REGISTER             ((volatile uint32_t*)0x40022010)  
#define FLASH_SR_REGISTER             ((volatile uint32_t*)0x4002200C)   
#define FLASH_MEMORY_BASE_ADDRESS     ((volatile uint32_t*)0x08000000)
#define FLASH_MEMORY_SIZE             (0x800)                           // 2 kB
#define FLASH_SR_BSY_MASK             (1U << 0)                         //mask for Busy bit
#define FLASH_SR_PGERR_MASK           (1U << 2)                         //mask for Programming error bit
#define FLASH_SR_WRPRTERR_MASK        (1U << 4)                         //mask for Write protection error bit
#define FLASH_SR_EOP_MASK             (1U << 5)                         //mask for End of operation bit
#define FLASH_CR_PG_MASK              (1U << 0)                         //mask for PG bit
#define FLASH_CR_PER_MASK             (1U << 1)                         //mask for PER bit
#define FLASH_CR_MER_MASK             (1U << 2)                         //mask for MER bit
#define FLASH_CR_STRT_MASK            (1U << 6)                         //mask for STRT bit
#define FLASH_CR_LOCK_MASK            (1U << 7)                         //mask for LOCK bit

//flash_sectors st32f41
#define SECTOR_00                     0x0
#define SECTOR_01                     0x8
#define SECTOR_02                     0x10
#define SECTOR_03                     0x18
#define SECTOR_04                     0x20
#define SECTOR_05                     0x28
#define SECTOR_06                     0x30
#define SECTOR_07                     0x38
#define SECTOR_08                     0x40
#define SECTOR_09                     0x48
#define SECTOR_10                     0x50
#define SECTOR_11                     0x58
#define SECTOR_ERASE_START_OP         0x10002
#define SECTOR_OFFSET                 0x8
#define SECTOR_AMOUNT                 7U




/*
-----------
cheat code
-----------
1.reading a certain bit in a register:
  i.    create a mask for the desired bit, something like this: (1U << n) -> bit n
  ii.   dereference the pointer to the register you want to read from, take the mask from i. and do a bitwise AND operation
  iii.  *DESIRED_REGISTER & BIT_MASK

2.setting a certain bit in a register:
  i.    create a mask for the desired bit, something like this: (1U << n) -> bit n
  ii.   dereference the pointer to the register you want to read from, take the mask from i. and do a bitwise OR operation
  iii.  *DESIRED_REGISTER |= BIT_MASK

3.clearing a certain bit in a register:
  i.    create a mask for the desired bit, something like this: (1U << n) -> bit n
  ii.   dereference the pointer to the register you want to read from, take the mask from i. and do an inverted bitwise AND operation  
  iii.  *DESIRED_REGISTER &= ~BIT_MASK

4.wait for a register to have a certain value or a certain bit
  i.     while (*MY_REGISTER & SOME_BIT_VALUE);

5.memeory structure stm32f3

Page N (2 KB = 2 048 B = 1 024 half‑words)
┌────────────────────────────────────────
│ HW[0] │ HW[1] │ HW[2] │ … │ HW[1022] │ HW[1023]               |
│ addr  │ addr+2│ addr+4│ … │ addr+2044│ addr+2046              |
│ (0)   │ (2)   │ (4)   │    │ (2044)  │ (2046)                 |
└────────────────────────────────────────
*/


/*
#########################
Unlocking the flash memory
#########################
After reset, the FPEC is protected against unwanted write or erase operations. The
FLASH_CR register is not accessible in write mode, except for the OBL LAUNCH bit, used
to reload the OBL. An unlocking sequence should be written to the FLASH_KEYR register
to open the access to the FLASH_CR register. This sequence consists of two write
operations into FLASH_KEYR register:
1. Write KEY1 = 0x45670123
2. Write KEY2 = 0xCDEF89AB
*/

/*
#########################
Flash memory erase STM32F3x
#########################
The flash memory can be erased page by page or completely (mass erase).
Page erase
To erase a page, the procedure below must be followed:
1. Check that no flash memory operation is ongoing by checking the BSY bit in the
FLASH_SR register.
2. Set the PER bit in the FLASH_CR register.
3. Program the FLASH_AR register to select a page to erase.
4. Set the STRT bit in the FLASH_CR register (see below note).
5. Wait for the BSY bit to be reset.
6. Check the EOP flag in the FLASH_SR register (it is set when the erase operation has
succeeded), and then clear it by software.
7. Clear the EOP flag.
*/

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
..
Sector 11 0x080E 0000 - 0x080F FFFF 128 Kbytes


1. Check that no flash memory operation is ongoing by checking the BSY bit in the
    FLASH_SR register
2. Set the SER bit and select the sector out of the 12 sectors in the main 
    memory block you wish to erase (SNB) in the FLASH_CR register
3. Set the STRT bit in the FLASH_CR register
4. Wait for the BSY bit to be cleared
*/


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
/*
#########################
flash memory programming STM32F3x
#########################
1. Check that no main flash memory operation is ongoing by checking the BSY bit in the
FLASH_SR register.
2. Set the PG bit in the FLASH_CR register.
3. Perform the data write (half-word) at the desired address. 16 Bit
4. Wait until the BSY bit is reset in the FLASH_SR register.
5. Check the EOP flag in the FLASH_SR register (it is set when the programming
operation has succeeded), and then clear it by software.
*/

/*
#########################
flash memory programming STM32F4x
#########################
1. Check that no main flash memory operation is ongoing by checking the BSY bit in the
FLASH_SR register.
2. Set the PG bit in the FLASH_CR register
3. Perform the data write operation(s) to the desired memory address (inside main
  memory block or OTP area):
  – Byte access in case of x8 parallelism
  – Half-word access in case of x16 parallelism
  – Word access in case of x32 parallelism
  – Double word access in case of x64 parallelism
4. Wait for the BSY bit to be cleared.
*/



#endif /* HELPER_H */
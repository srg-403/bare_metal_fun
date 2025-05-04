/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Generic application start

*/

//#include <stdio.h>
#include <stdint.h>
#define NUM_DELAY_LOOPS     (0x800000)

volatile uint32_t *pClkctrlreg = (volatile uint32_t*)0x40023830; 
volatile uint32_t *pPortDModeReg = (volatile uint32_t*)0x40020000;
volatile uint32_t *pPortDOutReg = (volatile uint32_t*)0x40020014;
uint8_t sleep_time = 1;

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/

static void _Delay (unsigned int Loops) {
  int Cnt;
  asm volatile (
        "  mov %0, %1;\n"
        "1:;\n"
        "  subs %0, %0, #1;\n"
        "  bpl.n 1b;\n"
        : "=r" (Cnt)
        : "r" (Loops)
  );
}


int main(void) {
  //setup
  *pClkctrlreg |= 0x01;
  *pPortDModeReg &= 0xFFFFF3FF;
  *pPortDModeReg |= 0x00000400;

  for (; ;) {
    *pPortDOutReg &= (0<<5);
    _Delay(NUM_DELAY_LOOPS);
    *pPortDOutReg |= (1<<5);
    _Delay(NUM_DELAY_LOOPS);
  }


}

/*************************** End of file ****************************/

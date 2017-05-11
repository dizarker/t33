/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 2012  SEGGER Microcontroller GmbH & Co KG                *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : SWO.c
Purpose : Simple implementation for output via SWO for Cortex-M processors.
          It can be used with any IDE. This sample implementation ensures that
          output via SWO is enabled in order to gurantee that the application 
          does not hang.
          
--------  END-OF-HEADER  ---------------------------------------------
*/

/*********************************************************************
*
*       Prototypes (to be placed in a header file such as SWO.h)
*/
void SWO_PrintChar  (char c);
void SWO_PrintString(const char *s);

/*********************************************************************
*
*       Defines for Cortex-M debug unit
*/
#define ITM_STIM_U32 (*(volatile unsigned int*)0xE0000000)    // Stimulus Port Register word acces
#define ITM_STIM_U8  (*(volatile         char*)0xE0000000)    // Stimulus Port Register byte acces
#define ITM_ENA      (*(volatile unsigned int*)0xE0000E00)    // Control Register
#define ITM_TCR      (*(volatile unsigned int*)0xE0000E80)    // Trace control register
#define DHCSR        (*(volatile unsigned int*)0xE000EDF0)    // Debug Halting Control Status Register
#define DEMCR        (*(volatile unsigned int*)0xE000EDFC)    // Debug Exception Monitor Control Register

/*********************************************************************
*
*       SWO_PrintChar
*
*  Function description
*    Print a character to the ITM_STIM register in order to provide data for SWO
*
*/
void SWO_PrintChar(char c) {
  //
  // Check if SWO is set up. If it is not, return to avoid that a program
  // hangs if no debugger is connected.
  //
  //
  // Check if DEBUGEN in DHCSR is set
  //
  if ((DHCSR & 1)!= 1) {
    return;
  }
  //
  // Check if TRACENA in DEMCR is set
  //
  if ((DEMCR & (1 << 24)) == 0) {
    return;
  }
  //
  // Check if ITM_TRC is enabled
  //
  if ((ITM_TCR & (1 << 22)) == 1) {
    return;
  }
  //
  // Check if stimulus port 0 is enabled
  //
  if ((ITM_ENA & 1) == 0) {
    return;
  }
  //
  // Wait until STIMx is ready to accept at least 1 word
  //
  while ((ITM_STIM_U8 & 1) == 0);
  ITM_STIM_U8 = c;
}

/*********************************************************************
*
*       SWO_PrintString
*
*  Function description
*    Print a string via SWO
*
*/
void SWO_PrintString(const char *s) {
  //
  // Print out character per character
  //
  while (*s) {
    SWO_PrintChar(*s++);
  }
}
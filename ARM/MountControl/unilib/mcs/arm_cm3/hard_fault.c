#include "include.h"

#ifndef NDEBUG

#pragma pack(push, 4)
typedef struct
{
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
} stacked_regs_t;
#pragma pack(pop)

//=============================================================================
// This is called from the HardFault_HandlerAsm with a pointer the Fault stack as the parameter.
// We can then read the values from the stack and place them into local variables for ease of reading.
//=============================================================================
void hard_fault_debug_print(stacked_regs_t* stack) // prints 8 regs saved on the stack and so on
{
	dprintf("\n\nHard fault:\n");
	dprintf("R0  : %#08lx\n", stack->r0);
	dprintf("R1  : %#08lx\n", stack->r1);
	dprintf("R2  : %#08lx\n", stack->r2);
	dprintf("R3  : %#08lx\n", stack->r3);
	dprintf("R12 : %#08lx\n", stack->r12);
	dprintf("LR  : %#08lx\n", stack->lr);
	dprintf("PC  : %#08lx\n", stack->pc);
	dprintf("PSR : %#08lx\n", stack->psr);  
	dprintf("BFAR: %#08lx\n", *((volatile uint32_t*)0xE000ED38));
	dprintf("CFSR: %#08lx\n", *((volatile uint32_t*)0xE000ED28));
	dprintf("HFSR: %#08lx\n", *((volatile uint32_t*)0xE000ED2C));
	dprintf("DFSR: %#08lx\n", *((volatile uint32_t*)0xE000ED30));
	dprintf("AFSR: %#08lx\n", *((volatile uint32_t*)0xE000ED3C));
}



//=============================================================================
// Alternative Hard Fault handler to help debug the reason for a fault.
// To use, edit the vector table to reference this function in the HardFault vector
// This code is suitable for Cortex-M3 and Cortex-M0 cores
//=============================================================================
__asm void HardFault_Handler(void) // __irq __attribute__((naked))
{
	EXPORT  HardFault_Handler

	IMPORT  hard_fault_debug_print

	movs r0, #(1<<2)
	mov  r1, lr
	tst  r0, r1
	beq  do_msp
	mrs  r0, psp
	B    (hard_fault_debug_print)
	B     .
do_msp
	mrs  r0, msp
	B    (hard_fault_debug_print)
	B     .
}

#endif

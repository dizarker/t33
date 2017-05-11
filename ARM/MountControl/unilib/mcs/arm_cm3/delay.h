#ifndef _DELAY_H_
#define _DELAY_H_

//=============================================================================
static __inline void delay_8cycles(uint32_t x)
{
/*
	subs r0
	bcs  exit

	mrs r0, primask
exit:
	bx lr

*/

	while (x--) // 4  SUBS + BCS + PIPLINE_RELOAD
	{
		__nop(); //	5
		__nop(); //	6
		__nop(); //	7
		__nop(); //	8
	}
}

//=============================================================================
static __inline void delay_cycles(uint32_t x)
{
	uint32_t rem = x&7; // x%8

	if      (rem==1)  {__nop();}
	else if (rem==2)  {__nop();__nop();}
	else if (rem==3)  {__nop();__nop();__nop();}
	else if (rem==4)  {__nop();__nop();__nop();__nop();}
	else if (rem==5)  {__nop();__nop();__nop();__nop();__nop();}
	else if (rem==6)  {__nop();__nop();__nop();__nop();__nop();__nop();}
	else if (rem==7)  {__nop();__nop();__nop();__nop();__nop();__nop();__nop();}

	if ((x/=8))       delay_8cycles(x);
}

//=============================================================================
static __inline void delay_ns  (double ns)    {delay_cycles(ns/((double)1e9/(double)F_CPU) + 0.5);}
void                 delay_us  (uint32_t us);
void                 delay_ms  (uint32_t ms);
void                 delay_sec (uint32_t sec);

#endif

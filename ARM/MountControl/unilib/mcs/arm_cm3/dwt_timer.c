#include "include.h"


//=============================================================================
void dwt_init(void)
{
    if (!(DWT_CONTROL & 1))
    {
        SCB_DEMCR   |= 0x01000000;
        DWT_CYCCNT   = 0; 
        DWT_CONTROL |= 1; // enable the counter
    }
}

//=============================================================================
static uint32_t dwt_get(void)
{
    return DWT_CYCCNT;
}

//=============================================================================
static uint_fast8_t dwt_compare(uint32_t tp)
{
    return (dwt_get()  < tp);
}

//=============================================================================
void dwt_delay(uint32_t us) // microseconds
{
    uint32_t tp = dwt_get() + us * (F_CPU/1000000UL);

    while (dwt_compare(tp))
	{
		;
	}
}

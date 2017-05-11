/*
 *  global_macro.h rev.2.0.0 2010-02-03
 *
 *  Common macros lib.
 *
 *  Contributors:
 *                Ivanov Anton (c) www.automatix.ru
 *
 */


#ifndef _GLOBAL_MACRO_H_
#define _GLOBAL_MACRO_H_

#ifdef __cplusplus
  extern "C" {
#endif

/*
#if defined ( __CC_ARM )
#	pragma anon_unions  // anonymous union/struct
#endif
*/

//=============================================================================
// Attributes macro
//=============================================================================
#define INLINE                      __inline __attribute__((always_inline))
#define NOINLINE                    __attribute__((noinline))
#define NORETURN                    __attribute__ ((noreturn))

#define nop()                       __nop()

#define pgm_read_byte(address)      *(( uint8_t*)(address))
#define pgm_read_word(address)      *((uint16_t*)(address))
#define pgm_read_dword(address)     *((uint32_t*)(address))
#define pgm_read_float(address)     *((   float*)(address))

#define pgm_read_ptr(address)       ((void*)pgm_read_word(address))

#define PGM_READ(DST,SRC)           memcpy((DST), (SRC), sizeof(*(DST)))
#define RAM_READ(DST,SRC)           memcpy((DST), (SRC), sizeof(*(DST)))


//=============================================================================
// str macro
//=============================================================================
#define STR(S)                      #S       // STR(blabla) = "blabla"
#define XSTR(S)                     STR(S)   // STR(_version) = "v1.0" if _version = "v1.0"

//=============================================================================
// binary nums macro
//=============================================================================
#define ARRAY_SIZE(ARRAY)           (sizeof((ARRAY))/sizeof((ARRAY[0])))

//#define ELEMENTS_QTY(PMAS)          (sizeof((PMAS))/sizeof((PMAS[0])))
//#define INTERVALS_QTY(PMAS)         (ELEMENTS_QTY((PMAS))-1)

#define TESTBIT(BYTE,BIT)           ((BYTE)&(1UL<<(BIT)))

#define BYTE0(X)                    ((uint8_t)((X)>>0))
#define BYTE1(X)                    ((uint8_t)((X)>>8))
#define BYTE2(X)                    ((uint8_t)((X)>>16))
#define BYTE3(X)                    ((uint8_t)((X)>>24))

#define LH2WORD(LBYTE,HBYTE)        (((uint16_t)(HBYTE)<<8) + (uint8_t)(LBYTE))

//#define SWAP(x,y)                 ((x) ^= (y) ^= (x) ^= (y))

//=============================================================================
// math macro
//=============================================================================

#define PI                          3.14159265358979323846264
#define RAD2DEG(RAD)                (((RAD)*180.0)/PI)
#define DEG2RAD(DEG)                (((DEG)/180.0)*PI)

#define FLT_NAN                     (0.0f/0.0f) // 0xffc0xxxx
#define FLT_INF                     (1.0f/0.0f) // 0x7f800000

#define DOUBLE_NAN                  (((double)0.0)/((double)0.0))

#define MIN(a,b)                    (((a)<(b))?(a):(b))
#define MAX(a,b)                    (((a)>(b))?(a):(b))
#define ABS(x)                      (((x)>0)?(x):(-(x)))

#define FABS(x)                     fabsf(x)        // or (((x)>0.0f)?(x):(-(x)))
#define FSIGN(x)                    signbit(x)

#define FROUND(f)                   ((uint32_t)((f)+0.5f))
#define FROUNDS(x)                  (((x)>=0)?(int32_t)((x)+0.5):(int32_t)((x)-0.5))

#define F1_CHECK (defined (STM32F10X_LD) || (STM32F10X_LD_VL) || (STM32F10X_MD) || (STM32F10X_MD_VL) || (STM32F10X_HD) || (STM32F10X_HD_VL) || (STM32F10X_XL) || (STM32F10X_CL)) //Журавлев

#ifdef __cplusplus
  }
#endif

#endif

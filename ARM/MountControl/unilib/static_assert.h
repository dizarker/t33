#ifndef _STATIC_ASSERT_H_
#define _STATIC_ASSERT_H_

//=======================================================================================
// ASSERTION - ÓÒÂÅÐÆÄÅÍÈÅ
//
// Example of usage:
// STATIC_ASSERT(sizeof(int) > 4); // produce error in ARM or x86 machine
//
//=======================================================================================

#ifdef __cplusplus
	template<bool> struct CompileTimeError;
	template<> struct CompileTimeError<true> {};
#	define STATIC_ASSERT(expr)    (CompileTimeError<(expr) != 0>())
#else
#	define MY_JOIN_3_H(A,B,C)     A##B##C
#	define MY_JOIN_3(A,B,C)       MY_JOIN_3_H(A,B,C)
#	define STATIC_ASSERT_H(expr)  typedef int MY_JOIN_3(static_assert_,__LINE__,_h) [(expr) ? 1 : -1]
#	define STATIC_ASSERT(expr)    STATIC_ASSERT_H(expr)
#endif

#endif //_STATIC_ASSERT_H_


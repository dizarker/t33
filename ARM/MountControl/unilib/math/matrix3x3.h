/* 
 * Math of 3x3 matrix.
 *
 *
 */

#ifndef _MATRIX_3X3_H_
#define _MATRIX_3X3_H_

void matrix_3x3_clr      (float m[3][3]);
void matrix_3x3_cpy      (float a[3][3], float b[3][3]);  //  a = b
void matrix_3x3_add      (float a[3][3], float b[3][3]);  //  a = a+b
void matrix_3x3_sub      (float a[3][3], float b[3][3]);  //  a = a-b
void matrix_3x3_mul      (float a[3][3], float b[3][3]);  //  a = a*b


void matrix_3x3_print    (float m[3][3]);


#endif

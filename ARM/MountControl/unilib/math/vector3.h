/* 
 * Math of vector[3].
 *
 *
 */

#ifndef _VECTOR3_H_
#define _VECTOR3_H_


#define VEC3_DEG2RAD(A,B,C) {DEG2RAD(A), DEG2RAD(B), DEG2RAD(C)}
#define VEC3_RAD2DEG(A,B,C) {RAD2DEG(A), RAD2DEG(B), RAD2DEG(C)}


void vector3_cpy           (float v1[3], float v2[3]); // v1=v2

void vector3_cross_product (float a[3], float b[3]); // a = a*b

void vector3_to_deg        (float v[3]);
void vector3_to_rad        (float v[3]);

void vector3_print         (float v[3]);
void vector3_print_as_deg  (float v[3]);


#endif

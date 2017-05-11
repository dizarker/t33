/* 
 * Math of spherical and decart.
 *
 *
 */

#ifndef _DECART_H_
#define _DECART_H_


void spherical_to_decart    (float spherical[2], float decart[3]);         // radianes
void decart_to_spherical    (float decart[3],    float spherical[2]);      // radianes
void decart_rotate          (float decart[3],    float fi_rad, char axis); // radianes

void euler321_to_ned_decart (float euler[3],  float decart[3]);            // radianes
void ned_decart_to_euler321 (float decart[3], float euler[3]);             // radianes


void euler312_to_ned_decart (float euler[3], float decart[3]);             // radianes
void ned_decart_to_euler312 (float decart[3], float euler[3]);             // radianes


void euler321_print_as_ned_decart(float euler[3]);
void euler312_print_as_ned_decart(float euler[3]);


#endif

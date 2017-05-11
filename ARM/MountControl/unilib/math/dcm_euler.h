/*
 * Memo:
 *		Euler[3] vector ang can be in any rotation sequence(123, 321...))!
 *		All angels in radianes.
 *
 */


#ifndef _DCM_EULER_H_
#define _DCM_EULER_H_

//=============================================================================
void  dcm_to_euler123   (float dcm[3][3], float euler[3]);   // xyz rotation sequence
void  dcm_to_euler321   (float dcm[3][3], float euler[3]);   // zyx rotation sequence
void  dcm_to_euler312   (float dcm[3][3], float euler[3]);   // zxy rotation sequence

void  euler_to_dcm      (float euler[3], float dcm[3][3], char axis[3]); // Convert the given euler's angels (any rotation sequence: "xyz", "zyx"....) into a directional matrix

void  euler321_to_dcm   (float euler[3], float dcm[3][3]);   // Convert the given euler321 (yaw, pitch, roll)  into a directional matrix
void  euler312_to_dcm   (float euler[3], float dcm[3][3]);   // Convert the given euler312 (yaw, roll,  pitch) into a directional matrix

void  rot_to_dcm        (char axis, float angle, float dcm[3][3]);

void  euler_cpy         (float euler1[3], float euler2[3]);  // 1<--2
void  euler_to_deg      (float euler[3]);
void  euler_to_rad      (float euler[3]);


void  euler321_to_312   (float euler[3]);                    // slow
void  euler312_to_321   (float euler[3]);                    // slow


void  euler312_zero_yaw (float euler[3]);                    // to spherical (yaw=0, roll, pitch)  
void  euler321_zero_yaw (float euler[3]);                    // to spherical (yaw=0, pitch, roll)  


void  euler321_move_yaw (float euler[3], float yaw_rad);     // with stabilization of XY decartes coordinates
void  euler312_move_yaw (float euler[3], float yaw_rad);     // with stabilization of XY decartes coordinates


void  dcm_init          (float dcm[3][3]);

void  dcm_rev           (float dcm[3][3]); // ?

//void xyz2xyzr(float xyz[3], float xyzr[3]);

float deg_to_180_range  (float deg);       // feet any angel to +-180 range
float rad_to_pi_range   (float rad);       // feet any angel to +-PI range

float short_arc         (float x);
void  euler321_minimize (float euler[3]);

#endif

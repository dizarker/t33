#include "include.h"

//=============================================================================
void quat_to_euler321(float quat[4], float euler[3])
{
	float q0 = quat[3];
	float q1 = quat[0];
	float q2 = quat[1];
	float q3 = quat[2];

	euler[0] = atan2f(2*(q1*q2+q0*q3),q0*q0+q1*q1-q2*q2-q3*q3);
	euler[1] = asinf(-2*(q1*q3-q0*q2));
	euler[2] = atan2f(2*(q2*q3+q0*q1),q0*q0-q1*q1-q2*q2+q3*q3);
}

/*

//=============================================================================
// Function Name  : VN_quat2DCM(float *q, float **A)
// Description    : Convert a quaternion into to a directional cosine matrix.
// Equation       : A = quat2dcm(q)                                        
// Input          : q -> Quaternion attitude
// Output         : A -> Directional cosine matrix (3x3)
// Return         : None
//=============================================================================
void VN_Quat2DCM(float *q, float **A)
{
	float t[12];

	t[0] = q[0]*q[0];
	t[1] = q[1]*q[1];
	t[2] = q[2]*q[2];
	t[3] = q[3]*q[3];
	t[4] = q[0]*q[1]*2.0;
	t[5] = q[0]*q[2]*2.0;
	t[6] = q[0]*q[3]*2.0;
	t[7] = q[1]*q[2]*2.0;
	t[8] = q[1]*q[3]*2.0;
	t[9] = q[2]*q[3]*2.0;
	t[10]= t[0]-t[1];
	t[11]= t[3]-t[2];

	A[0][0] =  t[10]+t[11];
	A[1][1] = -t[10]+t[11];
	A[2][2] = -t[0]-t[1]+t[2]+t[3];
	A[0][1] = t[4]+t[9];
	A[1][0] = t[4]-t[9];
	A[1][2] =  t[6]+t[7];
	A[2][1] = -t[6]+t[7];
	A[0][2] = t[5]-t[8];
	A[2][0] = t[5]+t[8];
}


//=============================================================================
void quat2prv(quat_t* quat, attitude_t* att) // Convert a quaternion attitude representation to the principal rotatin vector
{
	float p  = 2*acosf(quat->q[3]);
	float sp = sinf(p/2);

	att->array[0] = quat->q[0] / sp * p * (180.0/PI);
	att->array[1] = quat->q[1] / sp * p * (180.0/PI);
	att->array[2] = quat->q[2] / sp * p * (180.0/PI);
}
*/


#include "include.h"

//=============================================================================
float deg_to_180_range(float deg)       // feet any angel to +-180 range
{
	while ((int)(fabsf(deg)+0.5f) > 180) // integer degree math works only!
	{
		deg -= copysignf(360.0f, deg);
	}

	return (deg);
}

//=============================================================================
float rad_to_pi_range(float rad)      // feet any angel to +-PI range
{
	return (DEG2RAD(deg_to_180_range(RAD2DEG(rad))));
}


//=============================================================================
void euler321_minimize(float euler[3])
{
	if (fabsf(fabsf(euler[0])-DEG2RAD(180.0)) < DEG2RAD(0.00001))
	{
		euler[0]  = DEG2RAD(0);            // yaw
		euler[1]  = DEG2RAD(180)-euler[1]; // pitch
		euler[2] += DEG2RAD(180);          // roll
	}
	else if (fabsf(fabsf(euler[1])-DEG2RAD(90.0)) < DEG2RAD(0.00001))
	{
		float x   = euler[0];

		euler[0]  = DEG2RAD(0);                // yaw
		euler[1]  = euler[1];                  // pitch
		euler[2]  = -(fabsf(x)+fabsf(euler[2])); // roll
	}


	for (int i=0; i<3; i++)
	{
		euler[i] = rad_to_pi_range(euler[i]);
	}
}


//=============================================================================
void dcm_to_euler123(float dcm[3][3], float euler[3])   // xyz rotation sequence
{
	euler[0] =  atan2f(dcm[1][2], dcm[2][2]);  // x first  rotation
	euler[1] = -asinf( dcm[0][2]);             // y second rotation
	euler[2] =  atan2f(dcm[0][1], dcm[0][0]);  // z last   rotation 
}

//=============================================================================
void dcm_to_euler321(float dcm[3][3], float euler[3])   // zyx rotation sequence
{
	euler[0] = atan2f(-dcm[1][0], dcm[0][0]);  // z first  rotation
	euler[1] = asinf(  dcm[2][0]);             // y second rotation
	euler[2] = atan2f(-dcm[2][1], dcm[2][2]);  // x last   rotation

	//	analog:
	//	att[0] = RAD2DEG(atan2f(dcm[0][1], dcm[0][0])); // yaw  
	//	att[1] = RAD2DEG(-asinf(dcm[0][2]));            // pitch
	//	att[2] = RAD2DEG(atan2f(dcm[1][2], dcm[2][2])); // roll
}

//=============================================================================
void dcm_to_euler312(float dcm[3][3], float euler[3])   // zxy rotation sequence
{

	euler[0] =  atan2f( dcm[0][1], dcm[1][1]);  // z firs   rotation
	euler[1] = -asinf(  dcm[2][1]);             // x second rotation
	euler[2] =  atan2f( dcm[2][0], dcm[2][2]);  // y last   rotation 


/*
	euler[0] =  atan2f(-dcm[0][1], dcm[1][1]);  // z firs   rotation
	euler[1] = -asinf(  dcm[2][1]);             // x second rotation
	euler[2] =  atan2f( dcm[2][0], dcm[2][2]);  // y last   rotation 
*/

}

//=============================================================================
void euler_cpy(float euler1[3], float euler2[3]) // 1<--2
{
	vector3_cpy(euler1, euler2);
}

//=============================================================================
void euler_to_deg(float euler[3])
{
	vector3_to_deg(euler);
}

//=============================================================================
void euler_to_rad(float euler[3])
{
	vector3_to_rad(euler);
}


//=============================================================================
void euler321_to_312(float euler[3]) //slow
{
/*
	float s[3];
	float c[3];

	for (int i=0; i<3; i++)
	{
		float a = euler[i];  // ypr

		s[i] = sinf(a);
		c[i] = cosf(a);
	}

	enum
	{
		Y = 0,
		P = 1,
		R = 2
	};


	euler[0] =  atan2f(s[Y]*c[R]+c[Y]*s[P]*s[R], c[Y]*c[R]-s[Y]*s[P]*s[R]);  // z firs   rotation

	euler[1] = -asinf(-s[R]*c[P]);                                           // x second rotation

	euler[2] =  atan2f(s[P], c[R]*c[P]);                                     // y last   rotation 
*/
	float dcm[3][3];

	euler321_to_dcm(euler, dcm);

	dcm_to_euler312(dcm, euler);
}

//=============================================================================
void euler312_to_321(float euler[3]) //slow
{
	float dcm[3][3];

	euler312_to_dcm(euler, dcm);

	dcm_to_euler321(dcm, euler);
}

//=============================================================================
void euler312_zero_yaw(float euler[3]) // to spherical (yaw=0, roll, pitch)
{
	float decart[3];

	euler312_to_ned_decart(euler, decart);
	ned_decart_to_euler312(decart, euler);  
}

//=============================================================================
void euler321_zero_yaw(float euler[3]) // to spherical (yaw=0, pitch, roll)
{
	float decart[3];

	euler321_to_ned_decart(euler, decart);
	ned_decart_to_euler321(decart, euler);  
}

//=============================================================================
void euler_to_dcm(float euler[3], float dcm[3][3], char axis[3]) // Convert the given euler321 (yaw, pitch, roll) into a directional matrix
{
	dcm_init(dcm);

	for (int i=0; i<3; i++)
	{
		float dcm_rot[3][3];

		rot_to_dcm(axis[i], euler[i], dcm_rot); // init zero dcm

		matrix_3x3_mul(dcm, dcm_rot);           // dcm = dcm*dcm_rot
	}
}

//=============================================================================
void euler312_to_dcm(float euler[3], float dcm[3][3]) // Convert the given euler321 (yaw, pitch, roll) into a directional matrix
{
	euler_to_dcm(euler, dcm, "zxy");
}

//=============================================================================
void euler321_to_dcm(float euler[3], float dcm[3][3]) // Convert the given euler321 (yaw, pitch, roll) into a directional matrix
{
	float s[3];
	float c[3];

	for (int i=0; i<3; i++)
	{
		float a = euler[i];  // ypr

		s[i] = sinf(a);
		c[i] = cosf(a);
	}

	enum
	{
		Y = 0,
		P = 1,
		R = 2
	};


	dcm[0][0] =  c[Y]*c[P];   dcm[0][1] =  s[Y]*c[R] + c[Y]*s[P]*s[R];   dcm[0][2] =  s[Y]*s[R] - c[Y]*s[P]*c[R];
	dcm[1][0] = -s[Y]*c[P];   dcm[1][1] =  c[Y]*c[R] - s[Y]*s[P]*s[R];   dcm[1][2] =  c[Y]*s[R] + s[Y]*s[P]*c[R];
	dcm[2][0] =  s[P];        dcm[2][1] = -s[R]*c[P];                    dcm[2][2] =  c[R]*c[P];

/*
ALT:
	float t[12];

	t[0] = sinf(YPR[0]);
	t[1] = cosf(YPR[0]);
	t[2] = sinf(YPR[1]);
	t[3] = cosf(YPR[1]);
	t[4] = sinf(YPR[2]);
	t[5] = cosf(YPR[2]);
	t[6] = t[4]*t[2];
	t[7] = t[5]*t[2];

	A[0][0] = t[3]*t[1];
	A[0][1] = t[3]*t[0];
	A[0][2] = -t[2];
	A[1][0] = t[6]*t[1]-t[5]*t[0];
	A[1][1] = t[6]*t[0]+t[5]*t[1];
	A[1][2] = t[4]*t[3];
	A[2][0] = t[7]*t[1]+t[4]*t[0];
	A[2][1] = t[7]*t[0]-t[4]*t[1];
	A[2][2] = t[5]*t[3];

*/
}




//=============================================================================
void rot_to_dcm(char axis, float angle, float dcm[3][3])   // rad
{
	matrix_3x3_clr(dcm);

	#define ROT2DCM(A,B,C)                                   \
	do                                                       \
	{                                                        \
		dcm[A][A] = 1;                                       \
		dcm[B][B] = dcm[C][C]   = cosf(angle);               \
		dcm[C][B] = -(dcm[B][C] = sinf(angle));              \
	} while (0)

	axis = toupper(axis);

	if      (axis=='X')  ROT2DCM(0,1,2    );
	else if (axis=='Y')  ROT2DCM(  1,2,0  );
	else if (axis=='Z')  ROT2DCM(    2,0,1);
}

//=============================================================================
void dcm_init(float dcm[3][3])
{
	matrix_3x3_clr(dcm);

	for (int i=0; i<3; i++)
	{
		dcm[i][i] = 1;      // diag = 1
	}
}


//=============================================================================
void dcm_rev(float dcm[3][3]) //???
{
	for (int i=0; i<3; i++)
	{
		dcm[0][i] = -dcm[0][i];
		dcm[1][i] = -dcm[1][i];
		//dcm[2][i] =  dcm[2][i]; // dummy because of not minus ???
	}
}

/*
//=============================================================================
void xyz2xyzr(float xyz[3], float xyzr[3])
{
	float cf,cq,cy,sf,sq,sy;

	cf=cos(xyz[0]);
	cq=cos(xyz[1]);
	cy=cos(xyz[2]);

	sf=sin(xyz[0]);
	sq=sin(xyz[1]);
	sy=sin(xyz[2]);

	xyzr[0] =  atan2(sy*sq*cf-cy*sf,cq*cf);
	xyzr[1] = -asin(-cy*sq*cf-sy*sf);
	xyzr[2] =  atan2(-cy*sq*sf+sy*cf,cq*cy);
}
*/

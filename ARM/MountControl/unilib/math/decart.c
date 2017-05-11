#include "include.h"

//=============================================================================
void spherical2decart(float spherical[2], float decart[3])
{
	float st = sinf(spherical[0]);       // sin thetta

	decart[0] = st * cosf(spherical[1]); // r * sin(thetta) * cos(fi)
	decart[1] = st * sinf(spherical[1]); // r * sin(thetta) * sin(fi)
	decart[2] =      cosf(spherical[0]); // r * cos(thetta)
}

//=============================================================================
void decart2spherical(float decart[3], float spherical[2])
{
	spherical[0] = atan2f(hypotf(decart[0],decart[1]), decart[2]); // thetta = arctg(sqrt(x^2 + y^2)/z)       (Є Є Ўл pitch)
	spherical[1] = atan2f(decart[1],decart[0]);                    // fi     = arctg(y/x)                     (Є Є Ўл yaw)
}

//=============================================================================
void decart_rotate(float decart[3], float fi_rad, char axis)
{
	float x = decart[0];
	float y = decart[1];
	float z = decart[2];

	float cf = cosf(fi_rad);
	float sf = sinf(fi_rad);

	// algorithm: decart-row mpy with DCM (row*collumn)

	switch (toupper(axis)) // select axis of rotation
	{
		case 'X':
		{
			decart[0] =  x;           // x
			decart[1] =  y*cf - z*sf; // y
			decart[2] =  y*sf + z*cf; // z
			break;
		}

		case 'Y':
		{
			decart[0] =  x*cf + z*sf; // x
			decart[1] =  y;           // y
			decart[2] = -x*sf + z*cf; // z
			break;
		}

		case 'Z':
		{
			decart[0] = x*cf - y*sf;  // x
			decart[1] = x*sf + y*cf;  // y
			decart[2] = z;            // z

			break;
		}
	}
}

/*
 * Используется правая (NED) Декартова система координат (ZYX YAW-PITCH-ROLL):  
 *
 *      X
 *       /   Y
 *      /-----
 *      |
 *    Z |
 *
 */


//=============================================================================
// 321 (yaw-pitch-roll) section:
//
//	roll  = 90-thetta
//	pitch = 90-fi
//
//	x'=x 
//	y'=z
//	z'=-y 
//
//=============================================================================


//=============================================================================
static void euler321_to_spherical(float euler[3], float spherical[2]) // radianes
{
	spherical[0] = DEG2RAD(90)-euler[2]; // thetta = 90 - roll
	spherical[1] = DEG2RAD(90)-euler[1]; // fi     = 90 - pitch
}

//=============================================================================
static void spherical_to_euler321(float spherical[2], float euler[3]) // radianes
{
	euler[0] = DEG2RAD(0);                // yaw   = 0
	euler[1] = DEG2RAD(90)-spherical[1];  // pitch = 90 - fi
	euler[2] = DEG2RAD(90)-spherical[0];  // roll  = 90 - thetta
}


//=============================================================================
void euler321_to_ned_decart(float euler[3], float decart[3]) // radianes
{
	float decart2[3];
	float spherical[2];

	euler321_to_spherical(euler, spherical); // radianes

	spherical2decart(spherical, decart2);

	// to std NED(xyz) conversion

	decart[0] = decart2[0]; //x=x'
	decart[1] =-decart2[2]; //y=-z'
	decart[2] = decart2[1]; //z=y'

	decart_rotate(decart, -euler[0], 'z'); // radianes
}

//=============================================================================
void ned_decart_to_euler321(float decart[3], float euler[3]) // radianes
{
	float spherical[2];
	float decart2[3];

	// from std NED(xyz) conversion

	decart2[0] = decart[0]; //x'=x
	decart2[1] = decart[2]; //y'=z
	decart2[2] =-decart[1]; //z'=-y

	decart2spherical(decart2, spherical);

	spherical_to_euler321(spherical, euler); // radianes
}


//=============================================================================
// 312 (yaw-roll-pitch) section:
//
//	roll = fi        
//	pitch = 90-thetta
//
//	x'=z 
//	y'=-y
//	z'=x 
//
//=============================================================================

//=============================================================================
static void euler312_to_spherical(float euler[3], float spherical[2]) // radianes
{
	spherical[1] = euler[1];              // fi = roll
	spherical[0] = DEG2RAD(90)-euler[2];  // thetta = 90 - pitch
}

//=============================================================================
static void spherical_to_euler312(float spherical[2], float euler[3]) // radianes
{
	euler[0] = DEG2RAD(0);                 // yaw   = 0
	euler[1] = spherical[1];               // roll = fi
	euler[2] = DEG2RAD(90)-spherical[0];   // pitch = 90-thetta
}

//=============================================================================
void euler312_to_ned_decart(float euler[3], float decart[3]) // radianes
{
	float decart2[3];
	float spherical[2];

	euler312_to_spherical(euler, spherical); // radianes

	spherical2decart(spherical, decart2);

	// to std NED(xyz) conversion

	decart[0] = decart2[2]; //x=z'
	decart[1] =-decart2[1]; //y=-y'
	decart[2] = decart2[0]; //z=x'

	decart_rotate(decart, -euler[0], 'z'); // radianes
}

//=============================================================================
void ned_decart_to_euler312(float decart[3], float euler[3]) // radianes
{
	float spherical[2];
	float decart2[3];

	// from std NED(xyz) conversion

	decart2[0] = decart[2]; //x'=z
	decart2[1] =-decart[1]; //y'=-y
	decart2[2] = decart[0]; //z'=x

	decart2spherical(decart2, spherical);

	spherical_to_euler312(spherical, euler); // radianes
}

//=============================================================================
// Print section:
//=============================================================================


//=============================================================================
void euler321_print_as_ned_decart(float euler[3])
{
	float decart[3]; // x,y,z

	euler321_to_ned_decart(euler, decart); // radianes

	vector3_print(decart);
}

//=============================================================================
void euler312_print_as_ned_decart(float euler[3])
{
	float decart[3]; // x,y,z

	euler312_to_ned_decart(euler, decart); // radianes

	vector3_print(decart);
}

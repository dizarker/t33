#include "include.h"

//=============================================================================
void vector3_cpy(float v1[3], float v2[3]) // v1=v2
{
	memcpy(&v1[0], &v2[0], sizeof(v1[0])*3);
}

//=============================================================================
void vector3_cross_product(float a[3], float b[3]) // a = a*b
{
	float c[3];

	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	c[x] = a[y]*b[z] - a[z]*b[y];
	c[y] = a[z]*b[x] - a[x]*b[z];
	c[z] = a[x]*b[y] - a[y]*b[x];

	vector3_cpy(a,c);  // a=c
}

//=============================================================================
void vector3_to_deg(float v[3])
{
	for (int i=0; i<3; i++)
	{
		v[i] = RAD2DEG(v[i]);
	}
}

//=============================================================================
void vector3_to_rad(float v[3])
{
	for (int i=0; i<3; i++)
	{
		v[i] = DEG2RAD(v[i]);
	}
}

//=============================================================================
void vector3_print(float v[3])
{
	printf("%000.2f\t%000.2f\t%000.2f\n", v[0], v[1], v[2]);
}

//=============================================================================
void vector3_print_as_deg(float v[3])
{
	printf("%000.1f\t%000.1f\t%000.1f\n", RAD2DEG(v[0]), RAD2DEG(v[1]), RAD2DEG(v[2]));
}

#include "include.h"

//=============================================================================
void matrix_3x3_clr(float m[3][3])
{
	memset(&m[0][0], 0, sizeof(m[0][0])*3*3); // not sizeof(a) because is sizeof(ptr) and is not that we want
}

//=============================================================================
void matrix_3x3_cpy(float a[3][3], float b[3][3]) // a=b
{
	memcpy(&a[0][0], &b[0][0], sizeof(a[0][0])*3*3); // not sizeof(a) because is sizeof(ptr) and is not that we want
}

//=============================================================================
void matrix_3x3_add(float a[3][3], float b[3][3])     // a=a+b
{
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			a[i][j] += b[i][j];
		}
	}
}

//=============================================================================
void matrix_3x3_sub(float a[3][3], float b[3][3])     // a=a-b
{
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			a[i][j] -= b[i][j];
		}
	}
}

//=============================================================================
void matrix_3x3_mul(float a[3][3], float b[3][3]) // a=a*b
{
	float res[3][3];

	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			float sum = 0;

			for (int k=0; k<3; k++)
			{
        		sum += a[i][k] * b[k][j];
			}

			res[i][j] = sum;
		}
	}

	matrix_3x3_cpy(a, res);
}

//=============================================================================
void matrix_3x3_print(float m[3][3])
{
	for (int i=0; i<3; i++)
	{
		vector3_print(m[i]);
	}

	putchar('\n');
}

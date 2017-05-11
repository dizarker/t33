/*
 * PID-control module.
 *
 */

#ifndef _PID_H_
#define _PID_H_

//=============================================================================
#define PID_ADC_SAMPLES_RATE  200.0f     // 200Hz

enum
{
	PID_MODE_AUTO   = 0,   // pid operates like an ordinary controller
	PID_MODE_MANUAL = 1    // pid is tracking so that it matches given inputs and outputs
};

//=============================================================================
typedef struct
{
	struct
	{
		float e[2];         // error

		struct
		{
			float p;
			float i;
			float d;
		} k;
	} math;

	struct
	{
		float  sp;
		float  dsp;         // dead band
		float  *pv;         // real pv wo any filters

		struct
		{
			float pv[3];    // pv after lowpass filter: y[n] = alpha*x[n] + (1-alpha)*y[n-1]

			struct          // is a lowpass filter coefficient aplpha=h/(RC+h), RC=0.1td => alpha=h/(0.1td+h)
			{
				float a;    // a = 0..1
				float b;    // b = 1-a
			} k;
		} lowpass;
	} in;

	struct
	{
		float  y;         // -1..0..1
		float  y_manual;  // -1..0..1
	} out;

	uint_fast8_t mode;

} pid_t;

typedef struct
{
	float xp;
	float ti;
	float td;

	float dt;

	float sp;
	float dsp;

	float* pv;

} pid_init_param_t;


//=============================================================================
void  pid_init      (pid_t* const pid, const pid_init_param_t* const ip);
float pid_algorithm (pid_t* const pid); // every X ms

#endif //_PID_H_


/*
 *  pid.h rev.3.0.0 2011-12-06
 *
 *  Contributors:
 *                Ivanov Anton (c) www.automatix.ru
 *
 *
 * PID formula:
 *        y[n] = y[n-1] + K[e[k]-e[k-1] + e[k]*h/ti + td/h * (yf[k] - 2yf[k-1] + yf[k-2])]
 *
 * lets:  yf =  lowpass_filter(PV), Tf = RC = 0.1td
 *        kp =  K
 *        ki =  K * (h/ti)
 *        kd =  K * (td/h)
 *
 * then:  y[n] = y[n-1] + kp * (e[k]-e[k-1])
 *                      + ki * e[k]
 *                      + kd * (yf[k] - 2yf[k-1] + yf[k-2])		  // p0-2p1+p2 = (p0-p1)-(p1-p2)
 */

#include "include.h"

//=============================================================================
void pid_init(pid_t* const pid, const pid_init_param_t* const ip)
{
	memset(pid, '\0', sizeof(*pid)); // clr all

	float h  =      MAX(FLT_MIN, ip->dt);
	float ti =      MAX(FLT_MIN, ip->ti);
	float td =      MAX(FLT_MIN, ip->td);
	float k  = 1.0/(MAX(FLT_MIN, ip->xp)); // kp = 1/xp

	pid->math.k.p = k;
	pid->math.k.i = k * (h/ti);
	pid->math.k.d = k * (td/h);

	float a             = h / (0.1f*td + h); // lowpass filter coefficient a=h/(RC+h), RC=0.1td => alpha=h/(0.1td+h)

	pid->in.lowpass.k.a = a;           
	pid->in.lowpass.k.b = 1.0f - a;
	pid->in.sp          = ip->sp;
	pid->in.dsp         = ip->dsp;           // dead zone
	pid->in.pv          = ip->pv;            // pointer
}

//=============================================================================
static void pid_update_err(pid_t* const pid)
{
	float e  = (pid->in.sp - *(pid->in.pv));   // heater: e[n] = SP - PV
	float de = pid->in.dsp;                    // dead zone

	if      (e>de)   e -= de;  
	else if (e<-de)  e += de;
	else             e = 0;

	pid->math.e[1] = pid->math.e[0];
	pid->math.e[0] = e;
}

//=============================================================================
static void pid_update_pv(pid_t* const pid)
{
	pid->in.lowpass.pv[2] =  pid->in.lowpass.pv[1];
	pid->in.lowpass.pv[1] =  pid->in.lowpass.pv[0];
	pid->in.lowpass.pv[0] =  pid->in.lowpass.k.a * (*pid->in.pv)
	                       + pid->in.lowpass.k.b * pid->in.lowpass.pv[1]; // y[n] = a*x[n] + b*y[n-1];  b = 1-a;
}

//=============================================================================
void pid_track(pid_t* const pid)
{

}


//=============================================================================
float pid_algorithm(pid_t* const pid) // every X ms 
{
	float y;

	pid_update_pv(pid);

	if (pid->mode==PID_MODE_AUTO) 	// normal mode
	{
		pid_update_err(pid);

		float de = pid->math.e[0] - pid->math.e[1];																	 // de = e[n] - e[n-1]

		/*y = pid->out.y + pid->math.k.p * (de * fabsf(de))
		               + pid->math.k.i * pid->math.e[0]
		               + pid->math.k.d * (pid->in.lowpass.pv[0] - 2*pid->in.lowpass.pv[1] + pid->in.lowpass.pv[2]);  // de = dpv is for bumpless SP changes*/
		y = pid->out.y + pid->math.k.p * de
		               + pid->math.k.i * pid->math.e[0]
		               + pid->math.k.d * (pid->in.lowpass.pv[0] - 2*pid->in.lowpass.pv[1] + pid->in.lowpass.pv[2]);  // de = dpv is for bumpless SP changes
	}
	else // pid is tracking so that it matches given inputs and outputs
	{
		pid->in.lowpass.pv[0] = pid->in.lowpass.pv[1] = pid->in.lowpass.pv[2] = pid->in.sp = *pid->in.pv; // sp=pv

		pid->math.e[0] = 0;
		y              = pid->out.y_manual;
	}

	if (fabsf(y)>1.0f)           // Limiter
	{
		y = copysignf(1.0f,y);
	}

	return (pid->out.y = pid->out.y_manual = y);  // sync both to bumpless transfer (between AUTO and MANUAL modes)
}



#include "include.h"

uint16_t count_pid(pidSimple_t* const pid)
{	
		//for P
		pid->de = *pid->set_value - *pid->temp_value;
	
		// for D
		pid->temp_pde[1] = pid->temp_pde[0];
		pid->temp_pde[0] = pid->de;
				
		// check impact
		if((pid->de  > (pid->dsp)) || (pid->de < (-(pid->dsp))))
		{
			pid->impact  = 
			//P
			pid->impact	+ (int)(pid->de * pid->p)+ 
			//D			
			(int)((pid->temp_pde[0] - pid->temp_pde[1]) * pid->d);			
		}
		
		if(pid->impact > pid->MAX_IMPACT)
		{
			 pid->impact = pid->MAX_IMPACT;
		}	
		
	return pid->impact;		
}


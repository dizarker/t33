
#ifndef _PID_SIMPLE_H_
#define _PID_SIMPLE_H_
	
	typedef struct
	{
		float temp_cde[2];
		float temp_pde[2];			
		
		float *set_value;	  //pressure
		float *temp_value;
		
		float p;  		//P for pressure	 	
		float d;	 	  //D for pressure
							
		float dsp;			//dead zone
		
		float de;   			//delta error
		
		uint16_t impact;  		//impact for pressure
		uint16_t MAX_IMPACT;  	//limiter for timer
	} pidSimple_t;


uint16_t count_pid(pidSimple_t* const pid);

#endif 
	


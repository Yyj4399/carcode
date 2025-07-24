#include "zf_common_headfile.h"
#include "PID.h"

float motor_pid_r[2] = {50,13};												//����PID��������
float motor_pid_l[2] = {50,13};												//����PID��������
						
float Error_l=0;
float LastError_l=0;
						
float Error_r=0;
float LastError_r=0;
	
float Out_I_l=0;
float Out_P_l=0;
int Out_l=0;

float Out_I_r=0;
float Out_P_r=0;
int Out_r=0;


//����ʽPID
float PID_increase_l(float Kp,float Ki,float Nowdata,float point){
		
	Error_l = point - Nowdata;
	Out_P_l = Error_l;
	Out_I_l += Error_l;
	
	if(Out_I_l>=pwm_I_limit){
		Out_I_l=pwm_I_limit;
	}
	
	if(Out_I_l<=-pwm_I_limit){
		Out_I_l=-pwm_I_limit;
	}
	
	Out_l=(int)(Kp*Out_P_l+Ki*Out_I_l);
	
	if(Out_l>=pwm_limit){
		Out_l=pwm_limit;
	}
	
	if(Out_l<=-pwm_limit){
		Out_l=-pwm_limit;
	}
	
	LastError_l =Error_l;
	
	return Out_l;
}

float PID_increase_r(float Kp,float Ki,float Nowdata,float point){	
	
	Error_r = point - Nowdata;
	Out_P_r = Error_r;
	Out_I_r += Error_r;
	
	if(Out_I_r>=pwm_I_limit){
		Out_I_r=pwm_I_limit;
	}
	
	if(Out_I_r<=-pwm_I_limit){
		Out_I_r=-pwm_I_limit;
	}
	
	Out_r =(int)(Kp*Out_P_r+Ki*Out_I_r);
	
	if(Out_r>=pwm_limit){
		Out_r=pwm_limit;
	}
	
	if(Out_r<=-pwm_limit){
		Out_r=-pwm_limit;
	}
	
	LastError_r =Error_r;
	
	return Out_r;
}


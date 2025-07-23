#include "zf_common_headfile.h"
#include "PID.h"

float motor_pid_r[2] = {32,2};												//左轮PID参数设置
float motor_pid_l[2] = {32,2};												//右轮PID参数设置
						
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


//增量式PID
float PID_increase_l(float Kp,float Ki,float Nowdata,float point){
		
	Error_l = point - Nowdata;
	Out_P_l = Error_l - LastError_l;
	Out_I_l = Error_l;
	
	Out_l+=(int)(Kp*Out_P_l+Ki*Out_I_l);
	
	if(Out_l>=pwm_limit){
		Out_l=pwm_limit;
	}
	
	if(Out_l<=-pwm_limit){
		Out_l=-pwm_limit;
	}
	
	LastError_l = 0.9*Error_l + 0.1*LastError_l;
	
	return Out_l;
}

float PID_increase_r(float Kp,float Ki,float Nowdata,float point){	
	
	Error_r = point - Nowdata;
	Out_P_r = Error_r - LastError_r;
	Out_I_r = Error_r;
	
	Out_r+=(int)(Kp*Out_P_r+Ki*Out_I_r);
	
	if(Out_r>=pwm_limit){
		Out_r=pwm_limit;
	}
	
	if(Out_r<=-pwm_limit){
		Out_r=-pwm_limit;
	}
	
	LastError_r = 0.9*Error_r + 0.1*LastError_r;
	
	return Out_r;
}


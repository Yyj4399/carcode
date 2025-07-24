#ifndef __PID_H__
#define __PID_H__

float PID_increase_l(float Kp,float Ki,float Nowdata,float point);											//增量式PID
float PID_increase_r(float Kp,float Ki,float Nowdata,float point);											//增量式PID

#define pwm_limit 6000																																				//编码器限幅
#define pwm_I_limit 100																																				


extern float motor_pid_l[2];
extern float motor_pid_r[2];
							
extern float Error_l;
extern float LastError_l;
						
extern float Error_r;
extern float LastError_r;
	
extern float Out_I_l;
extern float Out_P_l;

extern float Out_I_r;
extern float Out_P_r;

#endif


#ifndef __PID_H__
#define __PID_H__

typedef struct PID{                  //PID参数定义
	float PrevError;						
	float Error;
	float LastError;
	float LastData;
	
	float Out_I;
	float Out_D;
	float Out_P;
	
	float Kp;
	float Ki;
	float Kd;
	float LowPass;
	
}PID;

extern struct PID motor_pid_l;
extern struct PID motor_pid_r;

float PID_increase(PID *PID,float Nowdata,float point);											//增量式PID

#endif


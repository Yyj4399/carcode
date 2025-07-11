#include "zf_common_headfile.h"
#include "PID.h"

PID motor_pid_l = {0,0,0,0,0,0,0,10,2,0,1};												//左轮PID参数设置
PID motor_pid_r = {0,0,0,0,0,0,0,10,2,0,1};												//右轮PID参数设置

//增量式PID
float PID_increase(PID *PID,float Nowdata,float point){
	
	PID->Error = point - Nowdata;
	PID->Out_P = (PID->Error - PID->LastError);
	PID->Out_I = PID->Error;
	
	PID->Out_D = (PID->Error - 2*PID->LastError + PID->PrevError);
	
	PID->PrevError = 0.9*PID->LastError + 0.1*PID->PrevError;
	PID->LastError = 0.9*PID->Error + 0.1*PID->LastError;
	PID->LastData = Nowdata;
	
	return (PID->Kp*PID->Out_P+PID->Ki*PID->Out_I+PID->Kd*PID->Out_D);
}


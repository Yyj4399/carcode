#include "zf_common_headfile.h"
#include "PID.h"

float motor_pid_r[2] = {50,13};												//����PID��������
float motor_pid_l[2] = {50,13};												//����PID��������
						
pid pid_l={0};			//����PID��������
pid pid_r={0};			//�ҵ��PID��������


//λ��ʽPID
float PID_l(float Kp,float Ki,float Nowdata,float point){
		
	pid_l.Error = point - Nowdata;
	pid_l.Out_P = pid_l.Error;
	pid_l.Out_I += pid_l.Error;
	//I���޷�
	if(pid_l.Out_I>=pwm_I_limit){
		pid_l.Out_I=pwm_I_limit;
	}
	
	if(pid_l.Out_I<=-pwm_I_limit){
		pid_l.Out_I=-pwm_I_limit;
	}
	//����PID
	pid_l.Out=(int)(Kp*pid_l.Out_P+Ki*pid_l.Out_I);
	//PID���޷�
	if(pid_l.Out>=pwm_limit){
		pid_l.Out=pwm_limit;
	}
	
	if(pid_l.Out<=-pwm_limit){
		pid_l.Out=-pwm_limit;
	}
	
	pid_l.LastError = pid_l.Error;
	
	return pid_l.Out;
}

float PID_r(float Kp,float Ki,float Nowdata,float point){	
	
	pid_r.Error = point - Nowdata;
	pid_r.Out_P = pid_r.Error;
	pid_r.Out_I += pid_r.Error;
	
	if(pid_r.Out_I>=pwm_I_limit){
		pid_r.Out_I=pwm_I_limit;
	}
	
	if(pid_r.Out_I<=-pwm_I_limit){
		pid_r.Out_I=-pwm_I_limit;
	}
	
	pid_r.Out =(int)(Kp*pid_r.Out_P+Ki*pid_r.Out_I);
	
	if(pid_r.Out>=pwm_limit){
		pid_r.Out=pwm_limit;
	}
	
	if(pid_r.Out<=-pwm_limit){
		pid_r.Out=-pwm_limit;
	}
	
	pid_r.LastError =pid_r.Error;
	
	return pid_r.Out;
}


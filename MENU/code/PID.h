#ifndef __PID_H__
#define __PID_H__

float PID_l(float Kp,float Ki,float Nowdata,float point);											//增量式PID
float PID_r(float Kp,float Ki,float Nowdata,float point);											//增量式PID

#define pwm_limit 8000																							//编码器限幅
#define pwm_I_limit 100																							//PID的I项限幅																															

typedef struct pid{	//PID参数定义
	
	float Error;					//本次速度误差
	float LastError;			//上一次速度误差
	float Out_I;					//速度环I项输出
	float Out_P;					//速度环P项输出
	int Out;							//速度环总输出

}pid;

extern float motor_pid_l[2];		//速度环PID参数
extern float motor_pid_r[2];		//速度环PID参数

extern struct pid pid_l;				//左电机PID参数结构体
extern struct pid pid_r;				//右电机PID参数结构体


#endif


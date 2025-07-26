#ifndef __PID_H__
#define __PID_H__

float PID_l(float Kp,float Ki,float Nowdata,float point);											//����ʽPID
float PID_r(float Kp,float Ki,float Nowdata,float point);											//����ʽPID

#define pwm_limit 8000																							//�������޷�
#define pwm_I_limit 100																							//PID��I���޷�																															

typedef struct pid{	//PID��������
	
	float Error;					//�����ٶ����
	float LastError;			//��һ���ٶ����
	float Out_I;					//�ٶȻ�I�����
	float Out_P;					//�ٶȻ�P�����
	int Out;							//�ٶȻ������

}pid;

extern float motor_pid_l[2];		//�ٶȻ�PID����
extern float motor_pid_r[2];		//�ٶȻ�PID����

extern struct pid pid_l;				//����PID�����ṹ��
extern struct pid pid_r;				//�ҵ��PID�����ṹ��


#endif


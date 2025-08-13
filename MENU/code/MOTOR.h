#ifndef __MOTOR_H
#define __MOTOR_H

void PWM_Init();																																							//PWM��ʼ��
void Encoder_Init();																																					//��������ʼ��

void car_protect(uint8 bio_image[MT9V03X_H][MT9V03X_W]);																			//���籣��
void Encoder_Get();																																						//������ֵ��ȡ
void Speed_Set(pwm_channel_enum pin1,gpio_pin_enum pin2,int Speed,uint8 just,uint8 lose);			//���õ���ٶ�

void Motor_Control(int Speed_L,int Speed_R);																									//�������
void Final_Motor_Control(float k,float d,int limit);																					//���ٿ���

void car_start();																																							//��������

int32 limit_int(int32 a,int32 b,int32 c);																											//�����޷�����







#define pwm_l TIM5_PWM_CH2_A1									//����PWM���Ŷ���
#define pwm_r TIM5_PWM_CH4_A3									//�ҵ��PWM���Ŷ���








typedef struct motor1{			//�����������
	
	int32 target_speed;				//Ŀ���ٶ�
	int32 duty;								//����ٶȻ�PWM���
	int32 encoder_speed;			//����������
	int32 encoder_raw;				//��һ�α���������
	int32 total_encoder;			//�����������ܺ�
	int32 motor_v;						//PWM�������
	
}motor1;

typedef struct PD{					//PD��������
	
	int error;								//������ֵ���
	int last_error;						//��һ����ֵ���
	int PD_v;									//�������
		
}PD;







extern struct motor1 motor_l;			//�����ṹ��																											
extern struct motor1 motor_r;			//�ҵ���ṹ��

extern uint8 car_num;							//��������
extern float p;										//����kp
extern float d;										//����kd
extern int32 high_speed;					//���ٶ�
extern int32 low_speed;						//���ٶ�
extern int32 speed;								//Ŀ���ٶ�

#endif
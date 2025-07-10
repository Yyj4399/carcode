#ifndef __MOTOR_H
#define __MOTOR_H

void PWM_Init();																																							//PWM��ʼ��
void Encoder_Init();																																					//��������ʼ��
void car_protect(uint8 bio_image[MT9V03X_H][MT9V03X_W]);																			//���籣��
void Encoder_Get();																																						//������ֵ��ȡ
void Speed_Set(pwm_channel_enum pin1,gpio_pin_enum pin2,int Speed,uint8 just,uint8 lose);			//���õ���ٶ�
void Motor_Control(int Speed_L,int Speed_R);																									//�������
void car_start();																																							//��������


#define pwm_limit 6000																																				//�������޷�
#define pwm_l TIM4_PWM_CH1_B6
#define pwm_r TIM3_PWM_CH1_B4

typedef struct motor1{																																				//�����������
	int32 target_speed;
	int32 duty;
	int32 encoder_speed;
	int32 encoder_raw;
	int32 total_encoder;
}motor1;

extern struct motor1 motor_l;																														
extern struct motor1 motor_r;


#endif
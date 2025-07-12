#ifndef __MOTOR_H
#define __MOTOR_H

void PWM_Init();																																							//PWM初始化
void Encoder_Init();																																					//编码器初始化
void car_protect(uint8 bio_image[MT9V03X_H][MT9V03X_W]);																			//出界保护
void Encoder_Get();																																						//编码器值获取
void Speed_Set(pwm_channel_enum pin1,gpio_pin_enum pin2,int Speed,uint8 just,uint8 lose);			//设置电机速度
void Motor_Control(int Speed_L,int Speed_R);																									//电机控制
void Final_Motor_Control(int Speed,float k,int limit);																				//差速控制
void car_start();																																							//发车函数



#define pwm_l TIM5_PWM_CH4_A3
#define pwm_r TIM5_PWM_CH2_A1

typedef struct motor1{																																				//电机参数定义
	int32 target_speed;
	int32 duty;
	int32 encoder_speed;
	int32 encoder_raw;
	int32 total_encoder;
}motor1;

extern struct motor1 motor_l;																														
extern struct motor1 motor_r;
extern uint8 car_num;


#endif
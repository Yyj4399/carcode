#ifndef __MOTOR_H
#define __MOTOR_H

void PWM_Init();																																							//PWM初始化
void Encoder_Init();																																					//编码器初始化

void car_protect(uint8 bio_image[MT9V03X_H][MT9V03X_W]);																			//出界保护
void Encoder_Get();																																						//编码器值获取
void Speed_Set(pwm_channel_enum pin1,gpio_pin_enum pin2,int Speed,uint8 just,uint8 lose);			//设置电机速度

void Motor_Control(int Speed_L,int Speed_R);																									//电机控制
void Final_Motor_Control(float k,float d,int limit);																					//差速控制

void car_start();																																							//发车函数

int32 limit_int(int32 a,int32 b,int32 c);																											//整形限幅函数







#define pwm_l TIM5_PWM_CH2_A1									//左电机PWM引脚定义
#define pwm_r TIM5_PWM_CH4_A3									//右电机PWM引脚定义








typedef struct motor1{			//电机参数定义
	
	int32 target_speed;				//目标速度
	int32 duty;								//电机速度环PWM输出
	int32 encoder_speed;			//编码器读数
	int32 encoder_raw;				//上一次编码器读数
	int32 total_encoder;			//编码器读数总和
	int32 motor_v;						//PWM最终输出
	
}motor1;

typedef struct PD{					//PD参数定义
	
	int error;								//本次中值误差
	int last_error;						//上一次中值误差
	int PD_v;									//方向环输出
		
}PD;







extern struct motor1 motor_l;			//左电机结构体																											
extern struct motor1 motor_r;			//右电机结构体

extern uint8 car_num;							//发车计数
extern float p;										//方向环kp
extern float d;										//方向环kd
extern int32 high_speed;					//高速度
extern int32 low_speed;						//低速度
extern int32 speed;								//目标速度

#endif
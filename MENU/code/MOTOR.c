#include "zf_common_headfile.h"
#include "MOTOR.H"
#include "PID.H"
#include "image.h"
#include "menu.h"

motor1 motor_l;						//定义左轮参数
motor1 motor_r;						//定义右轮参数
uint8 car_num=0;					//用来存储发车次数

//PWM初始化
void PWM_Init(){
	
	gpio_init(A0,GPO,GPIO_HIGH,GPO_PUSH_PULL);
	pwm_init(pwm_l,17000,0);
	gpio_init(A2,GPO,GPIO_HIGH,GPO_PUSH_PULL);
	pwm_init(pwm_r,17000,0);
	
}

//编码器初始化
void Encoder_Init(){
	
	encoder_quad_init(TIM4_ENCODER, TIM4_ENCODER_CH1_B6, TIM4_ENCODER_CH2_B7);
	encoder_quad_init(TIM3_ENCODER, TIM3_ENCODER_CH1_B4, TIM3_ENCODER_CH2_B5);
	
}

//出界保护
void car_protect(uint8 bio_image[MT9V03X_H][MT9V03X_W]){
	uint8 num=0;
	for(uint8 i=3;i<MT9V03X_W-2;i++){
		if(bio_image[bottom_line][i]==255){
				num++;
		}
	}
	if(num<=60){
		Speed_Set(pwm_l,A0,0,0,1);
		Speed_Set(pwm_r,A2,0,0,1);
	}
}

//编码器值获取
void Encoder_Get(){
	
	//左轮编码器值获取
	motor_l.encoder_raw = encoder_get_count(TIM4_ENCODER);
	motor_l.encoder_speed=motor_l.encoder_speed*0.2+motor_l.encoder_raw*0.8;
	motor_l.total_encoder+=motor_l.encoder_raw;
	encoder_clear_count(TIM4_ENCODER);
	
	//右轮编码器值获取
	motor_r.encoder_raw = encoder_get_count(TIM3_ENCODER);
	motor_r.encoder_speed=-(motor_r.encoder_speed*0.2+motor_r.encoder_raw*0.8);
	motor_r.total_encoder+=motor_r.encoder_raw;
	encoder_clear_count(TIM3_ENCODER);
	
}

//限幅函数
uint8 limit_int(int a,int b,int c){
	if(b>=a&&b<=c){
		return b;
	}
	else if(b<=a){
		return a;
	}
	else if(b>=c){
		return c;
	}
	return 0;
}

//设置电机速度
void Speed_Set(pwm_channel_enum pin1,gpio_pin_enum pin2,int Speed,uint8 just,uint8 lose){
	if(Speed>0){
		
		pwm_set_duty(pin1,Speed);
		gpio_set_level(pin2,just);
		
	}
	else {
		pwm_set_duty(pin1,-Speed);
		gpio_set_level(pin2,lose);
	}
}

//电机控制
void Motor_Control(int Speed_L,int Speed_R){
	
	//输入目标速度
	motor_l.target_speed = Speed_L;
	motor_r.target_speed = Speed_R;
	
	//PID控制
	motor_l.duty = PID_increase_l(motor_pid_l[0],motor_pid_l[1],(float)motor_l.encoder_speed,(float)motor_l.target_speed);
	motor_r.duty = PID_increase_r(motor_pid_r[0],motor_pid_r[1],(float)motor_r.encoder_speed,(float)motor_r.target_speed);
	
	//输出速度
	Speed_Set(pwm_l,A2,motor_l.duty,1,0);
	Speed_Set(pwm_r,A0,motor_r.duty,1,0);

	
}

//差速控制
void Final_Motor_Control(int Speed,float k,int limit){
	
	Motor_Control(limit_int(Speed-limit,Speed-k*(MID_W-final_mid_value),Speed+limit),limit_int(Speed-limit,Speed+k*(MID_W-final_mid_value),Speed+limit));
}

//车辆启动
void car_start(){
	
	if(KeyNumber==4){
		car_num++;
	}
	if(car_num!=0){
		if(car_num%2==1){
			Motor_Control(100,100);
//			Speed_Set(pwm_l,A2,1000,1,0);
//			Speed_Set(pwm_r,A0,1000,1,0);
		}
		else{
//			Motor_Control(20,20);
			Speed_Set(pwm_l,A2,0,0,1);
			Speed_Set(pwm_r,A0,0,0,1);
		}
	}
}


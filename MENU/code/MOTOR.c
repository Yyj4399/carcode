#include "zf_common_headfile.h"
#include "MOTOR.H"
#include "PID.H"
#include "image.h"
#include "menu.h"

motor1 motor_l;						//定义左轮参数定义
motor1 motor_r;						//定义右轮参数定义
PD pd;										//方向环参数定义
uint8 car_num=0;					//用来存储发车次数
float p=230;							//方向环kp     //210，110，100（11.9V）；230，180，100（摄像头改动后，12.0V）
float d=180;							//方向环kd
int32 speed=100;					//目标速度

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

uint8 car_protect_flag=0;

//出界保护
void car_protect(uint8 bio_image[MT9V03X_H-40][MT9V03X_W]){
	
	uint8 num=0;
	uint8 num1=0;

	for(uint8 i=3;i<MT9V03X_W-2;i++){
		
		if(bio_image[bottom_line][i]==255){
			
				num++;
			
		}
		if(bio_image[bottom_line][i-1]==255&&bio_image[bottom_line][i]==255&&bio_image[bottom_line][i+1]==0){
			
			num1++;
			
		}
		
	}
	
	if(num1>=5){
		
		car_protect_flag++;
		
	}
	if(num<=30||car_protect_flag>=25){
		
		Speed_Set(pwm_l,A0,0,1,0);
		Speed_Set(pwm_r,A2,0,1,0);
		
		while(1){
		}
		
	}
	
}

//编码器值获取
void Encoder_Get(){
	
	//左轮编码器值获取
	motor_l.encoder_raw = encoder_get_count(TIM4_ENCODER);
	motor_l.encoder_speed=motor_l.encoder_speed*0.2+motor_l.encoder_raw*0.8;
//	motor_l.total_encoder+=motor_l.encoder_raw;
	encoder_clear_count(TIM4_ENCODER);
	
	//右轮编码器值获取
	motor_r.encoder_raw = -encoder_get_count(TIM3_ENCODER);
	motor_r.encoder_speed=motor_r.encoder_speed*0.2+motor_r.encoder_raw*0.8;
//	motor_r.total_encoder+=motor_r.encoder_raw;
	encoder_clear_count(TIM3_ENCODER);
	
}

//限幅函数
int32 limit_int(int32 a,int32 b,int32 c){
	if(b>=a&&b<=c){
		return b;
	}
	else if(b<a){
		return a;
	}
	else if(b>c){
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
	motor_l.duty = PID_l(motor_pid_l[0],motor_pid_l[1],(float)motor_l.encoder_speed,(float)motor_l.target_speed);
	motor_r.duty = PID_r(motor_pid_r[0],motor_pid_r[1],(float)motor_r.encoder_speed,(float)motor_r.target_speed);
	
//	//输出速度
//	Speed_Set(pwm_l,A2,motor_l.duty,1,0);
//	Speed_Set(pwm_r,A0,motor_r.duty,1,0);

	
}

//差速控制
void Final_Motor_Control(float k,float d,int32 limit){
	//方向环计算
	pd.error = MID_W-final_mid_value;
	pd.PD_v=(int)(p*pd.error+d*(pd.error-pd.last_error));
	//方向环限幅
	pd.PD_v=limit_int(-limit,pd.PD_v,limit);
	//PWM总输出限幅和计算
	motor_l.motor_v=limit_int(-pwm_limit,motor_l.duty-pd.PD_v,pwm_limit);
	motor_r.motor_v=limit_int(-pwm_limit,motor_r.duty+pd.PD_v,pwm_limit);

//	motorl=limit_int(-pwm_limit,motor_l.duty-PD,pwm_limit);
//	motorr=limit_int(-pwm_limit,motor_r.duty+PD,pwm_limit);
	
	//给电机PWM	
	Speed_Set(pwm_l,A2,motor_l.motor_v,1,0);
	Speed_Set(pwm_r,A0,motor_r.motor_v,1,0);
	
//		Motor_Control(Speed,limit_int(Speed-limit,Speed+k*error+d*last_error,Speed+limit));
//		Motor_Control(limit_int(Speed-limit,Speed-k*error-d*last_error,Speed+limit),Speed);
	
	//更新变量
	pd.last_error=pd.error;
	
}

//车辆启动
void car_start(){
	
	if( KEY_SHORT_PRESS == key_get_state(KEY_4)&&main_menu_position==0){
		
		//发车计数累加
		car_num++;
		
		//清空按键状态
		key_clear_state(KEY_4);
		
	}
	if(car_num!=0){
		
		if(car_num%2==1){
//			if(final_mid_value-MID_W>=2||MID_W-final_mid_value>=2){
//				speed=200;
////				motor_pid_l[0]=225;
////				motor_pid_l[1]=160;
//			}
//			else{
//				speed=250;
////				motor_pid_l[0]=205;
////				motor_pid_l[1]=160;
//			}
//			Motor_Control(speed,speed);
//			Final_Motor_Control(k,d,7000);
//			car_protect(image);												

//			Speed_Set(pwm_l,A2,-1000,1,0);
//			Speed_Set(pwm_r,A0,1000,1,0);
		}
		
		else{
//			Motor_Control(20,20);
			
			Speed_Set(pwm_l,A2,0,1,0);
			Speed_Set(pwm_r,A0,0,1,0);
			
		}
		
	}
	else{
		
		//按键操作
		handle();
		
		//显示菜单
		print_menu();
		
	}
	
}


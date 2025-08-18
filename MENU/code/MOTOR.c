#include "zf_common_headfile.h"
#include "MOTOR.H"
#include "PID.H"
#include "image.h"
#include "menu.h"

motor1 motor_l={0};						//¶¨Òå×óÂÖ²ÎÊı¶¨Òå
motor1 motor_r={0};						//¶¨ÒåÓÒÂÖ²ÎÊı¶¨Òå
PD pd={0};								//·½Ïò»·²ÎÊı¶¨Òå
uint8 car_num=0;					//ÓÃÀ´´æ´¢·¢³µ´ÎÊı
float p=350;							//·½Ïò»·kp     //240£¨+10£©£¬520£¬120£¨12.2V£¬ÂúµçÒ²¿É£©£»250£¬660£¬130£¨12.0V£©;350,440,150,100£¨»»ÅäÖØ¿éºó£¬12.1V£©£»390£¬450£¬200£¬100£¨ÂúµçÒ²¿É£©£»360£¬400£¬200£¬110£¨»»Ì¥ºó£©
float d=400;							//·½Ïò»·kd
int32 high_speed=200;			//¸ßËÙ¶È
int32 low_speed=100;			//µÍËÙ¶È
int32 circle_speed=100;		//Ô²»·ËÙ¶È
int32 speed;							//Ä¿±êËÙ¶È


//PWM³õÊ¼»¯
void PWM_Init(){
	
	pwm_init(pwm_l1,17000,0);
	pwm_init(pwm_l2,17000,0);
	
	pwm_init(pwm_r1,17000,0);
	pwm_init(pwm_r2,17000,0);
	
}

//±àÂëÆ÷³õÊ¼»¯
void Encoder_Init(){
	
	encoder_quad_init(TIM4_ENCODER, TIM4_ENCODER_CH1_B6, TIM4_ENCODER_CH2_B7);
	
	encoder_quad_init(TIM3_ENCODER, TIM3_ENCODER_CH1_B4, TIM3_ENCODER_CH2_B5);
	
}

uint8 car_protect_flag=0;					//°ßÂíÏß±êÖ¾Î»

//³ö½ç±£»¤
void car_protect(uint8 bio_image[MT9V03X_H][MT9V03X_W]){
	
	uint8 num=0;
	uint8 banma_num=0;

	for(uint8 i=3;i<MT9V03X_W-2;i++){
		
		//É¨Ãèµ×ÏßºÚÉ«ÏñËØ
		if(image[bottom_line][i]==255){
			
				num++;
			
		}
		
		//É¨Ãèµ×ÏßºÚ°×Ìø±äµ
		if(image[bottom_line][i-1]==255&&image[bottom_line][i]==255&&image[bottom_line][i+1]==0){
			
			banma_num++;
			
		}
		
	}
	
	switch(car_protect_flag){
	
		case 0:
			
			//ÈôºÚ°×Ìø±äµãÊıÁ¿´óÓÚ5Ôò¼Æ°ßÂíÏßÊı
			if(banma_num>=5){
		
				car_protect_flag=1;
		
			}
			
			break;
			
		case 1:
			
			if(banma_num<=2){
				
				car_protect_flag=2;
			
			}
			
			break;
			
		case 2:
			
			//ÈôºÚ°×Ìø±äµãÊıÁ¿´óÓÚ5Ôò¼Æ°ßÂíÏßÊı
			if(banma_num>=5){
		
				car_protect_flag=3;
		
			}
		
			break;
	
	}
	
	//Èô³ö½ç»òÕß°ßÂíÏßÊı´ïµ½Ò»¶¨ÊıÁ¿ÔòÍ£³µ
	if(num<=30||car_protect_flag==3){ 
		
		car_protect_flag=0;
		
		pwm_set_duty(pwm_l1,0);
		pwm_set_duty(pwm_l2,0);
		pwm_set_duty(pwm_r1,0);
		pwm_set_duty(pwm_r2,0);
		
		while(1){
		}
		
	}
	
}

//±àÂëÆ÷Öµ»ñÈ¡
void Encoder_Get(){
	
	//×óÂÖ±àÂëÆ÷Öµ»ñÈ¡
	motor_l.encoder_raw = encoder_get_count(TIM4_ENCODER);
	motor_l.encoder_speed=motor_l.encoder_speed*0.2+motor_l.encoder_raw*0.8;
//	motor_l.total_encoder+=motor_l.encoder_raw;
	encoder_clear_count(TIM4_ENCODER);
	
	//ÓÒÂÖ±àÂëÆ÷Öµ»ñÈ¡
	motor_r.encoder_raw = -encoder_get_count(TIM3_ENCODER);
	motor_r.encoder_speed=motor_r.encoder_speed*0.2+motor_r.encoder_raw*0.8;
//	motor_r.total_encoder+=motor_r.encoder_raw;
	encoder_clear_count(TIM3_ENCODER);
	
}

//ÏŞ·ùº¯Êı
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

//ÉèÖÃµç»úËÙ¶È
void Speed_Set(pwm_channel_enum pin1,pwm_channel_enum pin2,int Speed){
	
	if(Speed>=0){
		
		pwm_set_duty(pin1,Speed);
		pwm_set_duty(pin2,0);
		
	}
	
	else {
		
		pwm_set_duty(pin1,0);
		pwm_set_duty(pin2,-Speed);
		
	}
	
}

//µç»ú¿ØÖÆ
void Motor_Control(int Speed_L,int Speed_R){
	
	//ÊäÈëÄ¿±êËÙ¶È
	motor_l.target_speed = Speed_L;
	motor_r.target_speed = Speed_R;
	
	//PID¿ØÖÆ
	motor_l.duty = PID_l(motor_pid_l[0],motor_pid_l[1],(float)motor_l.encoder_speed,(float)motor_l.target_speed);
	motor_r.duty = PID_r(motor_pid_r[0],motor_pid_r[1],(float)motor_r.encoder_speed,(float)motor_r.target_speed);
	
//	//Êä³öËÙ¶È
//	Speed_Set(pwm_l1,pwm_l2,motor_l.duty);
//	Speed_Set(pwm_r1,pwm_r2,motor_r.duty);

	
}

//²îËÙ¿ØÖÆ
void Final_Motor_Control(float k,float d,int32 limit){
	
	//·½Ïò»·¼ÆËã
	pd.error = MID_W-final_mid_value;
	pd.PD_v=(int)(p*pd.error+d*(pd.error-pd.last_error));
	
	//·½Ïò»·ÏŞ·ù
	pd.PD_v=limit_int(-limit,pd.PD_v,limit);
	
	//PWM×ÜÊä³öÏŞ·ùºÍ¼ÆËã
	motor_l.motor_v=limit_int(-pwm_limit,motor_l.duty-pd.PD_v,pwm_limit);
	motor_r.motor_v=limit_int(-pwm_limit,motor_r.duty+pd.PD_v,pwm_limit);

//	motorl=limit_int(-pwm_limit,motor_l.duty-PD,pwm_limit);
//	motorr=limit_int(-pwm_limit,motor_r.duty+PD,pwm_limit);
	
	//¸øµç»úPWM	
	Speed_Set(pwm_l1,pwm_l2,motor_l.motor_v);
	Speed_Set(pwm_r1,pwm_r2,motor_r.motor_v);
	
	//¸üĞÂÉÏÒ»´ÎÎó²î
	pd.last_error=pd.error;
	
}

//³µÁ¾Æô¶¯
void car_start(){
	
	if( KEY_SHORT_PRESS == key_get_state(KEY_4)&&main_menu_position==0){
		
		//·¢³µ¼ÆÊıÀÛ¼Ó
		car_num++;
		
		//Çå¿Õ°´¼ü×´Ì¬
		key_clear_state(KEY_4);
		
	}
	
	//·¢³µºó
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

//			Speed_Set(pwm_l,A2,-1000,0,1);
//			Speed_Set(pwm_r,A0,1000,1,0);
		}
		
		else{
//			Motor_Control(20,20);
			
			pwm_set_duty(pwm_l1,0);
			pwm_set_duty(pwm_l2,0);
			pwm_set_duty(pwm_r1,0);
			pwm_set_duty(pwm_r2,0);
			
		}
		
	}
	//Î´·¢³µÊ±
	else{
		
		//°´¼ü²Ù×÷
		handle();
		
		//ÏÔÊ¾²Ëµ¥
		print_menu();
		
	}
	
}


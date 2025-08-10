#include "zf_common_headfile.h"
#include "MOTOR.H"
#include "PID.H"
#include "image.h"
#include "menu.h"

motor1 motor_l={0};						//�������ֲ�������
motor1 motor_r={0};						//�������ֲ�������
PD pd={0};								//���򻷲�������
uint8 car_num=0;					//�����洢��������
float p=360;							//����kp     //240��+10����520��120��12.2V������Ҳ�ɣ���250��660��130��12.0V��;350,440,150,100�������ؿ��12.1V����390��450��200��100������Ҳ�ɣ���360��400��200��110����̥��
float d=400;							//����kd
int32 high_speed=200;			//���ٶ�
int32 low_speed=110;			//���ٶ�
int32 speed;							//Ŀ���ٶ�


//PWM��ʼ��
void PWM_Init(){
	
	gpio_init(A0,GPO,GPIO_HIGH,GPO_PUSH_PULL);
	pwm_init(pwm_l,17000,0);
	
	gpio_init(A2,GPO,GPIO_HIGH,GPO_PUSH_PULL);
	pwm_init(pwm_r,17000,0);
	
}

//��������ʼ��
void Encoder_Init(){
	
	encoder_quad_init(TIM4_ENCODER, TIM4_ENCODER_CH1_B6, TIM4_ENCODER_CH2_B7);
	
	encoder_quad_init(TIM3_ENCODER, TIM3_ENCODER_CH1_B4, TIM3_ENCODER_CH2_B5);
	
}

uint8 car_protect_flag=0;

//���籣��
void car_protect(uint8 bio_image[MT9V03X_H][MT9V03X_W]){
	
	uint8 num=0;
	uint8 num1=0;

	for(uint8 i=3;i<MT9V03X_W-2;i++){
		
		//ɨ����ߺ�ɫ����
		if(bio_image[bottom_line-10][i]==255){
			
				num++;
			
		}
		
		//ɨ����ߺڰ������
		if(bio_image[bottom_line][i-1]==255&&bio_image[bottom_line][i]==255&&bio_image[bottom_line][i+1]==0){
			
			num1++;
			
		}
		
	}
	
	//���ڰ��������������5��ư�������
	if(num1>=5){
		
		car_protect_flag++;
		
	}
	
	//��������߰��������ﵽһ��������ͣ��
	if(num<=30||car_protect_flag>=30){
		
		Speed_Set(pwm_l,A0,0,0,1);
		Speed_Set(pwm_r,A2,0,0,1);
		
		while(1){
		}
		
	}
	
}

//������ֵ��ȡ
void Encoder_Get(){
	
	//���ֱ�����ֵ��ȡ
	motor_l.encoder_raw = encoder_get_count(TIM4_ENCODER);
	motor_l.encoder_speed=motor_l.encoder_speed*0.2+motor_l.encoder_raw*0.8;
//	motor_l.total_encoder+=motor_l.encoder_raw;
	encoder_clear_count(TIM4_ENCODER);
	
	//���ֱ�����ֵ��ȡ
	motor_r.encoder_raw = -encoder_get_count(TIM3_ENCODER);
	motor_r.encoder_speed=motor_r.encoder_speed*0.2+motor_r.encoder_raw*0.8;
//	motor_r.total_encoder+=motor_r.encoder_raw;
	encoder_clear_count(TIM3_ENCODER);
	
}

//�޷�����
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

//���õ���ٶ�
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

//�������
void Motor_Control(int Speed_L,int Speed_R){
	
	//����Ŀ���ٶ�
	motor_l.target_speed = Speed_L;
	motor_r.target_speed = Speed_R;
	
	//PID����
	motor_l.duty = PID_l(motor_pid_l[0],motor_pid_l[1],(float)motor_l.encoder_speed,(float)motor_l.target_speed);
	motor_r.duty = PID_r(motor_pid_r[0],motor_pid_r[1],(float)motor_r.encoder_speed,(float)motor_r.target_speed);
	
//	//����ٶ�
//	Speed_Set(pwm_l,A2,motor_l.duty,0,1);
//	Speed_Set(pwm_r,A0,motor_r.duty,0,1);

	
}

//���ٿ���
void Final_Motor_Control(float k,float d,int32 limit){
	
	//���򻷼���
	pd.error = MID_W-final_mid_value;
	pd.PD_v=(int)(p*pd.error+d*(pd.error-pd.last_error));
	
	//�����޷�
	pd.PD_v=limit_int(-limit,pd.PD_v,limit);
	
	//PWM������޷��ͼ���
	motor_l.motor_v=limit_int(-pwm_limit,motor_l.duty-pd.PD_v,pwm_limit);
	motor_r.motor_v=limit_int(-pwm_limit,motor_r.duty+pd.PD_v,pwm_limit);

//	motorl=limit_int(-pwm_limit,motor_l.duty-PD,pwm_limit);
//	motorr=limit_int(-pwm_limit,motor_r.duty+PD,pwm_limit);
	
	//�����PWM	
	Speed_Set(pwm_l,A2,motor_l.motor_v,0,1);
	Speed_Set(pwm_r,A0,motor_r.motor_v,0,1);
	
	//������һ�����
	pd.last_error=pd.error;
	
}

//��������
void car_start(){
	
	if( KEY_SHORT_PRESS == key_get_state(KEY_4)&&main_menu_position==0){
		
		//���������ۼ�
		car_num++;
		
		//��հ���״̬
		key_clear_state(KEY_4);
		
	}
	
	//������
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
			
			Speed_Set(pwm_l,A2,0,0,1);
			Speed_Set(pwm_r,A0,0,0,1);
			
		}
		
	}
	//δ����ʱ
	else{
		
		//��������
		handle();
		
		//��ʾ�˵�
		print_menu();
		
	}
	
}


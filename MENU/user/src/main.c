#include "zf_common_headfile.h"
#include "MENU.H"
#include "IMAGE.H"
#include "MOTOR.H"
#include "PID.H"
	
uint8 mt9v03x_flag;


int main (void){

	
	clock_init(SYSTEM_CLOCK_120M);
	debug_init();
	
	mt9v03x_flag = mt9v03x_init();				//摄像头初始化验证
	
	Key_Init();														//按键初始化
	IPS_Init();														//屏幕初始化
	
	PWM_Init();														//PWM初始化
	Encoder_Init();												//编码器初始化
	
	gpio_init(BEEP, GPO, GPIO_LOW, GPO_PUSH_PULL); //蜂鸣器初始化
	
	pit_ms_init(TIM6_PIT,3);							//定时器初始化
	
	interrupt_set_priority(TIM6_IRQn,0);	
	
	if(mt9v03x_flag ==0){
		while(1){
			KeyNumber = KeyNum();												//获取按键信息
//			printf("%d,%d,%d,%d\n",motor_l.encoder_speed,motor_r.encoder_speed,motor_l.target_speed,motor_r.target_speed);
			
			get_image();																//获取图像
			threshold = Otsu(basic_image);							//获取阈值
			image_binaryzation(threshold);							//图像二值化
			
			find_basic_point(image);										//找到左右边线基点
			find_line(image);														//寻找边线
			final_mid_value = weight_find_mid_value();	//加权求中线值
			
			car_start();																//发车
	
			
			handle2(KeyNumber);													//操作二
			handle1(KeyNumber);													//操作一
			print_menu();																//显示菜单
			
//			num2++;
    }
	}
}


	
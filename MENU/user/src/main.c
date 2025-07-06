#include "zf_common_headfile.h"
#include "MENU.H"
#include "IMAGE.H"
	
uint8 mt9v03x_flag;

int main (void){
	
	clock_init(SYSTEM_CLOCK_120M);
	debug_init();
	
	Key_Init();														//按键初始化
	IPS_Init();														//屏幕初始化
	
	mt9v03x_flag = mt9v03x_init();				//摄像头初始化验证
	
	if(mt9v03x_flag ==0){
		while(1){
			
			get_image();																//获取图像
			threshold = Otsu(basic_image);							//获取阈值
			image_binaryzation(threshold);							//图像二值化
			find_basic_point(image);										//找到左右边线基点
			find_line(image);														//寻找边线
			final_mid_value = weight_find_mid_value();	//加权求中线值	
			
			KeyNumber = KeyNum();												//获取按键信息
			handle2(KeyNumber);													//操作
			handle1(KeyNumber);													//操作一
			print_menu();																//显示菜单
			
    }
	}
}


	
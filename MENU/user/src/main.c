#include "MENU.H"
#include "zf_common_headfile.h"

//#define KEY1                    (E2 )
//#define KEY2                    (E3 )
//#define KEY3                    (E4 )
//#define KEY4                    (E5 )

//void IPS_Init(){
//	ips200_set_color(RGB565_WHITE,RGB565_BLACK); //黑底白字
//	ips200_set_font(IPS200_8X16_FONT);            //字体大小8X16
//	ips200_set_dir(IPS200_PORTAIT);      				 //竖向显示
//  ips200_init(IPS200_TYPE_SPI);								 //SPI通信
//}

//void Key_Init(){
//	gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);                               // 初始化 KEY1 输入 默认高电平 上拉输入
//  gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);                               // 初始化 KEY2 输入 默认高电平 上拉输入
//  gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);                               // 初始化 KEY3 输入 默认高电平 上拉输入
//  gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);                               // 初始化 KEY4 输入 默认高电平 上拉输入
//}

//int KeyNum(){
//	int num =0;
//	if(!gpio_get_level(KEY1)){
//		system_delay_ms(20);
//		while(!gpio_get_level(KEY1));
//		system_delay_ms(20);
//		num =1;
//	}
//	else if(!gpio_get_level(KEY2)){
//		system_delay_ms(20);
//		while(!gpio_get_level(KEY1));
//		system_delay_ms(20);
//		num =2;
//	}
//	else if(!gpio_get_level(KEY3)){
//		system_delay_ms(20);
//		while(!gpio_get_level(KEY1));
//		system_delay_ms(20);
//		num =3;
//	}
//	else if(!gpio_get_level(KEY4)){
//		system_delay_ms(20);
//		while(!gpio_get_level(KEY1));
//		system_delay_ms(20);
//		num =4;
//	}
//	return num;
//}
	
uint8 mt9v03x_flag;
//uint8 basic_image[MT9V03X_H][MT9V03X_W];

int main (void){
	clock_init(SYSTEM_CLOCK_120M);
	debug_init();
	Key_Init();
	IPS_Init();
	mt9v03x_flag = mt9v03x_init();
	if(mt9v03x_flag ==0){
		while(1){
//			if(mt9v03x_finish_flag){
//				memcpy(basic_image[0],mt9v03x_image[0],MT9V03X_H*MT9V03X_W);
//			}
			
			KeyNumber = KeyNum();
			handle2(KeyNumber);
			handle1(KeyNumber);
			print_menu();
			
    }
	}
}


	
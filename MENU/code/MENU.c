#include "zf_common_headfile.h"
#include "image.h"

#define KEY1                    (E2 )
#define KEY2                    (E3 )
#define KEY3                    (E4 )
#define KEY4                    (E5 )

int KeyNumber=0;																		//按键参数
//uint8 base_image[MT9V03X_H][MT9V03X_W];  						//图像复制后的基础图像
int cursor_position=0;															//指针位置
int last_cursor_position=1;													//上一次指针的位置，用来消除上一次的指针图像
int main_menu_position=0;														//一级菜单的层数，表面为0，PID界面为1，image界面为2
int pid_menu_position=0;														//PID界面的层数，表面为0，kp调参界面为1，ki为2，kd为3
int len_main=2;																			//主菜单可进入界面的数量
int len_pid=3;																			//同理
int len_image=1;
int i=0;
float kp=0.2,ki=0.3,kd=0.5;

void IPS_Init(){
	ips200_set_color(RGB565_WHITE,RGB565_BLACK); //白底黑字
	ips200_set_font(IPS200_8X16_FONT);            //字体8X16
	ips200_set_dir(IPS200_PORTAIT);      				 //竖向显示
  ips200_init(IPS200_TYPE_SPI);								 //SPI通信
}

void Key_Init(){
	gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);                               //  KEY1  默认高电平 上拉输入
  gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);                               //  KEY2  默认高电平 上拉输入
  gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);                               //  KEY3  默认高电平 上拉输入
  gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);                               //  KEY4  默认高电平 上拉输入
}


//读取按键
int KeyNum(){
	int num =0;
	if(!gpio_get_level(KEY1)){
		system_delay_ms(20);
		while(!gpio_get_level(KEY1));
		system_delay_ms(20);
		num =1;
	}
	if(!gpio_get_level(KEY2)){
		system_delay_ms(20);
		while(!gpio_get_level(KEY2));
		system_delay_ms(20);
		num =2;
	}
	if(!gpio_get_level(KEY3)){
		system_delay_ms(20);
		while(!gpio_get_level(KEY3));
		system_delay_ms(20);
		num =3;
	}
	if(!gpio_get_level(KEY4)){
		system_delay_ms(20);
		while(!gpio_get_level(KEY4));
		system_delay_ms(20);
		num =4;
	}
	return num;
}


//重置指针
void rest_cursor(){
	cursor_position=0;
	last_cursor_position=1;
}


//显示菜单
void print_menu(){
	if(main_menu_position == 0){									//主菜单显示
		ips200_show_string(16,0,"pid");
		ips200_show_string(16,16,"image");
	
	
		ips200_show_string(0,cursor_position*16,">");
		ips200_show_string(0,last_cursor_position*16," ");
	}
	else if(main_menu_position == 1){
		if(pid_menu_position==0){					//PID菜单显示
			
			ips200_show_string(16,0,"kp");
			ips200_show_string(16,16,"ki");
			ips200_show_string(16,32,"kd");
			
			ips200_show_float(48,0,kp,2,3);
			ips200_show_float(48,16,ki,2,3);
			ips200_show_float(48,32,kd,2,3);
			
			ips200_show_string(16,32+16,"motor_l");
			ips200_show_string(16,32+16*2,"encoder_l");
			ips200_show_string(16,32+16*3,"motor_r");
			ips200_show_string(16,32+16*4,"encoder_r");
			
			
		
			ips200_show_string(0,cursor_position*16,">");
			ips200_show_string(0,last_cursor_position*16," ");
		}
		else if(pid_menu_position==1){				//调节kp的数值显示
			ips200_show_float(0,0,kp,2,3);
		}
		else if(pid_menu_position==2){				//调节ki的数值显示
			ips200_show_float(0,0,ki,2,3);
		}
		else if(pid_menu_position==3){				//调节kd的数值显示
			ips200_show_float(0,0,kd,2,3);
		}
	}
	else if(main_menu_position==2){					//图像界面显示
		
		//二值化图像显示
		ips200_show_gray_image(0,0,(const uint8*)image,MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
		
		//阈值显示
		ips200_show_string(0,128,"threshold");
		ips200_show_uint(8*10,128,threshold,3);
		
//		memcpy(base_image, mt9v03x_image, MT9V03X_H*MT9V03X_W);
//		ips200_show_gray_image(0, 0,(const uint8*)base_image, MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
		
		//左右基点显示
		ips200_show_string(0,128+16,"l_point");
		ips200_show_uint(8*10,128+16,l_point,3);
		ips200_show_string(0,128+16*2,"r_point");
		ips200_show_uint(8*10,128+16*2,r_point,3);
		
		//最终中值显示
		ips200_show_string(0,128+16*3,"m_value");
		ips200_show_uint(8*10,128+16*3,final_mid_line,3);
		
//		ips200_show_string(0,128+16,"l_line");
//		ips200_show_string(0,128+16*2,"r_line");

		//左右基点寻找起始位判断
//		ips200_show_string(0,128+16*3,"flag1_2");
//		ips200_show_int(8*10,128+16*3,flag1_2,2);
//		ips200_show_string(0,128+16*4,"flag1_4");
//		ips200_show_int(8*10,128+16*4,flag1_4,2);
//		ips200_show_string(0,128+16*5,"flag3_4");
//		ips200_show_int(8*10,128+16*5,flag3_4,2);

	}
	
	
//	ips200_show_int(0,32,last_cursor_position,2);
//	ips200_show_int(0,48,cursor_position,2);
//	ips200_show_int(0,64,menu_position,2);
}

//操作一
void handle1(int KeyNumber){
	
	switch(KeyNumber){
		case(0):break;
		case(1):
			if(main_menu_position==0){															//在主菜单界面时，按键一二用来改变指针位置
				last_cursor_position = cursor_position;
				cursor_position=(cursor_position-1+len_main)%len_main;
				
			}
			else{
				last_cursor_position = cursor_position;
				cursor_position=(cursor_position-1+len_pid)%len_pid;
				if(pid_menu_position==1){															//在PID界面时，按键一二用来调节PID参数
					kp+=0.1;
				}
				else if(pid_menu_position==2){
					ki+=0.1;
				}
				else if(pid_menu_position==3){
					kd+=0.1;
				}
			}
			break;
		case(2):
		  if(main_menu_position==0){															//在主菜单界面时，按键一二用来改变指针位置
				last_cursor_position = cursor_position;
				cursor_position=(cursor_position+1)%len_main;
			}
			else{
				last_cursor_position = cursor_position;
				cursor_position=(cursor_position+1)%len_pid;
				if(pid_menu_position==1){															//在PID界面时，按键一二用来调节PID参数
					kp-=0.1;
				}
				else if(pid_menu_position==2){
					ki-=0.1;
				}
				else if(pid_menu_position==3){
					kd-=0.1;
				}
			}
			break;

	}	
}



//操作二
void handle2(int KeyNumber){
	switch(KeyNumber){
		case(0):break;
		case(3):
			if(main_menu_position==0){					//按键三四用于确认和返回
				if(cursor_position==0){
					main_menu_position=1;
					ips200_clear();
				  rest_cursor();
				}
				else if(cursor_position == 1){
					main_menu_position=2;
					ips200_clear();
					rest_cursor();
				}	
			}
			else if(main_menu_position==1){
				if(cursor_position==0){
					pid_menu_position=1;
					ips200_clear();
				  rest_cursor();
				}
				else if(cursor_position == 1){
					pid_menu_position=2;
					ips200_clear();
					rest_cursor();
				}	
				else if(cursor_position == 1){
					pid_menu_position=3;
					ips200_clear();
					rest_cursor();
				}	
			}
			break;
		case(4):
		  if(pid_menu_position!=0){
		    pid_menu_position=0;
				ips200_clear();
		    rest_cursor();
			}
			else if(pid_menu_position==0&&main_menu_position!=0){
				main_menu_position=0;
				ips200_clear();
		    rest_cursor();
			}
			break;		
	}
}


//画边线和中线
void draw_line(){
	for(uint8 i=bottom_line;i>find_end_line;i--){
		ips200_draw_point(left_line[i],i,RGB565_RED);
		ips200_draw_point(right_line[i],i,RGB565_RED);
		ips200_draw_point(mid_line[i],i,RGB565_RED);
	}
	
}

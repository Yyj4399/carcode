#include "zf_common_headfile.h"
#include "image.h"
#include "MOTOR.H"
#include "PID.H"

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

//画边线和中线
void draw_line(){
	for(uint8 i=bottom_line;i>find_end_line;i--){
		ips200_draw_point(left_line[i],i,RGB565_RED);
		ips200_draw_point(left_line[i]+1,i,RGB565_BLUE);
		ips200_draw_point(right_line[i],i,RGB565_RED);
		ips200_draw_point(right_line[i]+1,i,RGB565_PINK);
		ips200_draw_point(mid_line[i],i,RGB565_GREEN);
	}
	
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
			
			ips200_show_float(48,0,motor_pid_l[0],4,3);
			ips200_show_float(48,16,motor_pid_l[1],4,3);
			ips200_show_float(48,32,Out_I_l,4,3);
			
			ips200_show_float(16*7,0,motor_pid_r[0],2,3);
			ips200_show_float(16*7,16,motor_pid_r[1],2,3);
			ips200_show_float(16*7,32,Out_I_l,2,3);
			
//			ips200_show_string(16,32+16,"motor_l");
			ips200_show_uint(8*15,32+16,car_num,2);
			ips200_show_string(16,32+16*2,"encoder_l");
			ips200_show_int(8*15,32+16*2,motor_l.encoder_speed,5);
//			ips200_show_string(16,32+16*3,"motor_r");
			ips200_show_string(16,32+16*4,"encoder_r");
			ips200_show_int(8*15,32+16*4,motor_r.encoder_speed,5);
			
			
		
			ips200_show_string(0,cursor_position*16,">");
			ips200_show_string(0,last_cursor_position*16," ");
		}
		else if(pid_menu_position==1){				//调节kp的数值显示
			ips200_show_float(0,0,motor_pid_l[0],2,3);
		}
		else if(pid_menu_position==2){				//调节ki的数值显示
			ips200_show_float(0,0,motor_pid_l[1],2,3);
		}
//		else if(pid_menu_position==3){				//调节kd的数值显示
//			ips200_show_float(0,0,motor_pid_l[2],2,3);
//		}
	}
	else if(main_menu_position==2){					//图像界面显示
		
		//二值化图像显示
		ips200_show_gray_image(0,0,(const uint8*)image,MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
		
		//阈值显示
		ips200_show_string(0,128,"threshold");
		ips200_show_uint(8*10,128,threshold,3);
		
		//左右基点显示
		ips200_show_string(0,128+16,"l_point");
		ips200_show_uint(8*10,128+16,l_point,3);
		ips200_show_string(0,128+16*2,"r_point");
		ips200_show_uint(8*10,128+16*2,r_point,3);
//		ips200_show_string(0,128+16*3,"m_point");
//		ips200_show_uint(8*10,128+16*3,(r_point+l_point)/2,3);
		
//		ips200_draw_point(x1,y1,RGB565_RED);
//		ips200_draw_point(x2,y2,RGB565_RED);
//		ips200_draw_point(x3,y3,RGB565_RED);
//		ips200_draw_point(x4,y4,RGB565_RED);
		
		ips200_show_uint(0,128+16*4,x1,3);
		ips200_show_uint(8*5,128+16*4,y1,3);
		ips200_show_uint(8*10,128+16*4,x2,3);
		ips200_show_uint(8*15,128+16*4,y2,3);
		ips200_show_uint(8*20,128+16*4,x3,3);
		ips200_show_uint(8*25,128+16*4,y3,3);
		ips200_show_uint(0,128+16*5,x4,3);
		ips200_show_uint(8*5,128+16*5,y4,3);
		ips200_show_uint(8*10,128+16*5,x5,3);
		ips200_show_uint(8*15,128+16*5,y5,3);
		ips200_show_uint(8*20,128+16*5,x6,3);
		ips200_show_uint(8*25,128+16*5,y6,3);
		ips200_show_uint(0,128+16*6,x7,3);
		ips200_show_uint(8*5,128+16*6,y7,3);
//		ips200_show_uint(8*10,128+16*6,x8,3);
//		ips200_show_uint(8*15,128+16*6,y8,3);
		ips200_show_float(0,128+16*7,kl,2,2);
		ips200_show_float(8*10,128+16*7,kr,2,2);
//		ips200_show_float(8*10,128+16*6,k1,2,2);
//		ips200_draw_line(x1,y1,x3,y3,RGB565_RED);
//		ips200_draw_line(x2,y2,x4,y4,RGB565_RED);
//		ips200_draw_line(x5,y5,x6,y6,RGB565_RED);
//		ips200_draw_line(x7,y7,x8,y8,RGB565_RED);

		//画边线
		draw_line();
	
		//最终中值显示
		ips200_show_string(0,128+16*3,"m_value");
		ips200_show_uint(8*10,128+16*3,final_mid_value,3);
		
//		ips200_show_uint(0,128+16*6,mid_line[80],3);
//		ips200_show_uint(8*5,128+16*6,mid_line[75],3);
//		ips200_show_uint(8*10,128+16*6,mid_line[70],3);
//		ips200_show_uint(8*15,128+16*6,mid_line[65],3);
		
//		ips200_show_uint(0,128+16*7,right_line[80],3);
//		ips200_show_uint(8*5,128+16*7,right_line[70],3);
//		ips200_show_uint(8*10,128+16*7,right_line[60],3);
//		ips200_show_uint(8*15,128+16*7,right_line[45],3);		

	}
	
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
					motor_pid_l[0]+=0.1;
				}
				else if(pid_menu_position==2){
					motor_pid_l[1]+=0.1;
				}
//				else if(pid_menu_position==3){
//					motor_pid_l[2]+=0.1;
//				}
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
					motor_pid_l[0]-=0.1;
				}
				else if(pid_menu_position==2){
					motor_pid_l[1]-=0.1;
				}
//				else if(pid_menu_position==3){
//					motor_pid_l[2]-=0.1;
//				}
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




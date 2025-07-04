#include "zf_common_headfile.h"

#define KEY1                    (E2 )
#define KEY2                    (E3 )
#define KEY3                    (E4 )
#define KEY4                    (E5 )

int KeyNumber=0;
uint8 basic_image[MT9V03X_H][MT9V03X_W];
int cursor_position=0;
int last_cursor_position=1;
int main_menu_position=0;
int pid_menu_position=0;
int len_main=2;
int len_pid=3;
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

void rest_cursor(){
	cursor_position=0;
	last_cursor_position=1;
}

void print_menu(){
	if(main_menu_position == 0){
		ips200_show_string(16,0,"pid");
		ips200_show_string(16,16,"image");
	
	
		ips200_show_string(0,cursor_position*16,">");
		ips200_show_string(0,last_cursor_position*16," ");
	}
	else if(main_menu_position == 1){
		if(pid_menu_position==0){
			
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
		else if(pid_menu_position==1){
			ips200_show_float(0,0,kp,2,3);
		}
		else if(pid_menu_position==2){
			ips200_show_float(0,0,ki,2,3);
		}
		else if(pid_menu_position==3){
			ips200_show_float(0,0,kd,2,3);
		}
	}
	else if(main_menu_position==2){
		memcpy(basic_image, mt9v03x_image, MT9V03X_H*MT9V03X_W);
		ips200_show_gray_image(0, 0,(const uint8*)basic_image, MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
		ips200_show_string(0,128,"midline");
		ips200_show_string(0,128+16,"l_line");
		ips200_show_string(0,128+16*2,"r_line");
	}
	
	
//	ips200_show_int(0,32,last_cursor_position,2);
//	ips200_show_int(0,48,cursor_position,2);
//	ips200_show_int(0,64,menu_position,2);
}


void handle1(int KeyNumber){
	
	switch(KeyNumber){
		case(0):break;
		case(1):
			if(main_menu_position==0){
				last_cursor_position = cursor_position;
				cursor_position=(cursor_position-1+len_main)%len_main;
				
			}
			else{
				last_cursor_position = cursor_position;
				cursor_position=(cursor_position-1+len_pid)%len_pid;
				if(pid_menu_position==1){
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
		  if(main_menu_position==0){
				last_cursor_position = cursor_position;
				cursor_position=(cursor_position+1)%len_main;
			}
			else{
				last_cursor_position = cursor_position;
				cursor_position=(cursor_position+1)%len_pid;
				if(pid_menu_position==1){
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

void handle2(int KeyNumber){
	switch(KeyNumber){
		case(0):break;
		case(3):
			if(main_menu_position==0){
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

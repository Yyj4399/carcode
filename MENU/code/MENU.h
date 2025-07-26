#ifndef __MENU_H
#define __MENU_H

void handle();			//操作
void print_menu();								//显示菜单
void IPS_Init();									//屏幕初始化
void Key_Init();									//按键初始化
int KeyNum();											//获取按键情况
void draw_line();									//画出边线和中线								
	
extern int main_menu_position;		//主菜单位置
//extern int32 num2;

#endif

#ifndef __MENU_H
#define __MENU_H

void handle1(int KeyNumber);			//操作一
void handle2(int KeyNumber);			//操作二
void print_menu();								//显示菜单
void IPS_Init();									//屏幕初始化
void Key_Init();									//按键初始化
int KeyNum();											//获取按键情况
void draw_line();									//画出边线和中线								

extern int KeyNumber;			
extern int main_menu_position;

#endif

#ifndef __MENU_H
#define __MENU_H

void handle1(int KeyNumber);			//����һ
void handle2(int KeyNumber);			//������
void print_menu();								//��ʾ�˵�
void IPS_Init();									//��Ļ��ʼ��
void Key_Init();									//������ʼ��
int KeyNum();											//��ȡ�������
void draw_line();									//�������ߺ�����								

extern int KeyNumber;			
extern int main_menu_position;

#endif

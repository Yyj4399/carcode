#ifndef __IMAGE_H
#define __IMAGE_H

void get_image();																								//��ȡͼ��
uint8 Otsu(uint8 basic_image[MT9V03X_H][MT9V03X_W]);						//��ȡ��ֵ
void image_binaryzation(uint8 thershold);												//ͼ��ֶζ�ֵ��
void find_basic_point(uint8 bio_image[MT9V03X_H][MT9V03X_W]);   //Ѱ�����һ���
void find_line(uint8 index[MT9V03X_H][MT9V03X_W]);   						//Ѱ�����ұ���
uint8 weight_find_mid_value();																	//��Ȩ������ֵ

extern uint8 threshold;																					//��ֵ����ֵ
extern uint8 basic_image[MT9V03X_H][MT9V03X_W];									//����ͼ��
extern uint8 image[MT9V03X_H][MT9V03X_W];												//��ֵ�����ͼ��
extern uint8 l_point;																						//�����������
extern uint8 r_point;																						//�ұ���������
extern int flag1_2;																							//��1/2����ʼѰ�һ����жϱ�־
extern int flag1_4;																							//��1/4����ʼѰ�һ����жϱ�־
extern int flag3_4;																							//��3/4����ʼѰ�һ����жϱ�־
extern uint8 left_line[MT9V03X_H];															//����ߴ洢
extern uint8 right_line[MT9V03X_H];															//�ұ��ߴ洢
extern uint8 mid_line[MT9V03X_H];																//���ߴ洢
extern uint8 mid_weight[120];																		//����Ȩ��
extern uint8 final_mid_value;																		//��������ֵ
extern uint8 x1,x2,x3,x4,y1,y2,y3,y4;														//�յ�����

#define find_end_line 20																				//����Ѱ�ҽ�ֹ��
#define bottom_line 119																					//����λ��

#endif
#ifndef __IMAGE_H
#define __IMAGE_H

void get_image();																								//��ȡͼ��
uint8 Otsu(uint8 basic_image[MT9V03X_H][MT9V03X_W]);						//��ȡ��ֵ
void image_binaryzation(uint8 thershold);												//ͼ��ֶζ�ֵ��
void find_basic_point(uint8 bio_image[MT9V03X_H][MT9V03X_W]);   //Ѱ�����һ���
void find_line(uint8 index[MT9V03X_H][MT9V03X_W]);   						//Ѱ�����ұ���
uint8 weight_find_mid_value();																	//��Ȩ������ֵ
uint8 mini_uint8(uint8 a,uint8 b);															//��Сֵ����
uint8 max_uint8(uint8 a,uint8 b);																//���ֵ����
uint8 limit_uint8(uint8 a,uint8 b,uint8 c);											//�޷�����

typedef struct cross{		//ʮ�ֲ�������
	
	uint8 x1;							//���¹յ�xֵ
	uint8 x2;							//���¹յ�xֵ
	uint8 x3;							//���Ϲյ�xֵ
	uint8 x4;							//���Ϲյ�xֵ
	
	uint8 y1;							//���¹յ�yֵ
	uint8 y2;							//���¹յ�yֵ
	uint8 y3;							//���Ϲյ�yֵ
	uint8 y4;							//���Ϲյ�yֵ
	
	float kl;							//��߲���б��
	float kr;							//�ұ߲���б��
	
}cross;

typedef struct circle{		//Բ����������
	
	uint8 x5;							//�뻷ʱ���¹յ�xֵ
	uint8 x6;							//�뻷ʱԲ���е�xֵ
	uint8 x7;							//�뻷ʱ���Ϲյ�xֵ
	uint8 x8;							//����ʱ���µ�xֵ
	uint8 x9;							//����ʱ���¹յ�xֵ
	
	uint8 y5;							//�뻷ʱ���¹յ�yֵ
	uint8 y6;							//�뻷ʱԲ���е�yֵ
	uint8 y7;							//�뻷ʱ���Ϲյ�yֵ
	uint8 y8;							//����ʱ���µ�yֵ
	uint8 y9;							//����ʱ���¹յ�yֵ
	
	float k1;							//��Բ��ʱֱ��б��
	float k3;							//�뻷ʱ����б��
	float k5;							//����ʱ����б��
	float k8;							//��Բ��ʱֱ��б��
	
}circle;

typedef struct f_point{		//ͼ������жϱ�־λ����
	
	int flag1_2;																							//��1/2����ʼѰ�һ����жϱ�־
	int flag1_4;																							//��1/4����ʼѰ�һ����жϱ�־
	int flag3_4;																							//��3/4����ʼѰ�һ����жϱ�־
	
}f_point;

extern uint8 find_end_line;																			//��Ѱ��ֹ��
extern uint8 threshold;																					//��ֵ����ֵ
extern uint8 basic_image[MT9V03X_H][MT9V03X_W];									//����ͼ��
extern uint8 image[MT9V03X_H][MT9V03X_W];												//��ֵ�����ͼ��

extern uint8 l_point;																						//�����������
extern uint8 r_point;																						//�ұ���������

extern uint8 left_line[MT9V03X_H];															//����ߴ洢
extern uint8 right_line[MT9V03X_H];															//�ұ��ߴ洢
extern uint8 mid_line[MT9V03X_H];																//���ߴ洢
extern uint8 mid_weight[120];																		//����Ȩ��
extern uint8 final_mid_value;																		//��������ֵ

extern struct f_point basic_point_flag;													//Ѱ�һ����жϱ�־λ�ṹ��
extern struct cross cross_point;																//Բ���յ��б�ʽṹ��
extern struct circle circle_point;															//ʮ�ֲ��ߵĹյ��б�ʽṹ��
																					


#define MID_W         94																				//ֱ������ֵ
#define BEEP         (D7 )																			//���������Ŷ���
#define bottom_line 	119																				//����λ��

#endif
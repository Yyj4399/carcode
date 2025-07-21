#ifndef __IMAGE_H
#define __IMAGE_H

void get_image();																								//获取图像
uint8 Otsu(uint8 basic_image[MT9V03X_H][MT9V03X_W]);						//获取阈值
void image_binaryzation(uint8 thershold);												//图像分段二值化
void find_basic_point(uint8 bio_image[MT9V03X_H][MT9V03X_W]);   //寻找左右基点
void find_line(uint8 index[MT9V03X_H][MT9V03X_W]);   						//寻找左右边线
uint8 weight_find_mid_value();																	//加权求中线值
uint8 mini_uint8(uint8 a,uint8 b);															//最小值函数
uint8 max_uint8(uint8 a,uint8 b);																//最大值函数
uint8 limit_uint8(uint8 a,uint8 b,uint8 c);											//限幅函数

extern uint8 find_end_line;
extern uint8 threshold;																					//二值化阈值
extern uint8 basic_image[MT9V03X_H][MT9V03X_W];									//基础图像
extern uint8 image[MT9V03X_H][MT9V03X_W];												//二值化后的图像
extern uint8 l_point;																						//左边生长基点
extern uint8 r_point;																						//右边生长基点
extern int flag1_2;																							//从1/2处开始寻找基点判断标志
extern int flag1_4;																							//从1/4处开始寻找基点判断标志
extern int flag3_4;																							//从3/4处开始寻找基点判断标志
extern uint8 left_line[MT9V03X_H];															//左边线存储
extern uint8 right_line[MT9V03X_H];															//右边线存储
extern uint8 mid_line[MT9V03X_H];																//中线存储
extern uint8 mid_weight[120];																		//中线权重
extern uint8 final_mid_value;																		//最终中线值
extern uint8 x1,x2,x3,x4,y1,y2,y3,y4,x5,y5,x6,y6,x7,y7,x8,y8;														//拐点坐标
extern float kl,kr;																							//十字补线斜率
extern float k1,k3;																							//圆环补线斜率

#define MID_W         94																				//直道中线值
#define BEEP         (D7 )
#define bottom_line 	119																					//底线位置

#endif
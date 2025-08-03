#ifndef __IMAGE_H
#define __IMAGE_H

void get_image();																								//获取图像
uint8 Otsu(uint8 basic_image[MT9V03X_H][MT9V03X_W]);						//获取阈值
void image_binaryzation(uint8 thershold);												//图像分段二值化

void find_basic_point(uint8 bio_image[MT9V03X_H][MT9V03X_W]);   //寻找左右基点

void find_line(uint8 index[MT9V03X_H][MT9V03X_W]);   						//寻找左右边线
uint8 weight_find_mid_value();																	//加权求中线值

uint8 mini_uint8(uint8 a,uint8 b);															//无符号8位整形最小值函数
uint8 max_uint8(uint8 a,uint8 b);																//无符号8位整形最大值函数
uint8 limit_uint8(uint8 a,uint8 b,uint8 c);											//无符号8位整形限幅函数





#define MID_W         94																				//直道中线值
#define BEEP         (D7 )																			//蜂鸣器引脚定义
#define bottom_line 	119																				//底线位置






typedef struct cross{		//十字参数定义
	
	uint8 x1;							//左下拐点x值
	uint8 x2;							//右下拐点x值
	uint8 x3;							//左上拐点x值
	uint8 x4;							//右上拐点x值
	
	uint8 y1;							//左下拐点y值
	uint8 y2;							//右下拐点y值
	uint8 y3;							//左上拐点y值
	uint8 y4;							//右上拐点y值
	
	float kl;							//左边补线斜率
	float kr;							//右边补线斜率
	
}cross;

typedef struct circle{		//圆环参数定义
	
	uint8 x5;							//入环时右下拐点x值
	uint8 x6;							//入环时圆环切点x值
	uint8 x7;							//入环时右上拐点x值
	uint8 x8;							//拉线时左下点x值
	uint8 x9;							//出环时左下拐点x值
	
	uint8 y5;							//入环时右下拐点y值
	uint8 y6;							//入环时圆环切点y值
	uint8 y7;							//入环时右上拐点y值
	uint8 y8;							//拉线时左下点y值
	uint8 y9;							//出环时左下拐点y值
	
	float k1;							//入圆环时直线斜率
	float k3;							//入环时拉线斜率
	float k5;							//出环时拉线斜率
	float k8;							//出圆环时直线斜率
	
}circle;

typedef struct f_line{		//寻找边线标志位定义
	
	uint8 l_find_flag;							//向左找左边线标志位
	uint8 m_l_find_flag;						//从中间找左边线标志位
	uint8 r_find_flag;							//向右找右边线标志位
	uint8 m_r_find_flag;						//从中间找右边线标志位
	
}f_line;

typedef struct f_circle{		//圆环状态标志位定义
	
	uint8 circle1_flag;							//圆环状态一标志位
	uint8 circle3_flag;							//圆环状态三标志位
	uint8 circle5_flag;							//圆环状态五标志位
	uint8 circle8_flag;							//圆环状态八标志位
	
	
}f_circle;

typedef struct Num{					//丢线数定义
	
	uint8 num_loss;										//两边丢线计数
	uint8 num_lossr;									//右边丢线计数
	uint8 num_lossl;									//左边丢线计数
	
}Num;








extern uint8 find_end_line;																			//搜寻截止行
extern uint8 threshold;																					//二值化阈值
extern uint8 basic_image[MT9V03X_H][MT9V03X_W];									//基础图像
extern uint8 image[MT9V03X_H][MT9V03X_W];												//二值化后的图像

extern uint8 l_point;																						//左边生长基点
extern uint8 r_point;																						//右边生长基点

extern uint8 left_line[MT9V03X_H];															//左边线存储
extern uint8 right_line[MT9V03X_H];															//右边线存储
extern uint8 mid_line[MT9V03X_H];																//中线存储
extern uint8 mid_weight[120];																		//中线权重
extern uint8 final_mid_value;																		//最终中线值

extern struct cross cross_point;																//圆环拐点和斜率结构体
extern struct circle circle_point;															//十字补线的拐点和斜率结构体
extern struct Num loss_num;																			//丢线数量结构体
																					

#endif
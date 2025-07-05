#include "zf_common_headfile.h"

uint8 basic_image[MT9V03X_H][MT9V03X_W];  					//图像复制后的基础图像
uint8 image[MT9V03X_H][MT9V03X_W];    							//二值化后的图像

//获取图像
void get_image(){
	
	memcpy(basic_image, mt9v03x_image, MT9V03X_H*MT9V03X_W);
	
}

//大津法求阈值
#define GrayScale 256
uint16 hist[GrayScale] ={0};    //灰度值像素点的数量，数值存放，直方图
float P[GrayScale] = {0};    		//每个灰度出现概率
float PK[GrayScale] = {0};			//概率累计和
float MK[GrayScale] = {0};			//灰度值累加均值
uint8 threshold;								//输出阈值

uint8 Otsu(uint8 index_image[MT9V03X_H][MT9V03X_W]){
	
	uint8 image_threshold;
	float imagesize = MT9V03X_H*MT9V03X_W;
	uint8 image_temp;
	
	float sumPK =0;				//概率累计和
	float sumMK =0;				//灰度值累加均值
	float var =0;
	float vartmp =0;
	
	//清零
	for(uint16 i=0;i<GrayScale;i++){
		
		hist[i]=0;
		P[i]=0;
		PK[i]=0;
		MK[i]=0;
		
	}
	
	//直方图
	for(uint8 i=0;i<MT9V03X_H;i++){
		
		for(uint16 j=0;j<MT9V03X_W;j++){
			
			image_temp = index_image[i][j];
			hist[image_temp]++;
			
		}
	}
	
	//求类间方差
	for(uint16 i=0;i<GrayScale;i++){
		
		P[i]=(float)hist[i]/imagesize;
		
		PK[i]=sumPK+P[i];
		sumPK=PK[i];
		
		MK[i]=sumMK+i*P[i];
		sumMK=MK[i];
		
	}
	
	//求解最大类间方差下的阈值
	for(uint8 i=5;i<245;i++){
		
		vartmp = ((MK[GrayScale-1]*PK[i]-MK[i])*(MK[GrayScale-1]*PK[i]-MK[i]))/(PK[i]*(1-PK[i]));
		
		if(vartmp>var){
			
			var = vartmp;
		  image_threshold = i;
			
		}
		
	}
	
	return image_threshold;
}

//图像分段二值化
void image_binaryzation(uint8 thershold){
	
	uint8 temp;
	
	for(uint8 i=0;i<40;i++){							//远端图像阈值+10
		for(uint8 j=0;j<MT9V03X_W;j++){
			temp = basic_image[i][j];
			if(temp<(thershold+10)){
				image[i][j] =0;
			}
			else{
				image[i][j]=255;
			}
		}
	}
	
	for(uint8 i=40;i<MT9V03X_H;i++){			//近端不变
		for(uint8 j=0;j<MT9V03X_W;j++){
			temp = basic_image[i][j];
			if(temp<thershold){
				image[i][j] =0;
			}
			else{
				image[i][j]=255;
			}
		}
	}
	
}

#define bottom_line 119
uint8 l_point;
uint8 r_point;
int flag1_2=0;
int flag1_4=0;
int flag3_4=0;
//找图像边线基点
void find_basic_point(uint8 bio_image[MT9V03X_H][MT9V03X_W]){
	//从1/2处开始找
	if(bio_image[bottom_line][MT9V03X_W/2]==255&&bio_image[bottom_line][MT9V03X_W/2+1]==255&&bio_image[bottom_line][MT9V03X_W/2-1]==255){
		flag1_2=1;
		flag1_4=0;
		flag3_4=0;
		//寻找左基点
		for(uint8 i=MT9V03X_W/2;i>0;i--){
			if(bio_image[bottom_line][i-1]==0&&bio_image[bottom_line][i]==255&&bio_image[bottom_line][i+1]==255){
				l_point = i;
				break;
			}
			if(i-1==1){
				l_point=1;
				break;
			}
		}
		//寻找右基点
		for(uint8 i=MT9V03X_W/2;i<MT9V03X_W-2;i++){
			if(bio_image[bottom_line][i-1]==255&&bio_image[bottom_line][i]==255&&bio_image[bottom_line][i+1]==0){
				r_point = i;
				break;
			}
			if(i+1==MT9V03X_W-2){
				r_point=MT9V03X_W-2;
				break;
			}
		}
		
	}
	//从1/4处开始找
	else if(bio_image[bottom_line][MT9V03X_W/4]==255&&bio_image[bottom_line][MT9V03X_W/4+1]==255&&bio_image[bottom_line][MT9V03X_W/4-1]==255){
		flag1_2=0;
		flag1_4=1;
		flag3_4=0;
		//寻找左基点
		for(uint8 i=MT9V03X_W/4;i>0;i--){
			if(bio_image[bottom_line][i-1]==0&&bio_image[bottom_line][i]==255&&bio_image[bottom_line][i+1]==255){
				l_point = i;
				break;
			}
			if(i-1==1){
				l_point=1;
				break;
			}
		}
		//寻找右基点
		for(uint8 i=MT9V03X_W/4;i<MT9V03X_W-2;i++){
			if(bio_image[bottom_line][i-1]==255&&bio_image[bottom_line][i]==255&&bio_image[bottom_line][i+1]==0){
				r_point = i;
				break;
			}
			if(i+1==MT9V03X_W-2){
				r_point=MT9V03X_W-2;
				break;
			}
		}
		
	}
	//从3/4处开始找
	else if(bio_image[bottom_line][MT9V03X_W/4*3]==255&&bio_image[bottom_line][MT9V03X_W/4*3+1]==255&&bio_image[bottom_line][MT9V03X_W/4*3-1]==255){
		flag1_2=0;
		flag1_4=0;
		flag3_4=1;
		//寻找左基点
		for(uint8 i=MT9V03X_W/4*3;i>0;i--){
			if(bio_image[bottom_line][i-1]==0&&bio_image[bottom_line][i]==255&&bio_image[bottom_line][i+1]==255){
				l_point = i;
				break;
			}
			if(i-1==1){
				l_point=1;
				break;
			}
		}
		//寻找右基点
		for(uint8 i=MT9V03X_W/4*3;i<MT9V03X_W-2;i++){
			if(bio_image[bottom_line][i-1]==255&&bio_image[bottom_line][i]==255&&bio_image[bottom_line][i+1]==0){
				r_point = i;
				break;
			}
			if(i+1==MT9V03X_W-2){
				r_point=MT9V03X_W-2;
				break;
			}
		}
		
	}
	
}

//限幅函数
uint8 limit_uint8(uint8 a,uint8 b,uint8 c){
	if(b>=a&&b<=c){
		return b;
	}
	else if(b<=a){
		return a;
	}
	else if(b>=c){
		return c;
	}
	return 0;
}


#define find_end_line 20   						//寻找截止处
#define l_right_find 10								//左边线向右寻找10个像素
#define l_left_find 5									//左边线向左寻找5个像素
#define r_right_find 5								//右边线向右寻找5个像素
#define r_left_find 10								//右边线向左寻找10个像素
uint8 left_line[MT9V03X_H]={0};				//左边线存储
uint8 right_line[MT9V03X_H]={0};			//右边线存储
uint8 mid_line[MT9V03X_H]={0};				//中线存储

//寻找边线
void find_line(uint8 index[MT9V03X_H][MT9V03X_W]){
	
	uint8 l = l_point;
	uint8 r = r_point;
	
	//从下往上开始寻找边线截止到20
	for(uint8 i=bottom_line-1;i>find_end_line;i--){
		
		uint8 l_find_flag=0;
		uint8 m_l_find_flag=0;
		
		//向右寻找左边线
		for(uint8 j=l;j<l+l_right_find;j++){
			if(index[i][j-1]==0&&index[i][j]==255&&index[i][j+1]==255){
				l = j;
				break;
			}
			else if(j>=MT9V03X_W-2){
				l=j-3;
				l_find_flag =1;
				break;
			}
			else if(j==l+l_right_find-1){
				l_find_flag =1;
				break;
			}
		}
		
		//向左寻找左边线
		if(l_find_flag==1){
			for(uint8 j=l;j>l-l_left_find;j--){
				if(index[i][j-1]==0&&index[i][j]==255&&index[i][j+1]==255){
					l = j;
					break;
				}
				else if(j==1){
					l = 1;
					m_l_find_flag =1;
					break;
				}
				else if(j==l-l_left_find+1){
					m_l_find_flag =1;
					break;
				}
			}
		}
		
		//从中间寻找左边线
		if(m_l_find_flag==1){
			for(uint8 j=MT9V03X_W/2;j>0;j--){
				
				if(index[i][j-1]==0&&index[i][j]==255&&index[i][j+1]==255){
					l = j;
					break;
				}
				
				else if(j==1){
					l = 1;
					break;
				}
		
			}
		}
		
		uint8 r_find_flag=0;
		uint8 m_r_find_flag=0;
		
		//向左寻找右边线
		for(uint8 j=l;j>l-r_left_find;j--){
			if(index[i][j-1]==255&&index[i][j]==255&&index[i][j+1]==0){
				r = j;
				break;
			}
			else if(j<=3){
				r=j+3;
				r_find_flag =1;
				break;
			}
			else if(j==j-r_left_find+1){
				r_find_flag =1;
				break;
			}
		}
		
		//向右寻找右边线
		if(r_find_flag==1){
			for(uint8 j=l;j<l+r_left_find;j++){
				if(index[i][j-1]==255&&index[i][j]==255&&index[i][j+1]==0){
					r = j;
					break;
				}
				else if(j==MT9V03X_W-2){
					r = MT9V03X_W-2;
					m_r_find_flag =1;
					break;
				}
				else if(j==l+r_right_find-1){
					m_r_find_flag =1;
					break;
				}
			}
		}
		
		//从中间寻找右边线
		if(m_r_find_flag==1){
			for(uint8 j=MT9V03X_W/2;j<MT9V03X_W-1;j++){
				
				if(index[i][j-1]==255&&index[i][j]==255&&index[i][j+1]==0){
					r = j;
					break;
				}
				
				else if(j==MT9V03X_W-2){
					r = MT9V03X_W-2;
					break;
				}
		
			}
		}
		
		//存储边线以及中线
		left_line[i] = limit_uint8(1,l,MT9V03X_W-2);
		right_line[i] = limit_uint8(1,r,MT9V03X_W-2);
		mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
	}
}

//中线权重
//权重越往上转弯越早
uint8 mid_weight[120]={
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	6,6,6,6,6,6,6,6,6,6,
	7,8,9,10,11,12,13,14,15,16,
	17,18,19,20,20,20,20,19,18,17,
	16,15,14,13,12,11,10,9,8,7,
	6,6,6,6,6,6,6,6,6,6,	
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1	
};

uint8 final_mid_line = MT9V03X_H/2;				//最终中线值
uint8 last_mid_line = MT9V03X_H/2;				//上一次的中线值


//加权求中线值
uint8 weight_find_mid_line(){
	
	uint8 mid_value = MT9V03X_H/2;					//最终中线值
	uint8 now_mid_line = MT9V03X_H/2;				//当下中线值
	uint32 weight_mid_line_sum;							//中线权重和
	uint32 weight_sum;											//总权重
	
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		weight_mid_line_sum +=mid_line[i] *mid_weight[i];
		weight_sum += mid_weight[i];
		
	}
	
	now_mid_line = (uint8)(weight_mid_line_sum/weight_sum);
	mid_value = now_mid_line*0.7+last_mid_line*0.3;			//互补滤波
	last_mid_line =now_mid_line;
	
	return mid_value;
}


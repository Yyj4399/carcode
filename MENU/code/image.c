#include "zf_common_headfile.h"
#include "image.h"

uint8 basic_image[MT9V03X_H][MT9V03X_W];  					//图像复制后的基础图像
uint8 image[MT9V03X_H][MT9V03X_W];    							//二值化后的图像
uint8 find_end_line=25;															//边线寻找截止处

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
	uint8 num=0;
	uint8 num1=0;
	
	for(uint8 i=0;i<40;i++){							//远端图像阈值+10
		for(uint8 j=0;j<MT9V03X_W;j++){
			temp = basic_image[i][j];
			if(temp<(thershold+10)){
				image[i][j] =0;
				num++;
			}
			else{
				image[i][j]=255;
			}
		}
		//若有一行基本为黑则这一行为搜寻截止行
		if(num>=180){
			find_end_line=i;
			num=0;
			num1++;
		}
//		else{
//			find_end_line=25;
//		}
		num=0;
	}
	
	for(uint8 i=40;i<MT9V03X_H;i++){			//近端不变
		for(uint8 j=0;j<MT9V03X_W;j++){
			temp = basic_image[i][j];
			if(temp<thershold){
				image[i][j] =0;
				num++;
			}
			else{
				image[i][j]=255;
			}
		}
		//若有一行基本为黑则这一行为搜寻截止行
		if(num>=180){
			find_end_line=i;
			num=0;
			num1++;
		}
//		else{
//			find_end_line=25;
//		}
		num=0;
	}
	if(num1==0||find_end_line<=25){
		find_end_line=25;
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

//最小值函数
uint8 mini_uint8(uint8 a,uint8 b){
	if(a>=b){
		return b;
	}
	else if(a<b){
		return a;
	}
	return 0;
}

//最大值函数
uint8 max_uint8(uint8 a,uint8 b){
	if(a>=b){
		return a;
	}
	else if(a<b){
		return b;
	}
	return 0;
}

#define l_right_find 20								//左边线向右寻找20个像素
#define l_left_find 10								//左边线向左寻找10个像素
#define r_right_find 10								//右边线向右寻找10个像素
#define r_left_find 20								//右边线向左寻找20个像素
uint8 left_line[MT9V03X_H]={0};				//左边线存储
uint8 right_line[MT9V03X_H]={0};			//右边线存储
uint8 mid_line[MT9V03X_H]={0};				//中线存储
uint8 x1=0,x2=0,x3=0,x4=0,y1=0,y2=0,y3=0,y4=0;				//十字拐点坐标
uint8 x5=0,y5=0,x6=0,y6=0,x7=0,y7=0,x8=0,y8=0,x9=0,y9=0;				//圆环拐点坐标
float kl=0,kr=0;													//十字补线斜率
float k1=0,k3=0,k5=0,k8=0;															//圆环补线斜率
uint16 count_beep = 0;									//蜂鸣器计数

//寻找边线
void find_line(uint8 index[MT9V03X_H][MT9V03X_W]){
	
	uint8 l = l_point;									//中间量存储
	uint8 r = r_point;									//中间量存储
	uint8 num_loss=0;										//两边丢线计数
	uint8 num_lossr=0;									//右边丢线计数
	uint8 num_lossl=0;									//左边丢线计数
	uint8 position[120]={0};						//十字丢线位置记录
	
	//从下往上开始寻找边线截止到20
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		uint8 l_find_flag=0;							//向左找左边线标志位
		uint8 m_l_find_flag=0;						//从中间找左边线标志位
		uint8 r_find_flag=0;							//向右找右边线标志位
		uint8 m_r_find_flag=0;						//从中间找右边线标志位
		
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
					l_find_flag =0;
					break;
				}
				else if(j==l-l_left_find+1){
					m_l_find_flag =1;
					l_find_flag =0;
					break;
				}
			}
		}
		
		//从3/4处寻找左边线
		if(m_l_find_flag==1){
			for(uint8 j=MT9V03X_W/4*3;j>0;j--){
				
				if(index[i][j-1]==0&&index[i][j]==255&&index[i][j+1]==255){
					l = j;
					m_l_find_flag =0;
					break;
				}
				
				else if(j==1){
					l = 1;
					m_l_find_flag =0;
					break;
				}
		
			}
		}
		
		//向左寻找右边线
		for(uint8 j=r;j>r-r_left_find;j--){
			if(index[i][j-1]==255&&index[i][j]==255&&index[i][j+1]==0){
				r = j;
				break;
			}
			else if(j<=3){
				r=j+3;
				r_find_flag =1;
				break;
			}
			 if(j<=r-r_left_find+1){
				r_find_flag =1;
				break;
			}
		}
		//向右寻找右边线
		if(r_find_flag==1){
			for(uint8 j=r;j<r+r_right_find;j++){
				if(index[i][j-1]==255&&index[i][j]==255&&index[i][j+1]==0){
					r = j;
					r_find_flag = 0;
					break;
				}
				else if(j==MT9V03X_W-2){
					r = MT9V03X_W-2;
					m_r_find_flag =1;
					r_find_flag = 0;
					break;
				}
				else if(j==r+r_right_find-1){
					m_r_find_flag =1;
					r_find_flag = 0;
					break;
				}
			}
		}
		
		//从1/2处寻找右边线(环境光问题)
		if(m_r_find_flag==1){
			for(uint8 j=MT9V03X_W/2;j<MT9V03X_W-1;j++){
				
				if(index[i][j-1]==255&&index[i][j]==255&&index[i][j+1]==0){
					r = j;
					m_r_find_flag = 0;
					break;
				}
				
				else if(j==MT9V03X_W-2){
					r = MT9V03X_W-2;
					m_r_find_flag = 0;
					break;
				}
		
			}
		}
		
		//存储边线以及中线
		left_line[i] = limit_uint8(1,l,MT9V03X_W-2);
		right_line[i] = limit_uint8(1,r,MT9V03X_W-2);
		mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
//		if(image[i][mid_line[i]]==0){
//			mid_line[i]=mid_line[i+1];
//		}
	}
	
	uint8 circle1_flag=0;							//圆环状态一标志位
	uint8 circle3_flag=0;							//圆环状态三标志位
	uint8 circle5_flag=0;							//圆环状态五标志位
	uint8 circle8_flag=0;							//圆环状态八标志位
	
	//十字补线
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		//计算两边丢线数量用作判断是否进入十字
		if(left_line[i]<=5&&right_line[i]>=MT9V03X_W-7){
			
			num_loss++;
			
		}		
		
		//计算左边未丢线右边丢线数量用作判断是否进入圆环状态1的依据之一
		if(left_line[i]>=5&&right_line[i]>=MT9V03X_W-7){
			num_lossr++;
			
		}
		
		//计算右边未丢线左边丢线数量
		if(left_line[i]<=5&&right_line[i]<=MT9V03X_W-7){
			num_lossl++;
			
		}
		
	}
	
	//拐点清零
	if(num_loss+num_lossr+num_lossl<=5){
		x1=0;y1=0;
		x2=0;y2=0;
		x3=0;y3=0;
		x4=0;y4=0;
		kl=0;kr=0;
	}
	
	//拐点清零
	if(num_lossr<=3&&num_loss<=10){
		x5=0;y5=0;
		x6=0;y6=0;
		x7=0;y7=0;
		x8=0;y8=0;
		k1=0;k3=0;
	}
	
//	//右边丢线数超过25则判断进入圆环状态
//	if(num_lossr>=15&&(num_loss<=10||x6!=0)){//&&((x5!=0&&y5!=0)||(x7!=0&&y7!=0))){
//		
//		for(uint8 i=bottom_line;i>25;i--){
//			//判断圆处是否出现切点用作判断是否进入圆环状态1的依据之一
//			if(i<70&&i>30&&right_line[i-1]-right_line[i]>=1&&right_line[i+1]-right_line[i]>=0&&right_line[i-2]-right_line[i]>=1&&right_line[i+2]-right_line[i]>=0){
//				if(x6==0&&left_line[i+15]>=5&&right_line[i+15]>=MT9V03X_W-7){
//					x6=right_line[i];
//					y6=i;
//					count_beep=500;
//				}

//				else if(x6!=0){
//					x6=right_line[i];
//					y6=i;
//				}
//				
//			}
//			if(count_beep)
//        {
//            gpio_set_level(BEEP, GPIO_HIGH);
//            count_beep --;
//        }
//        else
//        {
//            gpio_set_level(BEEP, GPIO_LOW);
//        }
//			if(x6!=0){
//				
//				//判断右边是否出现拐点并且拐点上方右边丢线左边未丢线用作判断是否进入圆环状态1的依据之一
//				if(i>80&&left_line[i-3]>=5&&right_line[i-3]>=MT9V03X_W-7&&left_line[i-4]>=5&&right_line[i-4]>=MT9V03X_W-7&&right_line[i+1]-right_line[i]>=1&&right_line[i-1]-right_line[i]>=1){
//					
//					x5=right_line[i];
//					y5=i;
//			
//				}

//				//判断右上方是否出现拐点并且拐点下方右边丢线左边未丢线用作判断是否进入圆环状态3的依据之一
//				if(i<y6&&i>40&&left_line[i+3]>=5&&right_line[i+3]>=MT9V03X_W-7&&left_line[i+4]>=5&&right_line[i+4]>=MT9V03X_W-7&&right_line[i+3]-right_line[i]>=20&&right_line[i]-right_line[i-1]<=3){
//					
//					x7=right_line[i];
//					y7=i;
//				
//				}
//				
//				//判断左下方是否出现拐点作为是否进入圆环状态5的依据之一
//				if(i>60&&left_line[i]-left_line[i+1]>=1&&left_line[i]-left_line[i-1]>=1&&left_line[i]>=3&&left_line[i]>=10){
//					
//					x9=left_line[i];
//					y9=i;
//					
//				}
//				
//				//如果右边出现拐点并且圆处有切点并且右上方没有拐点则进入状态1
//				if(x5!=0&&x7==0&&y6<=65){
//					
//					//圆环状态1标志位置1
//					circle1_flag=1;
//					
//					//计算斜率
//					k1=((float)(y6-y5))/(float)(x6-x5);
//					
//				}
//				//右上方出现拐点则进入状态3
//				else if(x7!=0&&x9==0&&left_line[40]>=5){
//					
//					//圆环状态3标志位置1
//					circle3_flag=1;
//					
//					//将左边底角边界点作为补线起始点
//					x8=left_line[90];
//					y8=90;
//					
//					//计算斜率
//					k3=((float)(y7-y8))/(float)(x7-x8);
//					
//				}
//				else if(x9!=0&&left_line[35]<=MT9V03X_W-7){
//					
//					//圆环状态5标志位置1
//					circle5_flag=1;
//					
//					if(k5==0){
//						
//						//计算斜率
//						k5=((float)(y9-(y9+2)))/(float)(x9-left_line[y9+2]);
//						
//					}
//					
//				}
//				else if(left_line[35]>MT9V03X_W-7){
//					
//					//圆环状态8标志位置1
//					circle8_flag=1;
//					
//					x7=0;
//					y7=0;
//					
//				}
//						
//				//状态1进行补线
//				if(circle1_flag==1&&y6<50){
//					
//					for(uint8 i=y5;i>y6;i--){
//						
//						right_line[i]=x5+(i-y5)/k1;
//						mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
//							
//								
//					}
//					
//					//圆环状态1标志位置0
//					circle1_flag=0;
//					
//				}
//				
//				//状态3进行补线
//				else if(circle3_flag==1){
//					
//					for(uint8 i=y8;i>y7;i--){
//						
//						left_line[i]=x8+(i-y8)/k3;
//						mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
//						
//					}
//					
//					//圆环状态3标志位置0
//					circle3_flag=0;
//					
//				}
//				
////				//状态5进行补线
////				else if(circle5_flag==1){
////				
////					for(uint8 i=y9+2;i>40;i--){
////						
////						left_line[i]=x9+(i-y9)/k5;
////						mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);			
////						
////					}
////					
////					//圆环状态5标志位置0
////					circle5_flag=0;
////					
////				}
////				
////				//状态8进行补线
////				else if(circle8_flag==1&&x7!=0){
////					//计算斜率
////					if(k8==0){
////						
////						k8=((float)(y7-(y7-2)))/(float)(x7-left_line[y7-2]);
////						
////					}
////					
////					for(uint8 i=y7-2;i<110;i++){
////						
////						left_line[i]=x7+(i-y7)/k8;
////						mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);			
////						
////					}
////					
////					//圆环状态8标志位置0
////					circle8_flag=0;
////					
////				}
//				
//				//计数清零
//				num_lossr=0;
//		
//			}

//		}
//		
//	}
	
	//判断丢线数量若大于15则判断进入十字
	if(x6==0&&(num_loss>10)){
		
		for(uint8 i=110;i>80;i--){
			
			//记录左下拐点
			if(left_line[i]-left_line[i+1]>=1&&left_line[i]-left_line[i-1]>=1&&left_line[i]>=3&&left_line[i]>=10){
				x1=left_line[i];
				y1=i;
			}

			//记录右下拐点
			else if(right_line[i+1]-right_line[i]>=1&&right_line[i-1]-right_line[i]>=1&&right_line[i]<=MT9V03X_W-3){
				x2=right_line[i];
				y2=i;
			}

		}
		for(uint8 i=70;i>=40;i--){
			
			//记录左上拐点
			if(left_line[i]-left_line[i+4]>=10&&left_line[i]>20){
				x3=left_line[i];
				y3=i;
			}
			 
			//记录右上拐点
			else if(right_line[i+3]-right_line[i]>=15&&right_line[i]-right_line[i-2]<=5&&right_line[i]<MT9V03X_W-5){
				x4=right_line[i];
				y4=i;
			}
			
		}
		if(y3>=100||y4>=100){
			x1=0;y1=0;
			x2=0;y2=0;
			x3=0;y3=0;
			x4=0;y4=0;
			kl=0;kr=0;
		}
		for(uint8 i=y2;i>40;i--){
			if(x2!=0&&x4==0){
				right_line[i]=x2+(i-y2)/1.8;
			}
			else{
				break;
			}
		}
		for(uint8 i=y1;i>40;i--){
			if(x1!=0&&x3==0){
				left_line[i]=x1+(i-y1)/-1.5;
			}
			else{
				break;
			}
		}
		//计数清零
		num_loss=0;
		num_lossl=0;
		num_lossr=0;
		
		//计算左边斜率
		if(x3!=0&&x1!=0){
			kl=((float)(y3-y1))/(float)(x3-x1);
		}
		else if((x3!=0&&x1==0)||kl>0){
			kl=-1.5;
		}
		
		//计算右边斜率
		if(x4!=0&&x2!=0){
			kr=((float)(y4-y2))/(float)(x4-x2);
		}
		else if((x4!=0&&x2==0)||kr<0){
			kr=1.8;
		}
		
		if(x1!=0&&x2!=0&&kl<0&&kr>0){
			//进行补线
			for(uint8 i=max_uint8(y1,y2);i>mini_uint8(y4,y3);i--){
				
					left_line[i]=x1+(i-y1)/kl;
					right_line[i]=x2+(i-y2)/kr;
					mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		else if(x1==0&&kl<0&&kr>0){
			//进行补线
			for(uint8 i=bottom_line;i>mini_uint8(y4,y3);i--){
				
				left_line[i]=x3+(i-y3)/kl;
				right_line[i]=x2+(i-y2)/kr;
				mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
		}
		else if(x2==0&&kl<0&&kr>0){
			//进行补线
			for(uint8 i=bottom_line;i>mini_uint8(y4,y3);i--){
				
				left_line[i]=x1+(i-y1)/kl;
				right_line[i]=x4+(i-y4)/kr;
				mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
				if(image[mid_line[i]][i]!=255){
					mid_line[i]=mid_line[i-1];
				}
							
			}
		}
		
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
	1,1,1,1,1,1,6,11,14,17,
	20,20,20,20,18,16,14,12,10,7,
	4,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
};

uint8 final_mid_value = MT9V03X_W/2;			//最终中线值

//加权求中线值
uint8 weight_find_mid_value(){
	
	
	uint8 last_mid_line = MT9V03X_W/2;			//上一次的中线值
	uint8 mid_v = MT9V03X_W/2;							//最终中线值
	uint8 now_mid_line = MT9V03X_W/2;				//当下中线值
	uint32 weight_mid_line_sum=0;							//中线权重和
	uint32 weight_sum=0;											//总权重
	
	for(uint8 i=bottom_line;i>find_end_line-1;i--){
		
		weight_mid_line_sum += mid_line[i] *mid_weight[i];
		weight_sum += mid_weight[i];

		
	}
	
	if(now_mid_line-last_mid_line>=25||last_mid_line-now_mid_line>=25){
		now_mid_line=last_mid_line;
	}
	
	now_mid_line = (uint8)(weight_mid_line_sum/weight_sum);
	mid_v = now_mid_line*0.9+last_mid_line*0.1;			//互补滤波
	last_mid_line =now_mid_line;
	

	
	return mid_v;
}


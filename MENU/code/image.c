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
	
	for(uint8 i=0;i<40;i++){			//远端图像阈值+10
		
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
			num1++;
			
		}

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
			num1++;
			
		}

		num=0;
	}
	
	if(num1==0||find_end_line<=25){
		
		find_end_line=25;
		
	}
	
}





#define bottom_line 119			//底线坐标

uint8 l_point;							//左基点
uint8 r_point;							//右基点


//找图像边线基点
void find_basic_point(uint8 bio_image[MT9V03X_H][MT9V03X_W]){
	
	//从1/2处开始找
	if(bio_image[bottom_line][MT9V03X_W/2]==255&&bio_image[bottom_line][MT9V03X_W/2+1]==255&&bio_image[bottom_line][MT9V03X_W/2-1]==255){
		
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

uint8 abs_subtact_uint8(uint8 a,uint8 b){
	if(a>=b){
		
		return a-b;
		
	}
	
	else{
		
		return b-a;
		
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

uint16 count_beep = 0;								//蜂鸣器计数

cross cross_point={0};								//十字拐点初始化
cross cross_zero_point={0};						//十字拐点清零
circle circle_point={0};							//圆环拐点初始化
circle circle_zero_point={0};					//圆环拐点清零

f_line find_line_flag={0};						//寻找边线标志位初始化
f_line find_line_zeroflag={0};				//寻找边线标志位清零
f_circle circle_stage_flag={0};				//圆环状态标志位
f_circle circle_stage_zeroflag={0};		//圆环状态标志位清零

Num loss_num={0};											//丢线数初始化
Num loss_zeronum={0};									//丢线数清零

//寻找边线
void find_line(uint8 index[MT9V03X_H][MT9V03X_W]){
	
	uint8 l = l_point;									//中间量存储
	uint8 r = r_point;									//中间量存储
	
	//从下往上开始寻找边线截止到搜寻截止行
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		//向右寻找左边线
		for(uint8 j=l;j<l+l_right_find;j++){
			
			if(index[i][j-1]==0&&index[i][j]==255&&index[i][j+1]==255){
				
				l = j;
				break;
				
			}
			
			else if(j>=MT9V03X_W-2){
				
				l=j-3;
				find_line_flag.l_find_flag =1;
				break;
				
			}
			
			else if(j==l+l_right_find-1){
				
				find_line_flag.l_find_flag =1;
				break;
				
			}
			
		}
		
		//向左寻找左边线
		if(find_line_flag.l_find_flag==1){
			
			for(uint8 j=l;j>l-l_left_find;j--){
				
				if(index[i][j-1]==0&&index[i][j]==255&&index[i][j+1]==255){
					
					l = j;
					break;
					
				}
				
				else if(j==1){
					
					l = 1;
					find_line_flag.m_l_find_flag =1;
					break;
					
				}
				
				else if(j==l-l_left_find+1){
					
					find_line_flag.m_l_find_flag =1;
					break;
					
				}
				
			}
			
		}
		
		//从1/2处寻找左边线（环境光问题）
		if(find_line_flag.m_l_find_flag==1){
			
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
		
		//向左寻找右边线
		for(uint8 j=r;j>r-r_left_find;j--){
			
			if(index[i][j-1]==255&&index[i][j]==255&&index[i][j+1]==0){
				
				r = j;
				break;
				
			}
			
			else if(j<=3){
				
				r=j+3;
				find_line_flag.r_find_flag =1;
				break;
				
			}
			
			 if(j<=r-r_left_find+1){
				 
				find_line_flag.r_find_flag =1;
				break;
				 
			}
			 
		}
		
		//向右寻找右边线
		if(find_line_flag.r_find_flag==1){
			
			for(uint8 j=r;j<r+r_right_find;j++){
				
				if(index[i][j-1]==255&&index[i][j]==255&&index[i][j+1]==0){
					
					r = j;
					break;
				}
				
				else if(j==MT9V03X_W-2){
					
					r = MT9V03X_W-2;
					find_line_flag.m_r_find_flag =1;
					break;
					
				}
				
				else if(j==r+r_right_find-1){
					
					find_line_flag.m_r_find_flag =1;
					break;
					
				}
				
			}
			
		}
		
		//从1/2处寻找右边线(环境光问题)
		if(find_line_flag.m_r_find_flag==1){
			
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
		
		//若中线为黑色则改变搜寻截止行
		if(image[i][mid_line[i]]==0){
			
			find_end_line = i;
			
		}
		
		//标志位清零
		find_line_flag=find_line_zeroflag;
		
	}
	
	
	
	//十字补线
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		//计算两边丢线数量用作判断是否进入十字
		if(left_line[i]<=5&&right_line[i]>=MT9V03X_W-7){
			
			loss_num.num_loss++;
			
		}		
		
		//计算左边未丢线右边丢线数量用作判断是否进入圆环状态1的依据之一
		if(left_line[i]>=5&&right_line[i]>=MT9V03X_W-7){
			
			loss_num.num_lossr++;
			
		}
		
		//计算右边未丢线左边丢线数量
		if(left_line[i]<=5&&right_line[i]<=MT9V03X_W-7){
			
			loss_num.num_lossl++;
			
		}
		
	}
	
	//拐点清零
	if(loss_num.num_loss+loss_num.num_lossr+loss_num.num_lossl<=5){
		
		cross_point=cross_zero_point;
		
	}
	
	//拐点清零
	if(loss_num.num_lossr<=3&&loss_num.num_loss<=10){
		
		circle_point=circle_zero_point;
		
	}
	
//	//右边丢线数超过25则判断进入圆环状态
//	if(loss_num.num_lossr>=15&&(loss_num.num_loss<=10||circle_point.x6!=0)){//&&((x5!=0&&y5!=0)||(x7!=0&&y7!=0))){
//		
//		for(uint8 i=bottom_line;i>25;i--){
//			//判断圆处是否出现切点用作判断是否进入圆环状态1的依据之一
//			if(i<70&&i>30&&right_line[i-1]-right_line[i]>=1&&right_line[i+1]-right_line[i]>=0&&right_line[i-2]-right_line[i]>=1&&right_line[i+2]-right_line[i]>=0){
//				if(circle_point.x6==0&&left_line[i+15]>=5&&right_line[i+15]>=MT9V03X_W-7){
//					circle_point.x6=right_line[i];
//					circle_point.y6=i;
//					//蜂鸣器计数
//					count_beep=500;
//				}

//				else if(circle_point.x6!=0){
//					circle_point.x6=right_line[i];
//					circle_point.y6=i;
//				}
//				
//			}
//			//蜂鸣器
//			if(count_beep)
//        {
//            gpio_set_level(BEEP, GPIO_HIGH);
//            count_beep --;
//        }
//        else
//        {
//            gpio_set_level(BEEP, GPIO_LOW);
//        }
//			if(circle_point.x6!=0){
//				
//				//判断右边是否出现拐点并且拐点上方右边丢线左边未丢线用作判断是否进入圆环状态1的依据之一
//				if(i>80&&left_line[i-3]>=5&&right_line[i-3]>=MT9V03X_W-7&&left_line[i-4]>=5&&right_line[i-4]>=MT9V03X_W-7&&right_line[i+1]-right_line[i]>=1&&right_line[i-1]-right_line[i]>=1){
//					
//					circle_point.x5=right_line[i];
//					circle_point.y5=i;
//			
//				}

//				//判断右上方是否出现拐点并且拐点下方右边丢线左边未丢线用作判断是否进入圆环状态3的依据之一
//				if(i<circle_point.y6&&i>40&&left_line[i+3]>=5&&right_line[i+3]>=MT9V03X_W-7&&left_line[i+4]>=5&&right_line[i+4]>=MT9V03X_W-7&&right_line[i+3]-right_line[i]>=20&&right_line[i]-right_line[i-1]<=3){
//					
//					circle_point.x7=right_line[i];
//					circle_point.y7=i;
//				
//				}
//				
//				//判断左下方是否出现拐点作为是否进入圆环状态5的依据之一
//				if(i>60&&left_line[i]-left_line[i+1]>=1&&left_line[i]-left_line[i-1]>=1&&left_line[i]>=3&&left_line[i]>=10){
//					
//					circle_point.x9=left_line[i];
//					circle_point.y9=i;
//					
//				}
//				
//				//如果右边出现拐点并且圆处有切点并且右上方没有拐点则进入状态1
//				if(circle_point.x5!=0&&circle_point.x7==0&&circle_point.y6<=65){
//					
//					//圆环状态1标志位置1
//					circle_stage_flag.circle1_flag=1;
//					
//					//计算斜率
//					circle_point.k1=((float)(circle_point.y6-circle_point.y5))/(float)(circle_point.x6-circle_point.x5);
//					
//				}
//				//右上方出现拐点则进入状态3
//				else if(circle_point.x7!=0&&circle_point.x9==0&&left_line[40]>=5){
//					
//					//圆环状态3标志位置1
//					circle_stage_flag.circle3_flag=1;
//					
//					//将左边底角边界点作为补线起始点
//					circle_point.x8=left_line[90];
//					circle_point.y8=90;
//					
//					//计算斜率
//					circle_point.k3=((float)(circle_point.y7-circle_point.y8))/(float)(circle_point.x7-circle_point.x8);
//					
//				}
//				else if(circle_point.x9!=0&&left_line[35]<=MT9V03X_W-7){
//					
//					//圆环状态5标志位置1
//					circle_stage_flag.circle5_flag=1;
//					
//					if(circle_point.k5==0){
//						
//						//计算斜率
//						circle_point.k5=((float)(circle_point.y9-(circle_point.y9+2)))/(float)(circle_point.x9-left_line[circle_point.y9+2]);
//						
//					}
//					
//				}
//				else if(left_line[35]>MT9V03X_W-7){
//					
//					//圆环状态8标志位置1
//					circle_stage_flag.circle8_flag=1;
//					
//					circle_point.x7=0;
//					circle_point.y7=0;
//					
//				}
//						
//				//状态1进行补线
//				if(circle_stage_flag.circle1_flag==1&&circle_point.y6<50){
//					
//					for(uint8 i=circle_point.y5;i>circle_point.y6;i--){
//						
//						right_line[i]=circle_point.x5+(i-circle_point.y5)/circle_point.k1;
//						mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
//							
//								
//					}
//					
//				}
//				
//				//状态3进行补线
//				else if(circle_stage_flag.circle3_flag==1){
//					
//					for(uint8 i=circle_point.y8;i>circle_point.y7;i--){
//						
//						left_line[i]=circle_point.x8+(i-circle_point.y8)/circle_point.k3;
//						mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
//						
//					}
//					
//				}
//				
//				//状态5进行补线
//				else if(circle_stage_flag.circle5_flag==1){
//				
//					for(uint8 i=circle_point.y9+2;i>40;i--){
//						
//						left_line[i]=circle_point.x9+(i-circle_point.y9)/circle_point.k5;
//						mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);			
//						
//					}
//					
//				}
//				
//				//状态8进行补线
//				else if(circle_stage_flag.circle8_flag==1&&circle_point.x7!=0){
//					//计算斜率
//					if(circle_point.k8==0){
//						
//						circle_point.k8=((float)(circle_point.y7-(circle_point.y7-2)))/(float)(circle_point.x7-left_line[circle_point.y7-2]);
//						
//					}
//					
//					for(uint8 i=circle_point.y7-2;i<110;i++){
//						
//						left_line[i]=circle_point.x7+(i-circle_point.y7)/circle_point.k8;
//						mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);			
//						
//					}
//					
//				}
//				
//				//计数清零
//				loss_num.num_lossr=0;
//				
//				//圆环标志位清零
//				circle_stage_flag=circle_stage_zeroflag;
//		
//			}

//		}
//		
//	}
	
	//判断丢线数量若大于15则判断进入十字
	if(circle_point.x6==0&&loss_num.num_loss>10){
		
		for(uint8 i=110;i>80;i--){
			
			//记录左下拐点
			if(left_line[i]-left_line[i+1]>=1&&left_line[i]-left_line[i-1]>=1&&left_line[i]>=3&&left_line[i]>=10){
				
				cross_point.x1=left_line[i];
				cross_point.y1=i;
				
				if(cross_point.x1>=94){
					
					cross_point.x1=0;
					cross_point.y1=0;
					
				}
				
			}

			//记录右下拐点
			else if(right_line[i+1]-right_line[i]>=1&&right_line[i-1]-right_line[i]>=1&&right_line[i]<=MT9V03X_W-3){
				
				cross_point.x2=right_line[i];
				cross_point.y2=i;
				
				if(cross_point.x2<=94){
					
					cross_point.x2=0;
					cross_point.y2=0;
					
				}
				
				
			}

		}
		
		for(uint8 i=70;i>=40;i--){
			
			//记录左上拐点
			if(left_line[i]-left_line[i+4]>=10&&left_line[i]>20){
				
				cross_point.x3=left_line[i];
				cross_point.y3=i;
				
				if(cross_point.x3>=94){
					
					cross_point.x3=0;
					cross_point.y3=0;
					
				}
				
				
			}
			 
			//记录右上拐点
			else if(right_line[i+3]-right_line[i]>=15&&right_line[i]-right_line[i-2]<=5&&right_line[i]<MT9V03X_W-5){
				
				cross_point.x4=right_line[i];
				cross_point.y4=i;
				
				if(cross_point.x4<=94){
					
					cross_point.x4=0;
					cross_point.y4=0;
					
				}
				
			}
			
		}
		
		//十字补线状态清除
		if(cross_point.y3>=100||cross_point.y4>=100){
			
			cross_point=cross_zero_point;
			
		}
		
		//计数清零
		loss_num=loss_zeronum;
		
		//计算左边斜率
		if(cross_point.x3!=0&&cross_point.x1!=0){
			
			cross_point.kl=((float)(cross_point.y3-cross_point.y1))/(float)(cross_point.x3-cross_point.x1);
			
		}
		else if((cross_point.x3!=0&&cross_point.x1==0)||(cross_point.x3==0&&cross_point.x1!=0)||cross_point.kl>0){
			
			cross_point.kl=-1.5;
			
		}
		
		//计算右边斜率
		if(cross_point.x4!=0&&cross_point.x2!=0){
			
			cross_point.kr=((float)(cross_point.y4-cross_point.y2))/(float)(cross_point.x4-cross_point.x2);
			
		}
		else if((cross_point.x4!=0&&cross_point.x2==0)||(cross_point.x4==0&&cross_point.x2!=0)||cross_point.kr<0){
			
			cross_point.kr=1.8;
			
		}
		
		//若左拐点都存在
		if(cross_point.x1!=0&&cross_point.x3!=0&&cross_point.kl<0){
			
			//进行补线
			for(uint8 i=cross_point.y1;i>cross_point.y3;i--){
				
				left_line[i]=cross_point.x1+(i-cross_point.y1)/cross_point.kl;
				mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
		//若左下拐点不存在
		else if(cross_point.x1==0&&cross_point.x3!=0&&cross_point.kl<0){
			
			//进行补线
			for(uint8 i=cross_point.y3;i<bottom_line;i++){
				
				left_line[i]=cross_point.x3+(i-cross_point.y3)/cross_point.kl;
				mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
		//若左上拐点不存在
		else if(cross_point.x1!=0&&cross_point.x3==0&&cross_point.kl<0){
			
			//进行补线
			for(uint8 i=cross_point.y1;i>40;i--){
				
				left_line[i]=cross_point.x1+(i-cross_point.y1)/cross_point.kl;
				mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
		//若右拐点都存在
		if(cross_point.x2!=0&&cross_point.x4!=0&&cross_point.kr>0){
			
			//进行补线
			for(uint8 i=cross_point.y2;i>cross_point.y4;i--){
				
					right_line[i]=cross_point.x2+(i-cross_point.y2)/cross_point.kr;
					mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
		//若右下拐点不存在
		else if(cross_point.x2==0&&cross_point.x4!=0&&cross_point.kr>0){
			
			//进行补线
			for(uint8 i=cross_point.y4;i<bottom_line;i++){
				
					right_line[i]=cross_point.x4+(i-cross_point.y4)/cross_point.kr;
					mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
		//若右上拐点不存在
		else if(cross_point.x2!=0&&cross_point.x4==0&&cross_point.kr>0){
			
			//进行补线
			for(uint8 i=cross_point.y2;i>40;i--){
				
					right_line[i]=cross_point.x2+(i-cross_point.y2)/cross_point.kr;
					mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
	}
	
	//计数清零
	loss_num=loss_zeronum;
	
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
	
	uint8 last_mid_line = MT9V03X_W/2;				//上一次的中线值
	uint8 mid_v = MT9V03X_W/2;								//最终中线值
	uint8 now_mid_line = MT9V03X_W/2;					//当下中线值
	
	uint32 weight_mid_line_sum = 0;						//中线权重和
	uint32 weight_sum = 0;										//总权重
	
	//计算中线权重和以及总权重
	for(uint8 i=bottom_line;i>find_end_line-1;i--){
		
		weight_mid_line_sum += mid_line[i] *mid_weight[i];
		weight_sum += mid_weight[i];

		
	}
	
	//防止中值突变
	if(now_mid_line-last_mid_line>=25||last_mid_line-now_mid_line>=25){
		
		now_mid_line = last_mid_line;
		
	}
	
	//计算中值
	now_mid_line = (uint8)(weight_mid_line_sum/weight_sum);
	
	//互补滤波
	mid_v = now_mid_line*0.9+last_mid_line*0.1;		
	
	//更新上一次中值
	last_mid_line = now_mid_line;
	
	return mid_v;
	
}


#include "zf_common_headfile.h"

uint8 basic_image[MT9V03X_H][MT9V03X_W];  					//ͼ���ƺ�Ļ���ͼ��
uint8 image[MT9V03X_H][MT9V03X_W];    							//��ֵ�����ͼ��

//��ȡͼ��
void get_image(){
	
	memcpy(basic_image, mt9v03x_image, MT9V03X_H*MT9V03X_W);
	
}

//�������ֵ
#define GrayScale 256
uint16 hist[GrayScale] ={0};    //�Ҷ�ֵ���ص����������ֵ��ţ�ֱ��ͼ
float P[GrayScale] = {0};    		//ÿ���Ҷȳ��ָ���
float PK[GrayScale] = {0};			//�����ۼƺ�
float MK[GrayScale] = {0};			//�Ҷ�ֵ�ۼӾ�ֵ
uint8 threshold;								//�����ֵ

uint8 Otsu(uint8 index_image[MT9V03X_H][MT9V03X_W]){
	
	uint8 image_threshold;
	float imagesize = MT9V03X_H*MT9V03X_W;
	uint8 image_temp;
	
	float sumPK =0;				//�����ۼƺ�
	float sumMK =0;				//�Ҷ�ֵ�ۼӾ�ֵ
	float var =0;
	float vartmp =0;
	
	//����
	for(uint16 i=0;i<GrayScale;i++){
		
		hist[i]=0;
		P[i]=0;
		PK[i]=0;
		MK[i]=0;
		
	}
	
	//ֱ��ͼ
	for(uint8 i=0;i<MT9V03X_H;i++){
		
		for(uint16 j=0;j<MT9V03X_W;j++){
			
			image_temp = index_image[i][j];
			hist[image_temp]++;
			
		}
	}
	
	//����䷽��
	for(uint16 i=0;i<GrayScale;i++){
		
		P[i]=(float)hist[i]/imagesize;
		
		PK[i]=sumPK+P[i];
		sumPK=PK[i];
		
		MK[i]=sumMK+i*P[i];
		sumMK=MK[i];
		
	}
	
	//��������䷽���µ���ֵ
	for(uint8 i=5;i<245;i++){
		
		vartmp = ((MK[GrayScale-1]*PK[i]-MK[i])*(MK[GrayScale-1]*PK[i]-MK[i]))/(PK[i]*(1-PK[i]));
		
		if(vartmp>var){
			
			var = vartmp;
		  image_threshold = i;
			
		}
		
	}
	
	return image_threshold;
}

//ͼ��ֶζ�ֵ��
void image_binaryzation(uint8 thershold){
	
	uint8 temp;
	
	for(uint8 i=0;i<40;i++){							//Զ��ͼ����ֵ+10
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
	
	for(uint8 i=40;i<MT9V03X_H;i++){			//���˲���
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
//��ͼ����߻���
void find_basic_point(uint8 bio_image[MT9V03X_H][MT9V03X_W]){
	//��1/2����ʼ��
	if(bio_image[bottom_line][MT9V03X_W/2]==255&&bio_image[bottom_line][MT9V03X_W/2+1]==255&&bio_image[bottom_line][MT9V03X_W/2-1]==255){
		flag1_2=1;
		flag1_4=0;
		flag3_4=0;
		//Ѱ�������
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
		//Ѱ���һ���
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
	//��1/4����ʼ��
	else if(bio_image[bottom_line][MT9V03X_W/4]==255&&bio_image[bottom_line][MT9V03X_W/4+1]==255&&bio_image[bottom_line][MT9V03X_W/4-1]==255){
		flag1_2=0;
		flag1_4=1;
		flag3_4=0;
		//Ѱ�������
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
		//Ѱ���һ���
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
	//��3/4����ʼ��
	else if(bio_image[bottom_line][MT9V03X_W/4*3]==255&&bio_image[bottom_line][MT9V03X_W/4*3+1]==255&&bio_image[bottom_line][MT9V03X_W/4*3-1]==255){
		flag1_2=0;
		flag1_4=0;
		flag3_4=1;
		//Ѱ�������
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
		//Ѱ���һ���
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

//�޷�����
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


#define find_end_line 20   						//Ѱ�ҽ�ֹ��
#define l_right_find 20								//���������Ѱ��20������
#define l_left_find 10								//���������Ѱ��10������
#define r_right_find 10								//�ұ�������Ѱ��10������
#define r_left_find 20								//�ұ�������Ѱ��20������
uint8 left_line[MT9V03X_H]={0};				//����ߴ洢
uint8 right_line[MT9V03X_H]={0};			//�ұ��ߴ洢
uint8 mid_line[MT9V03X_H]={0};				//���ߴ洢
uint8 x1,x2,x3,x4,y1,y2,y3,y4;				//�յ�����

//Ѱ�ұ���
void find_line(uint8 index[MT9V03X_H][MT9V03X_W]){
	
	uint8 l = l_point;
	uint8 r = r_point;
	uint8 num=0;
	uint8 position[120]={0};
	
	//�������Ͽ�ʼѰ�ұ��߽�ֹ��20
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		uint8 l_find_flag=0;
		uint8 m_l_find_flag=0;
		uint8 r_find_flag=0;
		uint8 m_r_find_flag=0;
		
		//����Ѱ�������
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
		
		//����Ѱ�������
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
		
		//��3/4��Ѱ�������
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
		
		//����Ѱ���ұ���
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
		//����Ѱ���ұ���
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
		
		//��1/4��Ѱ���ұ���
		if(m_r_find_flag==1){
			for(uint8 j=MT9V03X_W/4;j<MT9V03X_W-1;j++){
				
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
		
		//�洢�����Լ�����
		left_line[i] = limit_uint8(1,l,MT9V03X_W-2);
		right_line[i] = limit_uint8(1,r,MT9V03X_W-2);
		mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
	}
	
	uint8 yl[10]={0},yr[10]={0};
	uint8 yl_sum=0,yr_sum=0;
	uint8 yl_av=0,yr_av=0;
	uint8 xl[10]={0},xr[10]={0};
	uint8 xl_sum=0,xr_sum=0;
	uint8 xl_av=0,xr_av=0;
	uint8 vl_power_sum=0,vl_product_sum=0,vr_power_sum=0,vr_product_sum=0;
	int kl,kr;
	
	//ʮ�ֲ���
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		//���㶪������
		if(left_line[i]==1&&right_line[i]==MT9V03X_W-2){
			
			position[i]=1;
			num++;
			
		}
		
	}
	
	//�ж϶�������������20���жϽ���ʮ��
	if(num>=50){
		for(uint8 i=bottom_line;i>find_end_line;i--){
			//��¼���¹յ�
			if(left_line[i]-left_line[i+1]>=1&&left_line[i]-left_line[i-1]>=1){
				x1=left_line[i];
				y1=i;
			}
			//��¼���Ϲյ�
			else if(left_line[i]-left_line[i-1]>=5&&left_line[i+1]-left_line[i]<=2){
				x3=left_line[i];
				y3=i;
			}
			//��¼���¹յ�
			else if(right_line[i+1]-right_line[i]>=1&&right_line[i-1]-right_line[i]>=1){
				x2=right_line[i];
				y2=i;
			}
			//��¼���Ϲյ�
			else if(right_line[i-1]-right_line[i]>=5&&right_line[i]-right_line[i+1]<=2){
				x4=right_line[i];
				y4=i;
			}
			num=0;
		}
		kl=(int)(y3-y1)/(x3-x1);
		kr=(int)(y4-y2)/(x4-x2);
//		//��С���˷��������
//		for(uint8 i=0;i<5;i++){
//			
//			//�ɼ�����
//			xl[i]=left_line[y1-i];
//			xl[i+5]=left_line[y3+i];
//			
//			yl[i]=y1-i;
//			yl[i+5]=y3+i;
//			
//			xr[i]=right_line[y2-i];
//			xr[i+5]=right_line[y4+i];
//			
//			yr[i]=y2-i;
//			yr[i+5]=y4+i;
//			
//			//����X��Y���
//			yl_sum+=yl[i]+yl[i+5];
//			yr_sum+=yr[i]+yr[i+5];
//			xl_sum+=xl[i]+xl[i+5];
//			xr_sum+=xr[i]+xr[i+5];
//			
//		}
//		
//		//������X��Yƽ��
//		xl_av=xl_sum/10;
//		xr_av=xr_sum/10;
//		yl_av=yl_sum/10;
//		yr_av=yr_sum/10;
//		
//		//����������ϵ�Kֵ��ʽ
//		for(uint8 i=0;i<10;i++){
//			vl_power_sum+=xl[i]*xl[i]-10*xl_av*xl_av;
//			vl_product_sum+=xl[i]*yl[i]-10*xl_av*yl_av;
//			
//			vr_power_sum+=xr[i]*xr[i]-10*xr_av*xr_av;
//			vr_product_sum+=xr[i]*yr[i]-10*xr_av*yr_av;
//			
//		}
//		
//		//��������б��
//		kl=(int)(vl_product_sum/vl_power_sum);
//		kr=(int)(vr_product_sum/vr_power_sum);
//		
		//���в���
		for(uint8 i=bottom_line;i>find_end_line;i--){
			
			if(position[i]==1){
				
				left_line[i]=x3+(i-y1)/kl;
				right_line[i]=x4+(i-y2)/kr;
				mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
				
			}
						
		}
		
	}
	
}

//����Ȩ��
//Ȩ��Խ����ת��Խ��
uint8 mid_weight[120]={
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	6,6,6,6,6,6,6,6,6,6,
	7,8,9,10,11,12,13,14,15,16,
	17,18,19,20,20,20,20,19,18,17,
	16,15,14,13,12,11,10,9,8,7,
	6,6,6,6,6,6,6,6,6,6,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,	
};

uint8 final_mid_value = MT9V03X_H/2;			//��������ֵ

//��Ȩ������ֵ
uint8 weight_find_mid_value(){
	
	uint8 last_mid_line = MT9V03X_H/2;			//��һ�ε�����ֵ
	uint8 mid_v = MT9V03X_H/2;							//��������ֵ
	uint8 now_mid_line = MT9V03X_H/2;				//��������ֵ
	uint32 weight_mid_line_sum;							//����Ȩ�غ�
	uint32 weight_sum;											//��Ȩ��
	
	for(uint8 i=bottom_line;i>find_end_line-1;i--){
		
		weight_mid_line_sum += mid_line[i] *mid_weight[i];
		weight_sum += mid_weight[i];
		
	}
	
	now_mid_line = (uint8)(weight_mid_line_sum/weight_sum);
	mid_v = now_mid_line*0.8+last_mid_line*0.2;			//�����˲�
	last_mid_line =now_mid_line;
	
	return mid_v;
}


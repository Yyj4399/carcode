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
#define l_right_find 10								//���������Ѱ��10������
#define l_left_find 5									//���������Ѱ��5������
#define r_right_find 5								//�ұ�������Ѱ��5������
#define r_left_find 10								//�ұ�������Ѱ��10������
uint8 left_line[MT9V03X_H]={0};				//����ߴ洢
uint8 right_line[MT9V03X_H]={0};			//�ұ��ߴ洢
uint8 mid_line[MT9V03X_H]={0};				//���ߴ洢

//Ѱ�ұ���
void find_line(uint8 index[MT9V03X_H][MT9V03X_W]){
	
	uint8 l = l_point;
	uint8 r = r_point;
	
	//�������Ͽ�ʼѰ�ұ��߽�ֹ��20
	for(uint8 i=bottom_line-1;i>find_end_line;i--){
		
		uint8 l_find_flag=0;
		uint8 m_l_find_flag=0;
		
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
					break;
				}
				else if(j==l-l_left_find+1){
					m_l_find_flag =1;
					break;
				}
			}
		}
		
		//���м�Ѱ�������
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
		
		//����Ѱ���ұ���
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
		
		//����Ѱ���ұ���
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
		
		//���м�Ѱ���ұ���
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
		
		//�洢�����Լ�����
		left_line[i] = limit_uint8(1,l,MT9V03X_W-2);
		right_line[i] = limit_uint8(1,r,MT9V03X_W-2);
		mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
	}
}

//����Ȩ��
//Ȩ��Խ����ת��Խ��
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

uint8 final_mid_line = MT9V03X_H/2;				//��������ֵ
uint8 last_mid_line = MT9V03X_H/2;				//��һ�ε�����ֵ


//��Ȩ������ֵ
uint8 weight_find_mid_line(){
	
	uint8 mid_value = MT9V03X_H/2;					//��������ֵ
	uint8 now_mid_line = MT9V03X_H/2;				//��������ֵ
	uint32 weight_mid_line_sum;							//����Ȩ�غ�
	uint32 weight_sum;											//��Ȩ��
	
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		weight_mid_line_sum +=mid_line[i] *mid_weight[i];
		weight_sum += mid_weight[i];
		
	}
	
	now_mid_line = (uint8)(weight_mid_line_sum/weight_sum);
	mid_value = now_mid_line*0.7+last_mid_line*0.3;			//�����˲�
	last_mid_line =now_mid_line;
	
	return mid_value;
}


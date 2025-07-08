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

//��Сֵ����
uint8 mini_uint8(uint8 a,uint8 b){
	if(a>=b){
		return b;
	}
	else if(a<b){
		return a;
	}
	return 0;
}

//���ֵ����
uint8 max_uint8(uint8 a,uint8 b){
	if(a>=b){
		return a;
	}
	else if(a<b){
		return b;
	}
	return 0;
}


#define find_end_line 25   						//Ѱ�ҽ�ֹ��
#define l_right_find 20								//���������Ѱ��20������
#define l_left_find 10								//���������Ѱ��10������
#define r_right_find 10								//�ұ�������Ѱ��10������
#define r_left_find 20								//�ұ�������Ѱ��20������
uint8 left_line[MT9V03X_H]={0};				//����ߴ洢
uint8 right_line[MT9V03X_H]={0};			//�ұ��ߴ洢
uint8 mid_line[MT9V03X_H]={0};				//���ߴ洢
uint8 x1,x2,x3,x4,y1,y2,y3,y4,x5=0,y5=0,x6=0,y6=0,x7=0,y7=0,x8=0,y8=0;				//�յ�����
float kl,kr;													//ʮ�ֲ���б��
float k1,k3;															//Բ������б��

//Ѱ�ұ���
void find_line(uint8 index[MT9V03X_H][MT9V03X_W]){
	
	uint8 l = l_point;									//�м����洢
	uint8 r = r_point;									//�м����洢
	uint8 num_loss=0;										//���߶��߼���
	uint8 num_lossr=0;									//�ұ߶��߼���
	uint8 num_lossl=0;									//��߶��߼���
	uint8 position[120]={0};						//ʮ�ֶ���λ�ü�¼
	
	//�������Ͽ�ʼѰ�ұ��߽�ֹ��20
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		uint8 l_find_flag=0;							//����������߱�־λ
		uint8 m_l_find_flag=0;						//���м�������߱�־λ
		uint8 r_find_flag=0;							//�������ұ��߱�־λ
		uint8 m_r_find_flag=0;						//���м����ұ��߱�־λ
		
		if(left_line[i]-left_line[i-2]>=20){
			m_l_find_flag=1;
		}
		
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
		
		//��1/2��Ѱ���ұ���
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
		
		//�洢�����Լ�����
		left_line[i] = limit_uint8(1,l,MT9V03X_W-2);
		right_line[i] = limit_uint8(1,r,MT9V03X_W-2);
		mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
	}
	
	uint8 circle1_flag=0;
	uint8 circle3_flag=0;
	//ʮ�ֲ���
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		//�������߶������������ж��Ƿ����ʮ��
		if(left_line[i]<=5&&right_line[i]>=MT9V03X_W-7){
			
			num_loss++;
			
		}
		
//		//�ж��ұ��Ƿ���ֹյ㲢�ҹյ��Ϸ��ұ߶������δ���������ж��Ƿ����Բ��״̬1������֮һ
//		if(i>80&&left_line[i-3]>=5&&right_line[i-3]>=MT9V03X_W-7&&left_line[i-4]>=5&&right_line[i-4]>=MT9V03X_W-7&&right_line[i+1]-right_line[i]>=1&&right_line[i-1]-right_line[i]>=1){
//			x5=right_line[i];
//			y5=i;
//		
//			
//		}
		
		//�������δ�����ұ߶������������ж��Ƿ����Բ��״̬1������֮һ
		if(left_line[i]>=5&&right_line[i]>=MT9V03X_W-7){
			num_lossr++;
			
		}
		
//		//�ж��ұ��Ƿ���ֹյ㲢�ҹյ��·��ұ߶������δ���������ж��Ƿ����Բ��״̬3������֮һ
//		if(left_line[i+1]>=5&&right_line[i+1]>=MT9V03X_W-7&&left_line[i+2]>=5&&right_line[i+2]>=MT9V03X_W-7&&right_line[i+1]-right_line[i]>=10&&right_line[i]-right_line[i-1]>=1){
//			x7=right_line[i];
//			y7=i;
//		
//		}
//		else{
//			x7=0;
//			y7=0;
//		}
		//�����ұ�δ������߶�������
		if(left_line[i]<=5&&right_line[i]<=MT9V03X_W-7){
			num_lossl++;
			
		}
		
	}
	
	//�յ�����
	if(num_loss+num_lossr+num_lossl<=5){
		x1=0;y1=0;
		x2=0;y2=0;
		x3=0;y3=0;
		x4=0;y4=0;
		kl=0;kr=0;
	}
	
	//�յ�����
	if(num_lossr<=5){
		x5=0;y5=0;
		x6=0;y6=0;
		x7=0;y7=0;
		x8=0;y8=0;
		k1=0;k3=0;
	}
	
	//�ұ߶���������25���жϽ���Բ��״̬
	if(num_lossr>=25){//&&((x5!=0&&y5!=0)||(x7!=0&&y7!=0))){
		
		for(uint8 i=bottom_line;i>25;i--){
			//�ж�Բ���Ƿ�����е������ж��Ƿ����Բ��״̬1������֮һ
			if(i<70&&right_line[i-1]-right_line[i]>=1&&right_line[i+1]-right_line[i]>=0&&right_line[i-2]-right_line[i]>=1&&right_line[i+2]-right_line[i]>=0){
				
					x6=right_line[i];
					y6=i;
				
			}
			if(x6!=0){
				//�ж��ұ��Ƿ���ֹյ㲢�ҹյ��Ϸ��ұ߶������δ���������ж��Ƿ����Բ��״̬1������֮һ
				if(i>80&&left_line[i-3]>=5&&right_line[i-3]>=MT9V03X_W-7&&left_line[i-4]>=5&&right_line[i-4]>=MT9V03X_W-7&&right_line[i+1]-right_line[i]>=1&&right_line[i-1]-right_line[i]>=1){
					
					x5=right_line[i];
					y5=i;
			
				}

				//�ж����Ϸ��Ƿ���ֹյ㲢�ҹյ��·��ұ߶������δ���������ж��Ƿ����Բ��״̬3������֮һ
				if(i<70&&left_line[i+1]>=5&&right_line[i+1]>=MT9V03X_W-7&&left_line[i+2]>=5&&right_line[i+2]>=MT9V03X_W-7&&right_line[i+1]-right_line[i]>=10&&right_line[i]-right_line[i-1]>=1){
					
					x7=right_line[i];
					y7=i;
				
				}
				
				//����ұ߳��ֹյ㲢��Բ�����е㲢�����Ϸ�û�йյ������״̬1
				if(x5!=0&&x7==0){
					
					//Բ��״̬1��־λ��1
					circle1_flag=1;
					
					//����б��
					k1=((float)(y6-y5))/(float)(x6-x5);
					
				}
				//���Ϸ����ֹյ������״̬3
				else if(x7!=0){
					
					//Բ��״̬3��־λ��1
					circle3_flag=1;
					
					//����ߵ׽Ǳ߽����Ϊ������ʼ��
					x8=left_line[119];
					y8=119;
					
					k3=((float)(y7-y8))/(float)(x7-x8);
					
				}		
						
				//���в���
				if(circle1_flag==1){
					
					for(uint8 i=y5;i>y6;i--){
						
							right_line[i]=x5+(i-y5)/k1;
							mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
								
					}
					
					//Բ��״̬1��־λ��0
					circle1_flag=0;
				}
				else if(circle3_flag==1){
					
					for(uint8 i=bottom_line;i>y6;i--){
						
						left_line[i]=x5+(i-y5)/k3;
						mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);			
						
					}
				}
				
				//��������
				num_lossr=0;
		
			}

		}
		
	}
	
	//�ж϶�������������40���жϽ���ʮ��
	if(x6==0&&(num_loss>=40||num_lossl>=30||num_lossr>=30)){
		
		for(uint8 i=bottom_line;i>80;i--){
			
			//��¼���¹յ�
			if(left_line[i]-left_line[i+1]>=1&&left_line[i]-left_line[i-1]>=1&&left_line[i]>=3&&left_line[i]>=10){
				x1=left_line[i];
				y1=i;
			}

			//��¼���¹յ�
			else if(right_line[i+1]-right_line[i]>=1&&right_line[i-1]-right_line[i]>=1&&right_line[i]<=MT9V03X_W-3){
				x2=right_line[i];
				y2=i;
			}

		}
		for(uint8 i=70;i>=40;i--){
			
			//��¼���Ϲյ�
			 if(left_line[i]-left_line[i+4]>=10&&left_line[i]>10&&left_line[i+4]<=3){
				x3=left_line[i];
				y3=i;
			}
			 
			//��¼���Ϲյ�
			else if(right_line[i+3]-right_line[i]>=20&&right_line[i]-right_line[i-2]<=5&&right_line[i]<MT9V03X_W-5&&right_line[i+3]>=MT9V03X_W-5){
				x4=right_line[i];
				y4=i;
			}
			
		}
		
		//��������
		num_loss=0;
		num_lossl=0;
		num_lossr=0;
		
		//�������б��
		if(x3!=0&&x1!=0){
			kl=((float)(y3-y1))/(float)(x3-x1);
		}
		else if(x3!=0&&x1==0){
			kl=((float)(y3-(y3-1))/(float)(x3-left_line[y3-1]));
		}
		
		//�����ұ�б��
		if(x4!=0&&x2!=0){
			kr=((float)(y4-y2))/(float)(x4-x2);
		}
		else if(x4!=0&&x2==0){
			kr=((float)(y4-(y4-1))/(float)(x4-left_line[y4-1]));
		}
		if(x3!=0&&x4!=0){
			//���в���
			for(uint8 i=max_uint8(y1,y2);i>mini_uint8(y4,y3);i--){
				
					left_line[i]=x1+(i-y1)/kl;
					right_line[i]=x2+(i-y2)/kr;
					mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		else if(x3==0){
			//���в���
			for(uint8 i=y2;i>y4;i--){
				
					right_line[i]=x2+(i-y2)/kr;
					mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
		}
		else if(x4==0){
			//���в���
			for(uint8 i=y1;i>y3;i--){
				
					left_line[i]=x1+(i-y1)/kl;
					mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
		}
		
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
	1,1,1,1,1,1,1,1,1,1,
	6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,10,10,10,
	20,20,20,20,15,15,15,10,10,10,
	6,6,6,6,6,6,6,6,6,6,
	1,1,1,1,1,1,1,1,1,1,
};

uint8 final_mid_value = MT9V03X_W/2;			//��������ֵ

//��Ȩ������ֵ
uint32 weight_find_mid_value(){
	
	
	uint8 last_mid_line = MT9V03X_W/2;			//��һ�ε�����ֵ
	uint8 mid_v = MT9V03X_W/2;							//��������ֵ
	uint8 now_mid_line = MT9V03X_W/2;				//��������ֵ
	uint32 weight_mid_line_sum=0;							//����Ȩ�غ�
	uint32 weight_sum=0;											//��Ȩ��
	
	for(uint8 i=bottom_line;i>find_end_line-1;i--){
		
		weight_mid_line_sum += mid_line[i] *mid_weight[i];
		weight_sum += mid_weight[i];

		
	}
	
	now_mid_line = (uint8)(weight_mid_line_sum/weight_sum);
	mid_v = now_mid_line*0.9+last_mid_line*0.1;			//�����˲�
	last_mid_line =now_mid_line;
	
	return mid_v;
}


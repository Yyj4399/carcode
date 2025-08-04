#include "zf_common_headfile.h"
#include "image.h"

uint8 basic_image[MT9V03X_H][MT9V03X_W];  					//ͼ���ƺ�Ļ���ͼ��
uint8 image[MT9V03X_H][MT9V03X_W];    							//��ֵ�����ͼ��
uint8 find_end_line=25;															//����Ѱ�ҽ�ֹ��

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
	uint8 num=0;
	uint8 num1=0;
	
	for(uint8 i=0;i<40;i++){			//Զ��ͼ����ֵ+10
		
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
		
		//����һ�л���Ϊ������һ��Ϊ��Ѱ��ֹ��
		if(num>=180){
			
			find_end_line=i;
			num1++;
			
		}

		num=0;
	}
	
	for(uint8 i=40;i<MT9V03X_H;i++){			//���˲���
		
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
		
		//����һ�л���Ϊ������һ��Ϊ��Ѱ��ֹ��
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





#define bottom_line 119			//��������

uint8 l_point;							//�����
uint8 r_point;							//�һ���


//��ͼ����߻���
void find_basic_point(uint8 bio_image[MT9V03X_H][MT9V03X_W]){
	
	//��1/2����ʼ��
	if(bio_image[bottom_line][MT9V03X_W/2]==255&&bio_image[bottom_line][MT9V03X_W/2+1]==255&&bio_image[bottom_line][MT9V03X_W/2-1]==255){
		
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

uint8 abs_subtact_uint8(uint8 a,uint8 b){
	if(a>=b){
		
		return a-b;
		
	}
	
	else{
		
		return b-a;
		
	}
	
	return 0;
	
}





#define l_right_find 20								//���������Ѱ��20������
#define l_left_find 10								//���������Ѱ��10������
#define r_right_find 10								//�ұ�������Ѱ��10������
#define r_left_find 20								//�ұ�������Ѱ��20������

uint8 left_line[MT9V03X_H]={0};				//����ߴ洢
uint8 right_line[MT9V03X_H]={0};			//�ұ��ߴ洢
uint8 mid_line[MT9V03X_H]={0};				//���ߴ洢

uint16 count_beep = 0;								//����������

cross cross_point={0};								//ʮ�ֹյ��ʼ��
cross cross_zero_point={0};						//ʮ�ֹյ�����
circle circle_point={0};							//Բ���յ��ʼ��
circle circle_zero_point={0};					//Բ���յ�����

f_line find_line_flag={0};						//Ѱ�ұ��߱�־λ��ʼ��
f_line find_line_zeroflag={0};				//Ѱ�ұ��߱�־λ����
f_circle circle_stage_flag={0};				//Բ��״̬��־λ
f_circle circle_stage_zeroflag={0};		//Բ��״̬��־λ����

Num loss_num={0};											//��������ʼ��
Num loss_zeronum={0};									//����������

//Ѱ�ұ���
void find_line(uint8 index[MT9V03X_H][MT9V03X_W]){
	
	uint8 l = l_point;									//�м����洢
	uint8 r = r_point;									//�м����洢
	
	//�������Ͽ�ʼѰ�ұ��߽�ֹ����Ѱ��ֹ��
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		//����Ѱ�������
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
		
		//����Ѱ�������
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
		
		//��1/2��Ѱ������ߣ����������⣩
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
		
		//����Ѱ���ұ���
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
		
		//����Ѱ���ұ���
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
		
		//��1/2��Ѱ���ұ���(����������)
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
		
		//�洢�����Լ�����
		left_line[i] = limit_uint8(1,l,MT9V03X_W-2);
		right_line[i] = limit_uint8(1,r,MT9V03X_W-2);
		mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
		
		//������Ϊ��ɫ��ı���Ѱ��ֹ��
		if(image[i][mid_line[i]]==0){
			
			find_end_line = i;
			
		}
		
		//��־λ����
		find_line_flag=find_line_zeroflag;
		
	}
	
	
	
	//ʮ�ֲ���
	for(uint8 i=bottom_line;i>find_end_line;i--){
		
		//�������߶������������ж��Ƿ����ʮ��
		if(left_line[i]<=5&&right_line[i]>=MT9V03X_W-7){
			
			loss_num.num_loss++;
			
		}		
		
		//�������δ�����ұ߶������������ж��Ƿ����Բ��״̬1������֮һ
		if(left_line[i]>=5&&right_line[i]>=MT9V03X_W-7){
			
			loss_num.num_lossr++;
			
		}
		
		//�����ұ�δ������߶�������
		if(left_line[i]<=5&&right_line[i]<=MT9V03X_W-7){
			
			loss_num.num_lossl++;
			
		}
		
	}
	
	//�յ�����
	if(loss_num.num_loss+loss_num.num_lossr+loss_num.num_lossl<=5){
		
		cross_point=cross_zero_point;
		
	}
	
	//�յ�����
	if(loss_num.num_lossr<=3&&loss_num.num_loss<=10){
		
		circle_point=circle_zero_point;
		
	}
	
//	//�ұ߶���������25���жϽ���Բ��״̬
//	if(loss_num.num_lossr>=15&&(loss_num.num_loss<=10||circle_point.x6!=0)){//&&((x5!=0&&y5!=0)||(x7!=0&&y7!=0))){
//		
//		for(uint8 i=bottom_line;i>25;i--){
//			//�ж�Բ���Ƿ�����е������ж��Ƿ����Բ��״̬1������֮һ
//			if(i<70&&i>30&&right_line[i-1]-right_line[i]>=1&&right_line[i+1]-right_line[i]>=0&&right_line[i-2]-right_line[i]>=1&&right_line[i+2]-right_line[i]>=0){
//				if(circle_point.x6==0&&left_line[i+15]>=5&&right_line[i+15]>=MT9V03X_W-7){
//					circle_point.x6=right_line[i];
//					circle_point.y6=i;
//					//����������
//					count_beep=500;
//				}

//				else if(circle_point.x6!=0){
//					circle_point.x6=right_line[i];
//					circle_point.y6=i;
//				}
//				
//			}
//			//������
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
//				//�ж��ұ��Ƿ���ֹյ㲢�ҹյ��Ϸ��ұ߶������δ���������ж��Ƿ����Բ��״̬1������֮һ
//				if(i>80&&left_line[i-3]>=5&&right_line[i-3]>=MT9V03X_W-7&&left_line[i-4]>=5&&right_line[i-4]>=MT9V03X_W-7&&right_line[i+1]-right_line[i]>=1&&right_line[i-1]-right_line[i]>=1){
//					
//					circle_point.x5=right_line[i];
//					circle_point.y5=i;
//			
//				}

//				//�ж����Ϸ��Ƿ���ֹյ㲢�ҹյ��·��ұ߶������δ���������ж��Ƿ����Բ��״̬3������֮һ
//				if(i<circle_point.y6&&i>40&&left_line[i+3]>=5&&right_line[i+3]>=MT9V03X_W-7&&left_line[i+4]>=5&&right_line[i+4]>=MT9V03X_W-7&&right_line[i+3]-right_line[i]>=20&&right_line[i]-right_line[i-1]<=3){
//					
//					circle_point.x7=right_line[i];
//					circle_point.y7=i;
//				
//				}
//				
//				//�ж����·��Ƿ���ֹյ���Ϊ�Ƿ����Բ��״̬5������֮һ
//				if(i>60&&left_line[i]-left_line[i+1]>=1&&left_line[i]-left_line[i-1]>=1&&left_line[i]>=3&&left_line[i]>=10){
//					
//					circle_point.x9=left_line[i];
//					circle_point.y9=i;
//					
//				}
//				
//				//����ұ߳��ֹյ㲢��Բ�����е㲢�����Ϸ�û�йյ������״̬1
//				if(circle_point.x5!=0&&circle_point.x7==0&&circle_point.y6<=65){
//					
//					//Բ��״̬1��־λ��1
//					circle_stage_flag.circle1_flag=1;
//					
//					//����б��
//					circle_point.k1=((float)(circle_point.y6-circle_point.y5))/(float)(circle_point.x6-circle_point.x5);
//					
//				}
//				//���Ϸ����ֹյ������״̬3
//				else if(circle_point.x7!=0&&circle_point.x9==0&&left_line[40]>=5){
//					
//					//Բ��״̬3��־λ��1
//					circle_stage_flag.circle3_flag=1;
//					
//					//����ߵ׽Ǳ߽����Ϊ������ʼ��
//					circle_point.x8=left_line[90];
//					circle_point.y8=90;
//					
//					//����б��
//					circle_point.k3=((float)(circle_point.y7-circle_point.y8))/(float)(circle_point.x7-circle_point.x8);
//					
//				}
//				else if(circle_point.x9!=0&&left_line[35]<=MT9V03X_W-7){
//					
//					//Բ��״̬5��־λ��1
//					circle_stage_flag.circle5_flag=1;
//					
//					if(circle_point.k5==0){
//						
//						//����б��
//						circle_point.k5=((float)(circle_point.y9-(circle_point.y9+2)))/(float)(circle_point.x9-left_line[circle_point.y9+2]);
//						
//					}
//					
//				}
//				else if(left_line[35]>MT9V03X_W-7){
//					
//					//Բ��״̬8��־λ��1
//					circle_stage_flag.circle8_flag=1;
//					
//					circle_point.x7=0;
//					circle_point.y7=0;
//					
//				}
//						
//				//״̬1���в���
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
//				//״̬3���в���
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
//				//״̬5���в���
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
//				//״̬8���в���
//				else if(circle_stage_flag.circle8_flag==1&&circle_point.x7!=0){
//					//����б��
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
//				//��������
//				loss_num.num_lossr=0;
//				
//				//Բ����־λ����
//				circle_stage_flag=circle_stage_zeroflag;
//		
//			}

//		}
//		
//	}
	
	//�ж϶�������������15���жϽ���ʮ��
	if(circle_point.x6==0&&loss_num.num_loss>10){
		
		for(uint8 i=110;i>80;i--){
			
			//��¼���¹յ�
			if(left_line[i]-left_line[i+1]>=1&&left_line[i]-left_line[i-1]>=1&&left_line[i]>=3&&left_line[i]>=10){
				
				cross_point.x1=left_line[i];
				cross_point.y1=i;
				
				if(cross_point.x1>=94){
					
					cross_point.x1=0;
					cross_point.y1=0;
					
				}
				
			}

			//��¼���¹յ�
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
			
			//��¼���Ϲյ�
			if(left_line[i]-left_line[i+4]>=10&&left_line[i]>20){
				
				cross_point.x3=left_line[i];
				cross_point.y3=i;
				
				if(cross_point.x3>=94){
					
					cross_point.x3=0;
					cross_point.y3=0;
					
				}
				
				
			}
			 
			//��¼���Ϲյ�
			else if(right_line[i+3]-right_line[i]>=15&&right_line[i]-right_line[i-2]<=5&&right_line[i]<MT9V03X_W-5){
				
				cross_point.x4=right_line[i];
				cross_point.y4=i;
				
				if(cross_point.x4<=94){
					
					cross_point.x4=0;
					cross_point.y4=0;
					
				}
				
			}
			
		}
		
		//ʮ�ֲ���״̬���
		if(cross_point.y3>=100||cross_point.y4>=100){
			
			cross_point=cross_zero_point;
			
		}
		
		//��������
		loss_num=loss_zeronum;
		
		//�������б��
		if(cross_point.x3!=0&&cross_point.x1!=0){
			
			cross_point.kl=((float)(cross_point.y3-cross_point.y1))/(float)(cross_point.x3-cross_point.x1);
			
		}
		else if((cross_point.x3!=0&&cross_point.x1==0)||(cross_point.x3==0&&cross_point.x1!=0)||cross_point.kl>0){
			
			cross_point.kl=-1.5;
			
		}
		
		//�����ұ�б��
		if(cross_point.x4!=0&&cross_point.x2!=0){
			
			cross_point.kr=((float)(cross_point.y4-cross_point.y2))/(float)(cross_point.x4-cross_point.x2);
			
		}
		else if((cross_point.x4!=0&&cross_point.x2==0)||(cross_point.x4==0&&cross_point.x2!=0)||cross_point.kr<0){
			
			cross_point.kr=1.8;
			
		}
		
		//����յ㶼����
		if(cross_point.x1!=0&&cross_point.x3!=0&&cross_point.kl<0){
			
			//���в���
			for(uint8 i=cross_point.y1;i>cross_point.y3;i--){
				
				left_line[i]=cross_point.x1+(i-cross_point.y1)/cross_point.kl;
				mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
		//�����¹յ㲻����
		else if(cross_point.x1==0&&cross_point.x3!=0&&cross_point.kl<0){
			
			//���в���
			for(uint8 i=cross_point.y3;i<bottom_line;i++){
				
				left_line[i]=cross_point.x3+(i-cross_point.y3)/cross_point.kl;
				mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
		//�����Ϲյ㲻����
		else if(cross_point.x1!=0&&cross_point.x3==0&&cross_point.kl<0){
			
			//���в���
			for(uint8 i=cross_point.y1;i>40;i--){
				
				left_line[i]=cross_point.x1+(i-cross_point.y1)/cross_point.kl;
				mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
		//���ҹյ㶼����
		if(cross_point.x2!=0&&cross_point.x4!=0&&cross_point.kr>0){
			
			//���в���
			for(uint8 i=cross_point.y2;i>cross_point.y4;i--){
				
					right_line[i]=cross_point.x2+(i-cross_point.y2)/cross_point.kr;
					mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
		//�����¹յ㲻����
		else if(cross_point.x2==0&&cross_point.x4!=0&&cross_point.kr>0){
			
			//���в���
			for(uint8 i=cross_point.y4;i<bottom_line;i++){
				
					right_line[i]=cross_point.x4+(i-cross_point.y4)/cross_point.kr;
					mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
		//�����Ϲյ㲻����
		else if(cross_point.x2!=0&&cross_point.x4==0&&cross_point.kr>0){
			
			//���в���
			for(uint8 i=cross_point.y2;i>40;i--){
				
					right_line[i]=cross_point.x2+(i-cross_point.y2)/cross_point.kr;
					mid_line[i] = limit_uint8(1,(left_line[i]+right_line[i])/2,MT9V03X_W-2);
							
			}
			
		}
		
	}
	
	//��������
	loss_num=loss_zeronum;
	
}







//����Ȩ��
//Ȩ��Խ����ת��Խ��
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

uint8 final_mid_value = MT9V03X_W/2;			//��������ֵ

//��Ȩ������ֵ
uint8 weight_find_mid_value(){
	
	uint8 last_mid_line = MT9V03X_W/2;				//��һ�ε�����ֵ
	uint8 mid_v = MT9V03X_W/2;								//��������ֵ
	uint8 now_mid_line = MT9V03X_W/2;					//��������ֵ
	
	uint32 weight_mid_line_sum = 0;						//����Ȩ�غ�
	uint32 weight_sum = 0;										//��Ȩ��
	
	//��������Ȩ�غ��Լ���Ȩ��
	for(uint8 i=bottom_line;i>find_end_line-1;i--){
		
		weight_mid_line_sum += mid_line[i] *mid_weight[i];
		weight_sum += mid_weight[i];

		
	}
	
	//��ֹ��ֵͻ��
	if(now_mid_line-last_mid_line>=25||last_mid_line-now_mid_line>=25){
		
		now_mid_line = last_mid_line;
		
	}
	
	//������ֵ
	now_mid_line = (uint8)(weight_mid_line_sum/weight_sum);
	
	//�����˲�
	mid_v = now_mid_line*0.9+last_mid_line*0.1;		
	
	//������һ����ֵ
	last_mid_line = now_mid_line;
	
	return mid_v;
	
}


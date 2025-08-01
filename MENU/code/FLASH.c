#include "zf_common_headfile.h"
#include "flash.h"
#include "motor.h"

//��������
void save_pid(void){
	
	//���flash�����Ƿ�������
	if(flash_check(flash_sector,flash_page)){
		
		//�������
		flash_erase_page(flash_sector,flash_page);
		
	}
	
	//���������
	flash_buffer_clear();
	
	//д�뻺����
	flash_union_buffer[0].float_type=p;
	flash_union_buffer[1].float_type=d;
	
	flash_union_buffer[2].int32_type=speed;
	
	//����������
	flash_write_page_from_buffer(flash_sector,flash_page);
	
}

//ȡ������
void read_pid(void){
	
	//��ȡ���ݵ�������
	flash_read_page_to_buffer(flash_sector,flash_page);
	
	
	//ȡ������
	p=flash_union_buffer[0].float_type;
	d=flash_union_buffer[1].float_type;
	
	speed=flash_union_buffer[2].int32_type;
	
	//���������
	flash_buffer_clear();

}




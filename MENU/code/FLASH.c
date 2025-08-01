#include "zf_common_headfile.h"
#include "flash.h"
#include "motor.h"

//保存数据
void save_pid(void){
	
	//检查flash区域是否有数据
	if(flash_check(flash_sector,flash_page)){
		
		//清除数据
		flash_erase_page(flash_sector,flash_page);
		
	}
	
	//清除缓存区
	flash_buffer_clear();
	
	//写入缓存区
	flash_union_buffer[0].float_type=p;
	flash_union_buffer[1].float_type=d;
	
	flash_union_buffer[2].int32_type=speed;
	
	//缓存区保存
	flash_write_page_from_buffer(flash_sector,flash_page);
	
}

//取出数据
void read_pid(void){
	
	//读取数据到缓存区
	flash_read_page_to_buffer(flash_sector,flash_page);
	
	
	//取出数据
	p=flash_union_buffer[0].float_type;
	d=flash_union_buffer[1].float_type;
	
	speed=flash_union_buffer[2].int32_type;
	
	//清除缓存区
	flash_buffer_clear();

}




#include "flash.h"

FlashInformation flash;
//初始化flash
//使用方法：如果修改了参数，那么进入到具体屏幕按一下确认更新数据，如果未修改，直接用
void FlashInit()
{
    flash_init();    
    FlashWrite();	
    FlashRead();
	FlashParamInit();
}
void FlashParamInit()
{
	flash.status = 0;
}

void FlashWrite()
{
    //pid
    flash.temp_write[0][0] = float_conversion_uint32(pid.leftMotor.kp); 
    flash.temp_write[0][1] = float_conversion_uint32(pid.leftMotor.ki);        
    flash.temp_write[0][2] = float_conversion_uint32(pid.leftMotor.kd);
    
    flash.temp_write[1][0] = float_conversion_uint32(pid.rightMotor.kp);
    flash.temp_write[1][1] = float_conversion_uint32(pid.rightMotor.ki);  
    flash.temp_write[1][2] = float_conversion_uint32(pid.rightMotor.kd);
    
    flash.temp_write[2][0] = float_conversion_uint32(pid.servo.cam.normal.kp1);  
    flash.temp_write[2][1] = float_conversion_uint32(pid.servo.cam.normal.kp2);
    flash.temp_write[2][2] = float_conversion_uint32(pid.servo.cam.normal.kd);
    
    flash.temp_write[3][0] = motor.expectSpeed.normal;
    flash.temp_write[3][1] = motor.left.differential;
    flash.temp_write[3][2] = motor.right.differential;
    
    flash.temp_write[4][0] = task[servoTask].isOpen;
    flash.temp_write[4][1] = debug.stopProtect;
	flash.temp_write[4][2] = servo.nowMid;
	
	flash.temp_write[5][0] = task[ANODTTask].isOpen;
	flash.temp_write[5][1] = motor.expectSpeed.circle;
    
    if(flash_check(EXAMPLE_FLASH_SECTOR,EXAMPLE_FLASH_SECTOR_PAGE))//校验当前 扇区所在页是否有数据，如果有数据则擦除整个扇区
    {
        flash.status = flash_erase_sector(EXAMPLE_FLASH_SECTOR);//擦除扇区，如果扇区已经有数据则必须擦除扇区之后才能再次写入新的数据
        if(flash.status)  while(1);//擦除失败
    }
    
    flash.write_buf[0]  = flash.temp_write[0][0];
    flash.write_buf[1]  = flash.temp_write[0][1];
    flash.write_buf[2]  = flash.temp_write[0][2];
    flash.write_buf[3]  = flash.temp_write[1][0];
    flash.write_buf[4]  = flash.temp_write[1][1];
    flash.write_buf[5]  = flash.temp_write[1][2];
    flash.write_buf[6]  = flash.temp_write[2][0];
    flash.write_buf[7]  = flash.temp_write[2][1];
    flash.write_buf[8]  = flash.temp_write[2][2];
    flash.write_buf[9]  = flash.temp_write[3][0];
    flash.write_buf[10] = flash.temp_write[3][1];
    flash.write_buf[11] = flash.temp_write[3][2];
    flash.write_buf[12] = flash.temp_write[4][0];
    flash.write_buf[13] = flash.temp_write[4][1];
    flash.write_buf[14] = flash.temp_write[4][2];
    flash.write_buf[15] = flash.temp_write[5][0];
	flash.write_buf[16] = flash.temp_write[5][1];
     
    flash.status = flash_page_program(EXAMPLE_FLASH_SECTOR, EXAMPLE_FLASH_SECTOR_PAGE, flash.write_buf, FLASH_SAVE_NUM);
    if(flash.status)  while(1);//写入失败
    
}



void FlashRead()
{
    flash_read_page(EXAMPLE_FLASH_SECTOR, EXAMPLE_FLASH_SECTOR_PAGE, flash.read_buf, FLASH_SAVE_NUM);
    
	pid.leftMotor.kp = uint32_conversion_float(flash.read_buf[0]);
	pid.leftMotor.ki = uint32_conversion_float(flash.read_buf[1]);
	pid.leftMotor.kd = uint32_conversion_float(flash.read_buf[2]);
	
    pid.servo.cam.normal.kp1 = uint32_conversion_float(flash.read_buf[6]);
    pid.servo.cam.normal.kp2 = uint32_conversion_float(flash.read_buf[7]);
    pid.servo.cam.normal.kd  = uint32_conversion_float(flash.read_buf[8]);
	
    motor.expectSpeed.normal = flash.read_buf[9];
    motor.left.differential  = flash.read_buf[10];
    motor.right.differential = flash.read_buf[11];
	
    task[servoTask].isOpen   = flash.read_buf[12];
    debug.stopProtect        = flash.read_buf[13];
    servo.nowMid             = flash.read_buf[14];
	
	task[ANODTTask].isOpen =  flash.read_buf[15];
	motor.expectSpeed.circle = flash.read_buf[16];
    
}
/*
 * @Editor      : zyg
 * @Date        : Do not edit
 * @Version     : MDK5.32
 * @LastEditTime: 2020-12-23 16:03:17
 * @Description : 匿名上位机操作文件
 * @Attention   : 注意接收部分需要按照需要进行移植
 */

#include "ANO_DT.h"
#include "isr.h"

short  waveform[20];         //波形缓存
unsigned char data_to_receive[100]; //接收的缓存
unsigned char data_to_send[100];	//发送数据缓存
#define   WAVEFORM_COUNT   2        //有几路波形 会发送前x路波形      

pid_param_type pid_param[25];

unsigned char WAVEFORM_DISABLE = 0;

/**
 * @editior: zyg
 * @brief  : none
 * @param  : 输出数组地址
 * @param  : 输出数组长度
 * @return : NULL
 * @example: ANO_DT_Send_Data(my_data , sizerof(my_data))
 */
void ANO_DT_Send_Data(unsigned  char *_dataToSend , unsigned int length)
{
    //采用串口的发送方式
    seekfree_wireless_send_buff(_dataToSend, length);
}


/**
 * @editior: zyg
 * @brief  : 进行波形数据的写入
 * @param  : NULL
 * @return : NULL
 * @example: none
 */
void Waveform_Write(void)
{
    waveform[USERDATA_1]   = (short)Speed_Min;
    waveform[USERDATA_2]   = (short)IMC_Treated.gyro.y;
    waveform[USERDATA_3]   = 0;
    waveform[USERDATA_4]   = 0;
    waveform[USERDATA_5]   = 0;
    waveform[USERDATA_6]   = 0;
    waveform[USERDATA_7]   = 0;
    waveform[USERDATA_8]   = 0;
    waveform[USERDATA_9]   = 0;
    waveform[USERDATA_10]  = 0;
    waveform[USERDATA_11]  = 0;
    waveform[USERDATA_12]  = 0;
    waveform[USERDATA_13]  = 0;
    waveform[USERDATA_14]  = 0;
    waveform[USERDATA_15]  = 0;
    waveform[USERDATA_16]  = 0;
    waveform[USERDATA_17]  = 0;
    waveform[USERDATA_18]  = 0;
    waveform[USERDATA_19]  = 0;
    waveform[USERDATA_20]  = 0;
    //send
    Send_Waveform();
}


/**
 * @editior: zyg
 * @brief  : 调用此函数,则向上位机发送20路波形,每一路波形均为short型变量
 * @param  : none
 * @return : NULL
 * @example: Send_Waveform();
 */
void Send_Waveform(void)
{
    unsigned char _cnt = 0;
    data_to_send[_cnt++] = 0xAA;  //头字节
    data_to_send[_cnt++] = MYHWADDR; //器件地址
    data_to_send[_cnt++] = SWJADDR; //上位机地址
    data_to_send[_cnt++] = 0xF1;  //数据波形发送指令,只发送F1这一帧
    data_to_send[_cnt++] = 0;     //有效数据字节数
    //发送数据波形数组
    for(unsigned char i = 0; i < WAVEFORM_COUNT; i ++)
    {
        // data_to_send[_cnt++]=BYTE3(waveform[i]);
        // data_to_send[_cnt++]=BYTE2(waveform[i]);
        data_to_send[_cnt++] = BYTE1(waveform[i]);
        data_to_send[_cnt++] = BYTE0(waveform[i]);
    }
    data_to_send[4] = _cnt - 5; //有效数据字节
    unsigned char sum = 0;//和校验
    for(unsigned char i = 0; i < _cnt; i++)
        sum += data_to_send[i];
    data_to_send[_cnt++] = sum;
    //串口发送数据
    ANO_DT_Send_Data(data_to_send, _cnt);
}


/**
 * @editior: zyg
 * @brief  : 每次点击读取飞控时此函数被自动调用
 * @param  : NULL
 * @return : NULL
 * @example: Read_Params_of_MCU()
 * @attention: 在此函数中加入需要读取的变量
 */
void Read_Params_of_MCU(void)
{
    //第一组PID 一共可以有18组可供调用
    //    pid_param[PID1].P = Ang_Vel[0]*1000;
    //    pid_param[PID1].I = Ang_Vel[2]*1000;
    //    pid_param[PID1].D = Ang_Vel[1]*1000;
    //	pid_param[PID2].P = Angle[0]*1000;
    //    pid_param[PID2].I = Angle[2]*1000;
    //    pid_param[PID2].D = Angle[3]*1000;
}


/**
 * @editior: zyg
 * @brief  : 每次修改读取飞控时此函数被调用
 * @param  : NULL
 * @return : NULL
 * @example: none
 */
void Write_Params_of_MCU(void)
{
    //第一组PID 一共可以有18组可供调用
    //	Ang_Vel[0] = pid_param[PID1].P;
    //    Ang_Vel[2] = pid_param[PID1].I;
    //    Ang_Vel[1] = pid_param[PID1].D;
    //	Ang_Vel[0] /= 1000;
    //	Ang_Vel[1] /= 1000;
    //	Ang_Vel[2] /= 1000;
    //	Angle[0] = pid_param[PID2].P;
    //    Angle[2] = pid_param[PID2].I;
    //    Angle[1] = pid_param[PID2].D;
    //	Angle[0] /= 1000;
    //	Angle[1] /= 1000;
    //	Angle[2] /= 1000;

}


/**
 * @editior: zyg
 * @brief  : 上位机参数读取命令解析
 * @param  : 上位机数据存储地址
 * @return : NULL
 * @example: none
 * @attention:简易的使用上位机就两种命令,读取所有的参数 修改单项参数
 */
static unsigned char ano_fun, ano_fun_1, ano_read_val_num, ano_write_val_num;
unsigned char cmd_buff[17];
static unsigned int w_value;
void ANO_Cmd_Reslove(unsigned char *p)
{
    static unsigned char write_count = 0;
    //把收到的数据打印出去
    //PRINTF("%s",*p);
    //判断起始帧
    if(*p == 0xAA)
    {
        write_count = 0;//解算标志清零,重新解算
    }
    cmd_buff[write_count] = *p;//整个指令数组
    write_count ++;
    if(write_count >= 3)
    {
        ano_fun = cmd_buff[3];//功能字
        if(ano_fun == 0xE0)   //参数读取
        {
            if(write_count >= 16)
            {
                write_count = 0;
                ano_fun = cmd_buff[3];//功能字
                ano_fun_1 = cmd_buff[5];//命令字
                ano_read_val_num = cmd_buff[6] << 8 | cmd_buff[7]; //cmd1
            }
        }
        if(ano_fun == 0xE1)
        {
            if(write_count >= 12)
            {
                write_count = 0;
                ano_fun = cmd_buff[3];//功能字
                ano_write_val_num = cmd_buff[5] << 8 | cmd_buff[6]; //parameter
                w_value = cmd_buff[7] << 24 | cmd_buff[8] << 16 |  cmd_buff[9] << 8 | cmd_buff[10];
            }
        }
        WAVEFORM_DISABLE = 1;//失能波形显示
    }
    if(((ano_fun == 0xE0) && (ano_fun_1 == 0xE1)) && write_count == 0)
    {
        if(ano_read_val_num == 0x00)//读取参数起始命令
        {
            unsigned char _buff[17] = {0xAA, 0x05, 0xAF, 0xE1, 0x0B, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x4A};
            ANO_DT_Send_Data(_buff, 17);
        }
        else//读取每个参数
        {
            unsigned char _buff[17] = {0xAA, 0x05, 0xAF, 0xE1, 0x0B, 0, ano_read_val_num, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            _buff[5] = (ano_read_val_num & 0xff00) >> 8;
            _buff[6] = ano_read_val_num;
            Read_Params_of_MCU();//读取MUC的参数
            if(ano_read_val_num % 3 == 0)
            {
                _buff[7] = (pid_param[ano_read_val_num / 3 - 1].D & 0xFF000000) >> 24;
                _buff[8] = (pid_param[ano_read_val_num / 3 - 1].D & 0xFF0000) >> 16;
                _buff[9] = (pid_param[ano_read_val_num / 3 - 1].D & 0xFF00) >> 8;
                _buff[10] = pid_param[ano_read_val_num / 3 - 1].D & 0xFF;
                //				_buff[7] = (10 & 0xFF000000)>>24;
                //                _buff[8] = (10 & 0xFF0000)>>16;
                //                _buff[9] = (10 & 0xFF00)>>8;
                //                _buff[10] = 10 & 0xFF;
            }
            else if(ano_read_val_num % 3 == 1)
            {
                _buff[7] = (pid_param[ano_read_val_num / 3].P & 0xFF000000) >> 24;
                _buff[8] = (pid_param[ano_read_val_num / 3].P & 0xFF0000) >> 16;
                _buff[9] = (pid_param[ano_read_val_num / 3].P & 0xFF00) >> 8;
                _buff[10] = pid_param[ano_read_val_num / 3].P & 0xFF;
            }
            else if(ano_read_val_num % 3 == 2)
            {
                _buff[7] = (pid_param[ano_read_val_num / 3].I & 0xFF000000) >> 24;
                _buff[8] = (pid_param[ano_read_val_num / 3].I & 0xFF0000) >> 16;
                _buff[9] = (pid_param[ano_read_val_num / 3].I & 0xFF00) >> 8;
                _buff[10] = pid_param[ano_read_val_num / 3].I & 0xFF;
            }
            for(unsigned char j = 0; j < 16; j ++)
            {
                _buff[16] += _buff[j];
            }
            ANO_DT_Send_Data(_buff, 17);
            if(ano_read_val_num >= 0x49) WAVEFORM_DISABLE = 0;
        }
    }
    if((ano_fun == 0xE1) && write_count == 0) //参数修改命令
    {
        unsigned char __buff[12] = {0xAA, 0x05, 0xAF, 0xE1, 0x06, 0, 0, 0, 0, 0, 0, 0};
        __buff[5] = (ano_write_val_num & 0xff00) >> 8;
        __buff[6] = ano_write_val_num;
        __buff[7] = (w_value & 0xFF000000) >> 24;
        __buff[8] = (w_value & 0xFF0000) >> 16;
        __buff[9] = (w_value & 0xFF00) >> 8;
        __buff[10] = w_value & 0xFF;
        if(ano_write_val_num % 3 == 0)  pid_param[ano_write_val_num / 3 - 1].D = w_value; //修改MCU参数
        if(ano_write_val_num % 3 == 1)  pid_param[ano_write_val_num / 3 ].P = w_value;
        if(ano_write_val_num % 3 == 2)  pid_param[ano_write_val_num / 3 ].I = w_value;
        Write_Params_of_MCU();
        for(unsigned char j = 0; j < 11; j ++)
        {
            __buff[11] += __buff[j];
        }
        ANO_DT_Send_Data(__buff, 12);
        if(ano_write_val_num >= 0x49) WAVEFORM_DISABLE = 0;
    }
    //清除数据
    ano_fun = 0;
    ano_fun_1 = 0;
    ano_read_val_num = 0;
}



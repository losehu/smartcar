/*
 * @Editor      : zyg
 * @Date        : Do not edit
 * @Version     : MDK5.32
 * @LastEditTime: 2020-12-23 16:03:17
 * @Description : 匿名上位机操作文件
 * @Attention   : 注意接收部分按照需要进行移植
 */

#include "ANO_DT.h"

//需要设置
#define   WAVEFORM_COUNT   20        //有几路波形 会发送前x路波形  
extern int angle_out;

short  waveform[20];         //波形缓存
unsigned char data_to_receive[100]; //接收的缓存
unsigned char data_to_send[100];    //发送数据缓存
unsigned char WAVEFORM_ENABLE = 1;


pid_param_type pid_param[25];


//uart3 gpio init
void UART3_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOC, ENABLE);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_7);

    //UART3_TX   GPIOC.10
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //UART3_RX    GPIOC.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;


    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

//uart3  init
void UART3_RX_Init(u32 baudrate)
{
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_UART3, ENABLE);

    //UART3 NVIC
    NVIC_InitStructure.NVIC_IRQChannel = UART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //Baud rate
    UART_StructInit(&UART_InitStructure);
    UART_InitStructure.BaudRate = baudrate;
    //The word length is in 8-bit data format.
    UART_InitStructure.WordLength = UART_WordLength_8b;
    UART_InitStructure.StopBits = UART_StopBits_1;
    //No even check bit.
    UART_InitStructure.Parity = UART_Parity_No;
    //No hardware data flow control.
    UART_InitStructure.HWFlowControl = UART_HWFlowControl_None;
    UART_InitStructure.Mode = UART_Mode_Rx | UART_Mode_Tx;

    UART_Init(UART3, &UART_InitStructure);
    UART_ITConfig(UART3, UART_IT_RXIEN, ENABLE);
    UART_Cmd(UART3, ENABLE);

    UART3_GPIO_Init();
}


//DMA config
void DMA_NVIC_Send_Config(DMA_Channel_TypeDef *dam_chx, u32 target_addr, u32 data_addr, u16 length)
{
    DMA_InitTypeDef DMA_InitStructure;

    //-----------------dma init--------------------//
    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(dam_chx);
    DMA_StructInit(&DMA_InitStructure);
    //DMA transfer peripheral address
    DMA_InitStructure.DMA_PeripheralBaseAddr = target_addr;
    //DMA transfer memory address
    DMA_InitStructure.DMA_MemoryBaseAddr = data_addr;
    //DMA transfer direction from peripheral to memory
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    //DMA cache size
    DMA_InitStructure.DMA_BufferSize = length;
    //After receiving the data, the peripheral address is forbidden to move
    //backward
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //After receiving the data, the memory address is shifted backward
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //Define the peripheral data width to 8 bits
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//可设置回环模式
    //DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    //M2M mode is disabled
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Auto_reload = DMA_Auto_Reload_Disable;
    DMA_Init(dam_chx, &DMA_InitStructure);

    // Enable UARTy_DMA1_Channel Transfer complete interrupt
    DMA_ITConfig(dam_chx, DMA_IT_TC, ENABLE);
    //uart1 dma enable
    UART_DMACmd(UART3, UART_GCR_DMA, ENABLE);
    // UARTy_DMA1_Channel enable
    DMA_Cmd(dam_chx, ENABLE);
}


//设置中断优先级
void NVIC_Configure(u8 ch, u8 pri, u8 sub)
{
    exNVIC_Init_TypeDef  NVIC_InitStruct;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStruct.NVIC_IRQChannel = ch;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = pri;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = sub;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

    exNVIC_Init(&NVIC_InitStruct);
}


/**
 * @editior: zyg
 * @brief  : none
 * @param  : none
 * @return : NULL
 * @example: void ANO_DT_Init(void)
 */
void ANO_DT_Init(void)
{
    //txdma rxinterrupt
    UART3_RX_Init(ANO_UART_BAUD);
    NVIC_Configure(ANO_UART_DMA_IRQn, 2, 0);
    DMA_Cmd(ANO_UART_DMA_CHANNEL, DISABLE);
    DMA_NVIC_Send_Config(ANO_UART_DMA_CHANNEL, (u32)&UART3->TDR, (u32)&data_to_send[0], 1);
}


/**
 * @editior: zyg
 * @brief  : none
 * @param  : 输出数组地址
 * @param  : 输出数组长度
 * @return : NULL
 * @example: ANO_DT_Send_Data(my_data , sizerof(my_data))
 */
void ANO_DT_Send_Data(unsigned  char *_dataToSend, unsigned int length)
{
    //采用DMA的发送方式
    DMA_NVIC_Send_Config(ANO_UART_DMA_CHANNEL, (u32)&UART3->TDR, (u32)&_dataToSend, length);
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

    extern int L_SingleControlOut, R_SingleControlOut;

    waveform[USERDATA_1]   = (short)    ICM_State.gyro.y - angle_out;
    waveform[USERDATA_2]   = (short)L_SingleControlOut;
    waveform[USERDATA_3]   = (short)R_SingleControlOut;
    waveform[USERDATA_4]   = (short)Wheel.now;
    waveform[USERDATA_5]   = (short)    R_Wheel.now;
    waveform[USERDATA_6]   = (short) ICM_Treated.gyro.z;
    /* waveform[USERDATA_1]   = ELC_Center_14;
    if(CarInfo.ControlMode == ELCMODE && (Ramp.FindFlag == 'T' || Ramp.RecoverFlag == 'T'))
     waveform[USERDATA_2]   = 20;//Gyro_Z_out;
    else if(CarInfo.ControlMode == CAMMODE)
     waveform[USERDATA_2]   = 40;//Gyro_Z_out;
    else if(CarInfo.ControlMode == ELCMODE && Barn.FindFlag == 'T')
     waveform[USERDATA_2]   = 60;
    else if(CarInfo.ControlMode == ELCMODE && Cross.FindFlag == 'T')
     waveform[USERDATA_2]   = 80;
    else if(CarInfo.ControlMode == ELCMODE && Circle.FindFlag == 'T')
     waveform[USERDATA_2]   = 100;
    else if(CarInfo.ControlMode == ELCMODE )
     waveform[USERDATA_2]   = 120;
     waveform[USERDATA_3]   = (int16)ICM_Treated.gyro.z;
     waveform[USERDATA_4]   = (int16)(ICM_Treated.accdata.x*100);
     waveform[USERDATA_5]   = (int16)(ICM_Treated.accdata.y*100);
     waveform[USERDATA_6]   =  Circle.LeftUpInflexion.x;
     waveform[USERDATA_7]   = Circle.LeftUpInflexion.y;
     waveform[USERDATA_8]   = point_center;
     if(Circle.FindFlag == 'T' && Circle.state == CircleInStraight)
     waveform[USERDATA_9]   = -20;
    else  if(Circle.FindFlag == 'T' && Circle.state == CircleCenter)
     waveform[USERDATA_9]   = -40;
    else if(Circle.FindFlag == 'T' && Circle.state == CircleCouldInTurn)
     waveform[USERDATA_9]   = -60;
    else if(Circle.FindFlag == 'T' && Circle.state == CircleInTurn)
     waveform[USERDATA_9]   = -80;
    else
     waveform[USERDATA_9]   = 0;
     waveform[USERDATA_10]  = ELC_Center_14;
     waveform[USERDATA_11]  = ELC_Sum_14;
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
     */
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
    if (WAVEFORM_ENABLE == 1)
    {
        data_to_send[_cnt++] = 0xAA;  //头字节
        data_to_send[_cnt++] = 0xAA;  //头字节
        //      data_to_send[_cnt++]=MYHWADDR;//器件地址
        //      data_to_send[_cnt++]=SWJADDR; //上位机地址
        data_to_send[_cnt++] = 0xF1;  //数据波形发送指令,只发送F1这一帧
        data_to_send[_cnt++] = 40;     //有效数据字节数
        //发送数据波形数组
        for (unsigned char i = 0; i < WAVEFORM_COUNT; i ++)
        {
            // data_to_send[_cnt++]=BYTE3(waveform[i]);
            // data_to_send[_cnt++]=BYTE2(waveform[i]);
            data_to_send[_cnt++] = BYTE1(waveform[i]);
            data_to_send[_cnt++] = BYTE0(waveform[i]);
        }
        //      data_to_send[4] = _cnt-5;//有效数据字节
        unsigned char sum = 0;//和校验
        for (unsigned char i = 0; i < _cnt; i++)
            sum += data_to_send[i];
        data_to_send[_cnt++] = sum;
        //串口发送数据 ///////////////////写在这里
        DMA_NVIC_Send_Config(ANO_UART_DMA_CHANNEL, (u32)&UART3->TDR, (u32)&data_to_send, _cnt);
    }
}

/**
 * @editior: zyg
 * @brief  : 每次单击读取飞控时此函数被自动调用
 * @param  : NULL
 * @return : NULL
 * @example: Read_Params_of_MCU()
 * @attention: 在此函数中加入需要读取的变量
 */
void Read_Params_of_MCU(void)
{
    //第一组PID 一共可以有18组可供调用
    pid_param[PID1].P = 10;
    pid_param[PID1].I = 11;
    pid_param[PID1].D = 12;
    pid_param[PID2].P = 22;
    pid_param[PID2].I = 44;
    pid_param[PID2].D = 66;
}


/**
 * @editior: zyg
 * @brief  : 每次单击写入飞控时此函数被调用
 * @param  : NULL
 * @return : NULL
 * @example: none
 */
void Write_Params_of_MCU(void)
{
    //第一组PID 一共可以有18组可供调用

}


/**
 * @editior: zyg
 * @brief  : 上位机参数读取命令解析
 * @param  : 上位机数据存储地址    unsigned char型变量
 * @return : NULL
 * @example: none
 * @attention:此处需要移植到对应串口的接收中断函数中
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
    if (*p == 0xAA)
    {
        write_count = 0;//解算标志清零,重新解算
    }
    cmd_buff[write_count] = *p;//整个指令数组
    write_count ++;
    if (write_count >= 3)
    {
        ano_fun = cmd_buff[3];//功能字
        if (ano_fun == 0xE0)  //参数读取
        {
            if (write_count >= 16)
            {
                write_count = 0;
                ano_fun = cmd_buff[3];//功能字
                ano_fun_1 = cmd_buff[5];//命令字
                ano_read_val_num = cmd_buff[6] << 8 | cmd_buff[7]; //cmd1
            }
        }
        if (ano_fun == 0xE1)
        {
            if (write_count >= 12)
            {
                write_count = 0;
                ano_fun = cmd_buff[3];//功能字
                ano_write_val_num = cmd_buff[5] << 8 | cmd_buff[6]; //parameter
                w_value = cmd_buff[7] << 24 | cmd_buff[8] << 16 |  cmd_buff[9] << 8 | cmd_buff[10];
            }
        }
        //读取正常 关闭波形显示
        WAVEFORM_ENABLE = 0;
    }
    if (((ano_fun == 0xE0) && (ano_fun_1 == 0xE1)) && write_count == 0)
    {
        if (ano_read_val_num == 0x00) //读取参数起始命令
        {
            unsigned char _buff[17] = {0xAA, 0x05, 0xAF, 0xE1, 0x0B, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x4A};
            //ANO_DT_Send_Data(_buff, 17);
            //DMA_NVIC_Send_Config(ANO_UART_DMA_CHANNEL,(u32)&UART3->TDR,&_buff[0],17);
            uart_putbuff(UART_3, &_buff[0], 17);
        }
        else//读取每个参数
        {
            unsigned char _buff[17] = {0xAA, 0x05, 0xAF, 0xE1, 0x0B, 0, ano_read_val_num, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            _buff[5] = (ano_read_val_num & 0xff00) >> 8;
            _buff[6] = ano_read_val_num;
            Read_Params_of_MCU();//读取MUC的参数
            if (ano_read_val_num % 3 == 0)
            {
                _buff[7] = (pid_param[ano_read_val_num / 3 - 1].D & 0xFF000000) >> 24;
                _buff[8] = (pid_param[ano_read_val_num / 3 - 1].D & 0xFF0000) >> 16;
                _buff[9] = (pid_param[ano_read_val_num / 3 - 1].D & 0xFF00) >> 8;
                _buff[10] = pid_param[ano_read_val_num / 3 - 1].D & 0xFF;
                //              _buff[7] = (10 & 0xFF000000)>>24;
                //                _buff[8] = (10 & 0xFF0000)>>16;
                //                _buff[9] = (10 & 0xFF00)>>8;
                //                _buff[10] = 10 & 0xFF;
            }
            else if (ano_read_val_num % 3 == 1)
            {
                _buff[7] = (pid_param[ano_read_val_num / 3].P & 0xFF000000) >> 24;
                _buff[8] = (pid_param[ano_read_val_num / 3].P & 0xFF0000) >> 16;
                _buff[9] = (pid_param[ano_read_val_num / 3].P & 0xFF00) >> 8;
                _buff[10] = pid_param[ano_read_val_num / 3].P & 0xFF;
            }
            else if (ano_read_val_num % 3 == 2)
            {
                _buff[7] = (pid_param[ano_read_val_num / 3].I & 0xFF000000) >> 24;
                _buff[8] = (pid_param[ano_read_val_num / 3].I & 0xFF0000) >> 16;
                _buff[9] = (pid_param[ano_read_val_num / 3].I & 0xFF00) >> 8;
                _buff[10] = pid_param[ano_read_val_num / 3].I & 0xFF;
            }
            for (unsigned char j = 0; j < 16; j ++)
            {
                _buff[16] += _buff[j];
            }
            //DMA_NVIC_Send_Config(ANO_UART_DMA_CHANNEL,(u32)&UART3->TDR,(u32)&_buff,17);
            uart_putbuff(UART_3, &_buff[0], 17);
            if (ano_read_val_num >= 0x49) WAVEFORM_ENABLE = 1; //开启波形显示
        }
    }
    if ((ano_fun == 0xE1) && write_count == 0) //参数修改命令
    {
        unsigned char __buff[12] = {0xAA, 0x05, 0xAF, 0xE1, 0x06, 0, 0, 0, 0, 0, 0, 0};
        __buff[5] = (ano_write_val_num & 0xff00) >> 8;
        __buff[6] = ano_write_val_num;
        __buff[7] = (w_value & 0xFF000000) >> 24;
        __buff[8] = (w_value & 0xFF0000) >> 16;
        __buff[9] = (w_value & 0xFF00) >> 8;
        __buff[10] = w_value & 0xFF;
        if (ano_write_val_num % 3 == 0)  pid_param[ano_write_val_num / 3 - 1].D = w_value; //修改MCU参数
        if (ano_write_val_num % 3 == 1)  pid_param[ano_write_val_num / 3 ].P = w_value;
        if (ano_write_val_num % 3 == 2)  pid_param[ano_write_val_num / 3 ].I = w_value;
        Write_Params_of_MCU();
        for (unsigned char j = 0; j < 11; j ++)
        {
            __buff[11] += __buff[j];
        }
        //      DMA_NVIC_Send_Config(ANO_UART_DMA_CHANNEL,(u32)&UART3->TDR,(u32)&__buff,12);
        uart_putbuff(UART_3, &__buff[0], 12);
        if (ano_write_val_num >= 0x49) WAVEFORM_ENABLE = 1; //开启波形显示
    }
    //清除数据
    ano_fun = 0;
    ano_fun_1 = 0;
    ano_read_val_num = 0;
}



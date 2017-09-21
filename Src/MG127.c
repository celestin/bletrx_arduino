/**
  ******************************************************************************
  * @file    :MG127.c
  * @author  :MG Team
  * @version :V1.0
  * @date
  * @brief
  ******************************************************************************
***/

/* Includes ------------------------------------------------------------------*/
#include "Includes.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/



/* Private macro -------------------------------------------------------------*/
//MAC 地址长度 6字节
#define LEN_BLE_ADDR 6


/* Private variables ---------------------------------------------------------*/
//ms 计数器
extern unsigned short tick;

//RX buffer 39 字节
unsigned char rx_buf[39]; //include header(2B)+mac(6B)+data(max31B), for rx application

//BLE ADV_data, maxlen=31
//广播数据长度
#define LEN_DATA 30
//广播数组内容 PDU 格式。
uint8_t adv_data[30] = {0x02,0x01,0x04, 0x1a,0xff,0x4c,0x00,2,0x15, 0xfd,0xa5,0x06,0x93,0xa4,0xe2,0x4f,0xb1,0xaf,0xcf,0xc6,0xeb,0x07,0x64,0x78,0x25, 0x27,0x32,0x52,0xa9, 0xB6};


/* Private function prototypes -----------------------------------------------*/
//BLE 校准，温度变化而改变
void BLE_Do_Cal(void);


/*******************************************************************************
* Function   :     	BLE_Mode_Sleep  BLE睡眠模式
* Parameter  :     	void
* Returns    :     	void
* Description:
* Note:      : 		BLE enter sleep mode. current: 3ua
*******************************************************************************/
void BLE_Mode_Sleep(void)
{
    uint8_t	temp0[4];

    temp0[0] = 0x02;
    temp0[1] = 0xff;
    temp0[2] = 0xff;
    temp0[3] = 0xff;

    SPI_Write_Buffer(SLEEP_WAKEUP,temp0,4);
}

/*******************************************************************************
* Function   :     	BLE_Mode_Wakeup BLE 唤醒模式
* Parameter  :     	void
* Returns    :     	void
* Description:
* Note:      : 		BLE reg:0x00--0x1f. write operation must or 0x20
*******************************************************************************/
void BLE_Mode_Wakeup(void)
{
    SPI_Write_Reg(SLEEP_WAKEUP|0x20, 0x01);
}

/*******************************************************************************
* Function   :     	BLE_Mode_PwrUp BLE 上电模式
* Parameter  :     	void
* Returns    :     	void
* Description:
* Note:      : 		BLE Power up
*******************************************************************************/
void BLE_Mode_PwrUp(void)
{
    SPI_Write_Reg(0X50, 0x51);//发送spi命令 50 51
    SPI_Write_Reg(0X20, 0x7a); //pwr up//发送spi命令 20 7a

    SPI_Write_Reg(0x50, 0x53);//发送spi命令 50 53
    BLE_Do_Cal();
}

/*******************************************************************************
* Function   :     	BLE_Mode_PwrDn BLE 掉电模式
* Parameter  :     	void
* Returns    :     	void
* Description:
* Note:      : 		BLE Power Dwon
*******************************************************************************/
void BLE_Mode_PwrDn(void)
{
    SPI_Write_Reg(0X50, 0x51);//发送spi命令 50 51
    SPI_Write_Reg(0X20, 0x78); //pwr down //发送spi命令 20 78
    SPI_Write_Reg(0X50, 0x56);//发送spi命令 50 56
}


/*******************************************************************************
* Function   :     	BLE_Set_StartTime 开始时间
* Parameter  :     	uint32_t
* Returns    :     	void
* Description:		BLE RX TX模式中。需要设置此时间。执行完次时间后BLE 才进入真正TX RX 状态
* Note:      :
*******************************************************************************/
void BLE_Set_StartTime(uint32_t htime)
{
    uint8_t temp0[3];

    temp0[0] = htime & 0xFF;
    temp0[1] = (htime>>8) & 0xFF;
    temp0[2] = (htime>>16) & 0xFF;

    SPI_Write_Buffer(START_TIME,temp0,3);
}


/*******************************************************************************
* Function   :     	BLE_Set_TimeOut 溢出时间设置
* Parameter  :     	uint32_t data_us  
* Returns    :     	void
* Description:      TX/RX timeout .unit:us  单位:us
* Note:      :
*******************************************************************************/
void BLE_Set_TimeOut(uint32_t data_us)
{
    uint8_t temp0[3];

    temp0[0] = data_us & 0xff;
    temp0[1] = (data_us >> 8) & 0xff;
    temp0[2] = (data_us >> 16) & 0xff;

    SPI_Write_Buffer(TIMEOUT, temp0, 3);
}

/*******************************************************************************
* Function   :     	BLE_Set_Xtal
* Parameter  :     	uint8_t
* Returns    :     	void
* Description:		BLE 晶体数据设置 。
* Note:      :
*******************************************************************************/
void BLE_Set_Xtal(uint8_t on_flag)
{
    SPI_Write_Reg(0x50, 0x53);
    SPI_Write_Reg(0x3D, 0x18|(on_flag<<2));
    SPI_Write_Reg(0x50, 0x56);
}

/*******************************************************************************
* Function   :     	BLE_Get_Pdu   
* Parameter  :     	uint8_t *, uint8_t *
* Returns    :     	void
* Description:
* Note:      :		BLE rx OK.read fifo data .  rx OK 。读取FIFO 数据
*******************************************************************************/
static void BLE_Get_Pdu(uint8_t *ptr, uint8_t *len)
{
    uint8_t hdr_type;
    uint8_t len_tmp;
    uint8_t bank_buf[6];
	//判断收据数据的长度和标志
    SPI_Read_Buffer(ADV_HDR_RX, bank_buf, 2);

    *len = bank_buf[1] + 2;
    ptr[0] = bank_buf[0];
    ptr[1] = bank_buf[1];

    hdr_type = bank_buf[0] & 0xF;
    len_tmp = bank_buf[1];
	//根据接收数据标志不同，进行不同操作
    switch(hdr_type)
    {
    case ADV_IND:  //advA+0~31  连接广播包
    case ADV_NONCONN_IND://非连接广播包
        //case ADV_SCAN_IND:
        //case ADV_SCAN_RSP:
        //读取接收数据的MAC 地址
        SPI_Read_Buffer(INITA_RX, &ptr[2], LEN_BLE_ADDR);  //INITA
        len_tmp -= LEN_BLE_ADDR;
        break;
    /*
            case ADV_DIRECT_IND:  //advA+InitA
                if(len_tmp == 12){
                    SPI_Read_Buffer(ADVA_RX, &ptr[2], LEN_BLE_ADDR);  //advA
                    len_tmp -= LEN_BLE_ADDR;
                    SPI_Read_Buffer(INITA_RX, &ptr[2+LEN_BLE_ADDR], LEN_BLE_ADDR);  //INITA
                    len_tmp -= LEN_BLE_ADDR;
                }
                len_tmp = 0;
                break;
            case ADV_SCAN_REQ:  //scanA + advA
                if(len_tmp == 12){
                    SPI_Read_Buffer(INITA_RX, &ptr[2], LEN_BLE_ADDR);  //INITA
                    len_tmp -= LEN_BLE_ADDR;
                    SPI_Read_Buffer(ADVA_RX, &ptr[2+LEN_BLE_ADDR], LEN_BLE_ADDR);  //advA
                    len_tmp -= LEN_BLE_ADDR;
                }
                len_tmp = 0;
                break;
            case ADV_CONN_REQ:  //InitA + advA + LL(22B)
                if(len_tmp == 34){
                    SPI_Read_Buffer(INITA_RX, &ptr[2], LEN_BLE_ADDR);  //INITA
                    SPI_Read_Buffer(ADVA_RX, &ptr[2+LEN_BLE_ADDR], LEN_BLE_ADDR);  //advA
                    SPI_Read_Buffer(R_RX_PAYLOAD, &ptr[2+LEN_BLE_ADDR+LEN_BLE_ADDR], 22);
                }
                len_tmp = 0;
                break;
    */
    default:
        len_tmp = 0;
        break;
    }

    if(len_tmp > 0)
    {
    	//读取RX数据其它 数据 PDU TYPE 为主
        SPI_Read_Buffer(R_RX_PAYLOAD, &ptr[2+LEN_BLE_ADDR], len_tmp);
    }
}


#define TXGAIN_DFF 0x15
//BLE 校准PLL 稳定性相关  固定内容
void BLE_Do_Cal()  //calibration
{
    uint8_t data_buf[2];

    SPI_Write_Reg(0x3F, 0x03);//发送spi命令 3f 03
    do
    {
        data_buf[0] = SPI_Read_Reg(0x1F);//发送spi命令 1f
    }
    while(data_buf[0]&0x03);

    SPI_Write_Reg(0x3F, 0x03);//发送spi命令 3f 03
    do
    {
        data_buf[0] = SPI_Read_Reg(0x1F);//发送spi命令 1f
    }
    while(data_buf[0]&0x03);

    //////////////////////////////////////////////////
    SPI_Write_Reg(0x35,0x01); //testm for tx/temp//发送spi命令 35 01
    SPI_Write_Reg(0x32,0xA0);//发送spi命令 32 a0
    SPI_Write_Reg(0x2a,0x04);//发送spi命令 2a 04
    SPI_Write_Reg(0x2a,0x00);//发送spi命令 2a 00

    SPI_Write_Reg(0x32,0x88);//发送spi命令 32 88
    data_buf[0] = 0x01;
    data_buf[1] = 0x21;
    SPI_Write_Buffer(0x13, data_buf, 2);//发送spi命令 13 01 21
    data_buf[0] = 0x01;
    data_buf[1] = 0x20;
    SPI_Write_Buffer(0x13, data_buf, 2);//发送spi命令 13 01 20
    SPI_Write_Reg(0x35,0x00);  //exist testm//发送spi命令 35 00
    ////////////////////////////////////////////////////
    SPI_Write_Reg(0x50, 0x56);//发送spi命令 50 56
}

/*******************************************************************************
* Function   :     	BLE_Init  蓝牙初始化
* Parameter  :     	void
* Returns    :     	void
* Description:      power on .BLE must initnal reg .  蓝牙芯片上电后必须执行的操作
* Note:      : 		delay 30ms .
*******************************************************************************/
void BLE_Init(void)
{
    uint8_t status;
    uint8_t data_buf[4];
    uint8_t ble_Addr[6];

    SPI_Write_Reg(0x50, 0x51);//发送spi命令 50 51
    SPI_Write_Reg(0x50, 0x53);//发送spi命令 50 53
    SPI_Write_Reg(0x35, 0x00);//发送spi命令 35 00
    SPI_Write_Reg(0x3D, 0x18);//发送spi命令 3D 18
    SPI_Write_Reg(0x50, 0x51);//发送spi命令 50 51

    do
    {
    	//固定格式配置内部寄存器
        SPI_Write_Reg(0x50, 0x53);//发送spi命令 50 53

        data_buf[0] = 0;
        data_buf[1] = 0;
        data_buf[2] = 1;
        SPI_Write_Buffer(0x00, data_buf, 3);//发送spi命令 00 00 00 01

        SPI_Write_Reg(0x36, 0x8e);//发送spi命令 36 8e
        SPI_Write_Reg(0x37, 0x8e);//发送spi命令 37 8e
        SPI_Write_Reg(0x38, 0x88);//发送spi命令 38 88
        SPI_Write_Reg(0x39, 0x8e);//发送spi命令 39 8e

        SPI_Write_Reg(0x50, 0x51);//发送spi命令 5051

        SPI_Read_Reg(0x1e);//发送spi命令 1e
		//读取芯片状态。判断SPI 是否OK 。0x80表示芯片识别OK
        status = SPI_Read_Reg(CHIP_OK);
    }
    while(status != 0x80);


    //read chip version  读取芯片版本
    status = SPI_Read_Reg(0x1e);
#if 1 //debug  方便调试使用
    Uart_Send_String("chip version=");
    Uart_Send_Byte(status);
    Uart_Send_String("\r\n");
#endif

    SPI_Write_Reg(0X20, 0x78);//power down,tx, for hot reset BLE 进入掉电模式
    SPI_Write_Reg(0X26, 0x06);//1Mbps   设置发送速率1Mbps
    SPI_Write_Reg(0X20, 0x7a);//power up 设置芯片上电模式

    SPI_Write_Reg(0x50, 0x56);//发送spi命令 50 56

    BLE_Mode_Sleep();//进入睡眠模式

    //read BLE address. BLE MAC Address  读取芯片MAC 地址
    SPI_Read_Buffer(0x08, ble_Addr, 6);
#if 1 //debug 打印MAC 地址信息
    Uart_Send_String("BleAddr=");
    Uart_Send_Byte(ble_Addr[5]);
    Uart_Send_Byte(ble_Addr[4]);
    Uart_Send_Byte(ble_Addr[3]);
    Uart_Send_Byte(ble_Addr[2]);
    Uart_Send_Byte(ble_Addr[1]);
    Uart_Send_Byte(ble_Addr[0]);
    Uart_Send_String("\r\n");
#endif

	//固定格式   BLE 内部寄存器设置
    SPI_Write_Reg(0x50, 0x53);//发送spi命令 50 53

    data_buf[0] = 0xff;
    data_buf[1] = 0x80; //xocc
    SPI_Write_Buffer(0x14,data_buf,2);//发送spi命令 14 ff 80

    //set BLE TX Power  设置发射功率
    data_buf[0] = 0x02;
    data_buf[1] = BLE_TX_POWER;
    SPI_Write_Buffer(0x0f,data_buf,2);

    data_buf[1] = SPI_Read_Reg(0x08); //发送spi命令 08
    if(0 == data_buf[1])
    {
        data_buf[1] = TXGAIN_DFF;
    }
    data_buf[0] = 0xc0;
    data_buf[2] = 0x1D; // 1E, 20161212
    SPI_Write_Buffer(0x4, data_buf, 3);//发送spi命令 04 c0 1d


    data_buf[0] = 0;
    data_buf[1] = 0x00; //default08  20161212
    SPI_Write_Buffer(0x0C, data_buf, 2);//发送spi命令 0c 00 00

    data_buf[0] = 0x81;
    data_buf[1] = 0x22;
    SPI_Write_Buffer(0x13, data_buf, 2);//发送spi命令 13 81 22

    SPI_Write_Reg(0x3C, 0x30);//发送spi命令 3c 30
    SPI_Write_Reg(0x3E, 0x30);//发送spi命令 3e 30

    data_buf[0] = 0x10;
    data_buf[1] = 0x02;
    SPI_Write_Buffer(0xA, data_buf, 2);//发送spi命令 0a 10 02

    data_buf[0] = 0x02;
    data_buf[1] = 0x12;
    SPI_Write_Buffer(0xD, data_buf, 2);//发送spi命令 0d 02 12

    data_buf[0] = 0x01;
    data_buf[1] = 0x07;
    SPI_Write_Buffer(0xE, data_buf, 2);//发送spi命令 0e 01 07

    SPI_Write_Reg(0x50, 0x56);//发送spi命令 50 56
}

/*******************************************************************************
* Function   :     	BLE_TRX
* Parameter  :     	txcnt, rxcnt
* Returns    :     	void
* Description:      Beacon data .process .蓝牙数据收发循环
* Note:      :
*******************************************************************************/
void BLE_TRX()
{
    uint8_t status = 0;
    uint8_t ch = 37;//设置发送通道
    uint8_t data_buf[2];
    uint8_t tmp_cnt = txcnt+rxcnt;//判断蓝牙收发工作次数
    uint8_t len_pdu = 0;
    uint8_t loop = 0;
	//如果收发次数是0.将推出循环
    if(tmp_cnt == 0) return;
	//设置 晶体参数
    BLE_Set_Xtal(1);
	//BLE 进入上电模式
    BLE_Mode_PwrUp();

#if 1  //if adv_data no change, can move this block to the end of BLE_Init()
    //set BLE TX default channel:37.38.39、
    //设置蓝牙工作通道。 广播通道有3个：37 38 39 .默认37开始工作
    SPI_Write_Reg(CH_NO|0X20, ch);

    //BLT FIFO write adv_data . max len:31 byte  设置BLE 发射FIFO .自定义数组需要此调用前改变
    SPI_Write_Buffer(W_TX_PAYLOAD, adv_data, LEN_DATA);

    //PDU TYPE: 2  non-connectable undirected advertising . tx add:random address
    //set BLT PDU length:adv_data+6 mac adress.
    //BLE PDU 设置 非连接广播包，   tx add:随机地址 
    data_buf[0] = 0x02;
	//BLE PDU 数据长度 广播数据+MAC 地址长度
    data_buf[1] = LEN_DATA+LEN_BLE_ADDR;
	//写入寄存器
    SPI_Write_Buffer(ADV_HDR_TX, data_buf, 2);

    //clear all interrupt  清除BLE 寄存器中断标志
    data_buf[0] = 0xFF;
    data_buf[1] = 0x80;
    SPI_Write_Buffer(INT_FLAG, data_buf, 2);
#endif
	//BLE 进入唤醒模式
    BLE_Mode_Wakeup();
	//设置蓝牙RX 溢出时间。默认30MS 。BLE_RX_TIMEOUT 单位：US 。
	//用途：BLE 进入RX模式，RX等待的时间。
    BLE_Set_TimeOut(BLE_RX_TIMEOUT);
	//BLE 未任何中断的保护时间。
    tick = BLE_GUARD_TIME;

    while(1)
    {
        //BLE IRQ LOW  读取IRQ脚状态
        if (!BLE_IRQ_GET())
        {
            //clear interrupt flag 读取状态寄存器中断标志
            status = SPI_Read_Reg(INT_FLAG);
			//清除状态寄存器中断标志
            SPI_Write_Reg(INT_FLAG|0X20, status);
            //Uart_Send_Byte(status); //debug

            if(INT_TYPE_WAKEUP & status)//BLE 进入唤醒模式
            {
                if(txcnt > 0)
                {
                	//如果txcnt >0.将使能一次发送，并把txcnt减一。
                	//BLE_Set_StartTime 时间和MCU SPI 时钟有关系。时钟越快BLE_START_TIME 可以适当减小默认：10ms
                	//使能BLE 发射内部寄存器将准备发射配置和数据， 等待BLE_START_TIME时间将发射数据。
                    txcnt --;
                    SPI_Write_Reg(MODE_TYPE|0X20, RADIO_MODE_ADV_TX);
                    BLE_Set_StartTime(BLE_START_TIME);
                }
                else if(rxcnt > 0)
                {
                	//如果rxcnt >0.将使能一次接收，并把rxcnt减一。
                	//BLE_Set_StartTime 时间和MCU SPI 时钟有关系。时钟越快BLE_START_TIME 可以适当减小默认：10ms
                	//使能BLE 发射内部寄存器将准备发射配置和数据， 等待BLE_START_TIME时间将接收数据。接收数据
                	//等待时间是由BLE_RX_TIMEOUT设置

                    rxcnt --;
                    SPI_Write_Reg(MODE_TYPE|0X20, RADIO_MODE_ADV_RX);
                    BLE_Set_StartTime(BLE_START_TIME);
                }
                continue; //goto while(1)

            }

			//BLE 进入睡眠模式
            BLE_Mode_Sleep();

			//BLE 接收到数据将执行此部分
            if(INT_TYPE_PDU_OK & status)  //only happen in rx application, no need porting in tx only application
            {
                LED_RED_ON(); //debug  LED 亮
                //获取蓝牙接收数据的内容
                BLE_Get_Pdu(rx_buf, &len_pdu);
#if 1 //debug	答应接收数据内容
                Uart_Send_String("\r\nRX: ");
                for(loop=0; loop<len_pdu; loop++)
                {
                    Uart_Send_Byte(rx_buf[loop]);
                    Uart_Send_String(" ");
                }
#endif
            }
            else if(INT_TYPE_TX_START & status)   //only happen in tx application//TX 开始发射数据。将执行此函数
            {
            	
                LED_GREEN_ON(); //debug
            }

            if(INT_TYPE_SLEEP & status)//sleep BLE 进入睡眠状态执行的代码
            {
                LED_GREEN_OFF(); //debug
                LED_RED_OFF();  //debug
                //此部分可以作为一个保护
                tick = BLE_GUARD_TIME;

                //BLE channel  睡眠的时候需要为下一次接收或发射做状态。切换通道。 37 38 39 循环
                if (++ch > 39)
                {
                    ch = 37;
                }
				//设置下一次发射或者接收通道
                SPI_Write_Reg(CH_NO|0X20, ch);

                tmp_cnt --;
                if(tmp_cnt == 0)
                {
                	//发射和接收到执行完成，将设置晶体寄存器，关闭电源。退出BLE_TRX 函数
                    BLE_Set_Xtal(0);
                    BLE_Mode_PwrDn();
                    break; //exit from while(1)
                }
                else
                    BLE_Mode_Wakeup();//如果未执行完成，将进入唤醒状态。
            }

        }
        else if(tick == 0)  //robustness, in case no int 
        {
        	//tick ==0.一个延时都没任何中断发生。将强行进入睡眠模式。
            BLE_Mode_Sleep();
        }

    }
}

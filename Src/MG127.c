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
//MAC ��ַ���� 6�ֽ�
#define LEN_BLE_ADDR 6


/* Private variables ---------------------------------------------------------*/
//ms ������
extern unsigned short tick;

//RX buffer 39 �ֽ�
unsigned char rx_buf[39]; //include header(2B)+mac(6B)+data(max31B), for rx application

//BLE ADV_data, maxlen=31
//�㲥���ݳ���
#define LEN_DATA 30
//�㲥�������� PDU ��ʽ��
uint8_t adv_data[30] = {0x02,0x01,0x04, 0x1a,0xff,0x4c,0x00,2,0x15, 0xfd,0xa5,0x06,0x93,0xa4,0xe2,0x4f,0xb1,0xaf,0xcf,0xc6,0xeb,0x07,0x64,0x78,0x25, 0x27,0x32,0x52,0xa9, 0xB6};


/* Private function prototypes -----------------------------------------------*/
//BLE У׼���¶ȱ仯���ı�
void BLE_Do_Cal(void);


/*******************************************************************************
* Function   :     	BLE_Mode_Sleep  BLE˯��ģʽ
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
* Function   :     	BLE_Mode_Wakeup BLE ����ģʽ
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
* Function   :     	BLE_Mode_PwrUp BLE �ϵ�ģʽ
* Parameter  :     	void
* Returns    :     	void
* Description:
* Note:      : 		BLE Power up
*******************************************************************************/
void BLE_Mode_PwrUp(void)
{
    SPI_Write_Reg(0X50, 0x51);//����spi���� 50 51
    SPI_Write_Reg(0X20, 0x7a); //pwr up//����spi���� 20 7a

    SPI_Write_Reg(0x50, 0x53);//����spi���� 50 53
    BLE_Do_Cal();
}

/*******************************************************************************
* Function   :     	BLE_Mode_PwrDn BLE ����ģʽ
* Parameter  :     	void
* Returns    :     	void
* Description:
* Note:      : 		BLE Power Dwon
*******************************************************************************/
void BLE_Mode_PwrDn(void)
{
    SPI_Write_Reg(0X50, 0x51);//����spi���� 50 51
    SPI_Write_Reg(0X20, 0x78); //pwr down //����spi���� 20 78
    SPI_Write_Reg(0X50, 0x56);//����spi���� 50 56
}


/*******************************************************************************
* Function   :     	BLE_Set_StartTime ��ʼʱ��
* Parameter  :     	uint32_t
* Returns    :     	void
* Description:		BLE RX TXģʽ�С���Ҫ���ô�ʱ�䡣ִ�����ʱ���BLE �Ž�������TX RX ״̬
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
* Function   :     	BLE_Set_TimeOut ���ʱ������
* Parameter  :     	uint32_t data_us  
* Returns    :     	void
* Description:      TX/RX timeout .unit:us  ��λ:us
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
* Description:		BLE ������������ ��
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
* Note:      :		BLE rx OK.read fifo data .  rx OK ����ȡFIFO ����
*******************************************************************************/
static void BLE_Get_Pdu(uint8_t *ptr, uint8_t *len)
{
    uint8_t hdr_type;
    uint8_t len_tmp;
    uint8_t bank_buf[6];
	//�ж��վ����ݵĳ��Ⱥͱ�־
    SPI_Read_Buffer(ADV_HDR_RX, bank_buf, 2);

    *len = bank_buf[1] + 2;
    ptr[0] = bank_buf[0];
    ptr[1] = bank_buf[1];

    hdr_type = bank_buf[0] & 0xF;
    len_tmp = bank_buf[1];
	//���ݽ������ݱ�־��ͬ�����в�ͬ����
    switch(hdr_type)
    {
    case ADV_IND:  //advA+0~31  ���ӹ㲥��
    case ADV_NONCONN_IND://�����ӹ㲥��
        //case ADV_SCAN_IND:
        //case ADV_SCAN_RSP:
        //��ȡ�������ݵ�MAC ��ַ
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
    	//��ȡRX�������� ���� PDU TYPE Ϊ��
        SPI_Read_Buffer(R_RX_PAYLOAD, &ptr[2+LEN_BLE_ADDR], len_tmp);
    }
}


#define TXGAIN_DFF 0x15
//BLE У׼PLL �ȶ������  �̶�����
void BLE_Do_Cal()  //calibration
{
    uint8_t data_buf[2];

    SPI_Write_Reg(0x3F, 0x03);//����spi���� 3f 03
    do
    {
        data_buf[0] = SPI_Read_Reg(0x1F);//����spi���� 1f
    }
    while(data_buf[0]&0x03);

    SPI_Write_Reg(0x3F, 0x03);//����spi���� 3f 03
    do
    {
        data_buf[0] = SPI_Read_Reg(0x1F);//����spi���� 1f
    }
    while(data_buf[0]&0x03);

    //////////////////////////////////////////////////
    SPI_Write_Reg(0x35,0x01); //testm for tx/temp//����spi���� 35 01
    SPI_Write_Reg(0x32,0xA0);//����spi���� 32 a0
    SPI_Write_Reg(0x2a,0x04);//����spi���� 2a 04
    SPI_Write_Reg(0x2a,0x00);//����spi���� 2a 00

    SPI_Write_Reg(0x32,0x88);//����spi���� 32 88
    data_buf[0] = 0x01;
    data_buf[1] = 0x21;
    SPI_Write_Buffer(0x13, data_buf, 2);//����spi���� 13 01 21
    data_buf[0] = 0x01;
    data_buf[1] = 0x20;
    SPI_Write_Buffer(0x13, data_buf, 2);//����spi���� 13 01 20
    SPI_Write_Reg(0x35,0x00);  //exist testm//����spi���� 35 00
    ////////////////////////////////////////////////////
    SPI_Write_Reg(0x50, 0x56);//����spi���� 50 56
}

/*******************************************************************************
* Function   :     	BLE_Init  ������ʼ��
* Parameter  :     	void
* Returns    :     	void
* Description:      power on .BLE must initnal reg .  ����оƬ�ϵ�����ִ�еĲ���
* Note:      : 		delay 30ms .
*******************************************************************************/
void BLE_Init(void)
{
    uint8_t status;
    uint8_t data_buf[4];
    uint8_t ble_Addr[6];

    SPI_Write_Reg(0x50, 0x51);//����spi���� 50 51
    SPI_Write_Reg(0x50, 0x53);//����spi���� 50 53
    SPI_Write_Reg(0x35, 0x00);//����spi���� 35 00
    SPI_Write_Reg(0x3D, 0x18);//����spi���� 3D 18
    SPI_Write_Reg(0x50, 0x51);//����spi���� 50 51

    do
    {
    	//�̶���ʽ�����ڲ��Ĵ���
        SPI_Write_Reg(0x50, 0x53);//����spi���� 50 53

        data_buf[0] = 0;
        data_buf[1] = 0;
        data_buf[2] = 1;
        SPI_Write_Buffer(0x00, data_buf, 3);//����spi���� 00 00 00 01

        SPI_Write_Reg(0x36, 0x8e);//����spi���� 36 8e
        SPI_Write_Reg(0x37, 0x8e);//����spi���� 37 8e
        SPI_Write_Reg(0x38, 0x88);//����spi���� 38 88
        SPI_Write_Reg(0x39, 0x8e);//����spi���� 39 8e

        SPI_Write_Reg(0x50, 0x51);//����spi���� 5051

        SPI_Read_Reg(0x1e);//����spi���� 1e
		//��ȡоƬ״̬���ж�SPI �Ƿ�OK ��0x80��ʾоƬʶ��OK
        status = SPI_Read_Reg(CHIP_OK);
    }
    while(status != 0x80);


    //read chip version  ��ȡоƬ�汾
    status = SPI_Read_Reg(0x1e);
#if 1 //debug  �������ʹ��
    Uart_Send_String("chip version=");
    Uart_Send_Byte(status);
    Uart_Send_String("\r\n");
#endif

    SPI_Write_Reg(0X20, 0x78);//power down,tx, for hot reset BLE �������ģʽ
    SPI_Write_Reg(0X26, 0x06);//1Mbps   ���÷�������1Mbps
    SPI_Write_Reg(0X20, 0x7a);//power up ����оƬ�ϵ�ģʽ

    SPI_Write_Reg(0x50, 0x56);//����spi���� 50 56

    BLE_Mode_Sleep();//����˯��ģʽ

    //read BLE address. BLE MAC Address  ��ȡоƬMAC ��ַ
    SPI_Read_Buffer(0x08, ble_Addr, 6);
#if 1 //debug ��ӡMAC ��ַ��Ϣ
    Uart_Send_String("BleAddr=");
    Uart_Send_Byte(ble_Addr[5]);
    Uart_Send_Byte(ble_Addr[4]);
    Uart_Send_Byte(ble_Addr[3]);
    Uart_Send_Byte(ble_Addr[2]);
    Uart_Send_Byte(ble_Addr[1]);
    Uart_Send_Byte(ble_Addr[0]);
    Uart_Send_String("\r\n");
#endif

	//�̶���ʽ   BLE �ڲ��Ĵ�������
    SPI_Write_Reg(0x50, 0x53);//����spi���� 50 53

    data_buf[0] = 0xff;
    data_buf[1] = 0x80; //xocc
    SPI_Write_Buffer(0x14,data_buf,2);//����spi���� 14 ff 80

    //set BLE TX Power  ���÷��书��
    data_buf[0] = 0x02;
    data_buf[1] = BLE_TX_POWER;
    SPI_Write_Buffer(0x0f,data_buf,2);

    data_buf[1] = SPI_Read_Reg(0x08); //����spi���� 08
    if(0 == data_buf[1])
    {
        data_buf[1] = TXGAIN_DFF;
    }
    data_buf[0] = 0xc0;
    data_buf[2] = 0x1D; // 1E, 20161212
    SPI_Write_Buffer(0x4, data_buf, 3);//����spi���� 04 c0 1d


    data_buf[0] = 0;
    data_buf[1] = 0x00; //default08  20161212
    SPI_Write_Buffer(0x0C, data_buf, 2);//����spi���� 0c 00 00

    data_buf[0] = 0x81;
    data_buf[1] = 0x22;
    SPI_Write_Buffer(0x13, data_buf, 2);//����spi���� 13 81 22

    SPI_Write_Reg(0x3C, 0x30);//����spi���� 3c 30
    SPI_Write_Reg(0x3E, 0x30);//����spi���� 3e 30

    data_buf[0] = 0x10;
    data_buf[1] = 0x02;
    SPI_Write_Buffer(0xA, data_buf, 2);//����spi���� 0a 10 02

    data_buf[0] = 0x02;
    data_buf[1] = 0x12;
    SPI_Write_Buffer(0xD, data_buf, 2);//����spi���� 0d 02 12

    data_buf[0] = 0x01;
    data_buf[1] = 0x07;
    SPI_Write_Buffer(0xE, data_buf, 2);//����spi���� 0e 01 07

    SPI_Write_Reg(0x50, 0x56);//����spi���� 50 56
}

/*******************************************************************************
* Function   :     	BLE_TRX
* Parameter  :     	txcnt, rxcnt
* Returns    :     	void
* Description:      Beacon data .process .���������շ�ѭ��
* Note:      :
*******************************************************************************/
void BLE_TRX()
{
    uint8_t status = 0;
    uint8_t ch = 37;//���÷���ͨ��
    uint8_t data_buf[2];
    uint8_t tmp_cnt = txcnt+rxcnt;//�ж������շ���������
    uint8_t len_pdu = 0;
    uint8_t loop = 0;
	//����շ�������0.���Ƴ�ѭ��
    if(tmp_cnt == 0) return;
	//���� �������
    BLE_Set_Xtal(1);
	//BLE �����ϵ�ģʽ
    BLE_Mode_PwrUp();

#if 1  //if adv_data no change, can move this block to the end of BLE_Init()
    //set BLE TX default channel:37.38.39��
    //������������ͨ���� �㲥ͨ����3����37 38 39 .Ĭ��37��ʼ����
    SPI_Write_Reg(CH_NO|0X20, ch);

    //BLT FIFO write adv_data . max len:31 byte  ����BLE ����FIFO .�Զ���������Ҫ�˵���ǰ�ı�
    SPI_Write_Buffer(W_TX_PAYLOAD, adv_data, LEN_DATA);

    //PDU TYPE: 2  non-connectable undirected advertising . tx add:random address
    //set BLT PDU length:adv_data+6 mac adress.
    //BLE PDU ���� �����ӹ㲥����   tx add:�����ַ 
    data_buf[0] = 0x02;
	//BLE PDU ���ݳ��� �㲥����+MAC ��ַ����
    data_buf[1] = LEN_DATA+LEN_BLE_ADDR;
	//д��Ĵ���
    SPI_Write_Buffer(ADV_HDR_TX, data_buf, 2);

    //clear all interrupt  ���BLE �Ĵ����жϱ�־
    data_buf[0] = 0xFF;
    data_buf[1] = 0x80;
    SPI_Write_Buffer(INT_FLAG, data_buf, 2);
#endif
	//BLE ���뻽��ģʽ
    BLE_Mode_Wakeup();
	//��������RX ���ʱ�䡣Ĭ��30MS ��BLE_RX_TIMEOUT ��λ��US ��
	//��;��BLE ����RXģʽ��RX�ȴ���ʱ�䡣
    BLE_Set_TimeOut(BLE_RX_TIMEOUT);
	//BLE δ�κ��жϵı���ʱ�䡣
    tick = BLE_GUARD_TIME;

    while(1)
    {
        //BLE IRQ LOW  ��ȡIRQ��״̬
        if (!BLE_IRQ_GET())
        {
            //clear interrupt flag ��ȡ״̬�Ĵ����жϱ�־
            status = SPI_Read_Reg(INT_FLAG);
			//���״̬�Ĵ����жϱ�־
            SPI_Write_Reg(INT_FLAG|0X20, status);
            //Uart_Send_Byte(status); //debug

            if(INT_TYPE_WAKEUP & status)//BLE ���뻽��ģʽ
            {
                if(txcnt > 0)
                {
                	//���txcnt >0.��ʹ��һ�η��ͣ�����txcnt��һ��
                	//BLE_Set_StartTime ʱ���MCU SPI ʱ���й�ϵ��ʱ��Խ��BLE_START_TIME �����ʵ���СĬ�ϣ�10ms
                	//ʹ��BLE �����ڲ��Ĵ�����׼���������ú����ݣ� �ȴ�BLE_START_TIMEʱ�佫�������ݡ�
                    txcnt --;
                    SPI_Write_Reg(MODE_TYPE|0X20, RADIO_MODE_ADV_TX);
                    BLE_Set_StartTime(BLE_START_TIME);
                }
                else if(rxcnt > 0)
                {
                	//���rxcnt >0.��ʹ��һ�ν��գ�����rxcnt��һ��
                	//BLE_Set_StartTime ʱ���MCU SPI ʱ���й�ϵ��ʱ��Խ��BLE_START_TIME �����ʵ���СĬ�ϣ�10ms
                	//ʹ��BLE �����ڲ��Ĵ�����׼���������ú����ݣ� �ȴ�BLE_START_TIMEʱ�佫�������ݡ���������
                	//�ȴ�ʱ������BLE_RX_TIMEOUT����

                    rxcnt --;
                    SPI_Write_Reg(MODE_TYPE|0X20, RADIO_MODE_ADV_RX);
                    BLE_Set_StartTime(BLE_START_TIME);
                }
                continue; //goto while(1)

            }

			//BLE ����˯��ģʽ
            BLE_Mode_Sleep();

			//BLE ���յ����ݽ�ִ�д˲���
            if(INT_TYPE_PDU_OK & status)  //only happen in rx application, no need porting in tx only application
            {
                LED_RED_ON(); //debug  LED ��
                //��ȡ�����������ݵ�����
                BLE_Get_Pdu(rx_buf, &len_pdu);
#if 1 //debug	��Ӧ������������
                Uart_Send_String("\r\nRX: ");
                for(loop=0; loop<len_pdu; loop++)
                {
                    Uart_Send_Byte(rx_buf[loop]);
                    Uart_Send_String(" ");
                }
#endif
            }
            else if(INT_TYPE_TX_START & status)   //only happen in tx application//TX ��ʼ�������ݡ���ִ�д˺���
            {
            	
                LED_GREEN_ON(); //debug
            }

            if(INT_TYPE_SLEEP & status)//sleep BLE ����˯��״ִ̬�еĴ���
            {
                LED_GREEN_OFF(); //debug
                LED_RED_OFF();  //debug
                //�˲��ֿ�����Ϊһ������
                tick = BLE_GUARD_TIME;

                //BLE channel  ˯�ߵ�ʱ����ҪΪ��һ�ν��ջ�����״̬���л�ͨ���� 37 38 39 ѭ��
                if (++ch > 39)
                {
                    ch = 37;
                }
				//������һ�η�����߽���ͨ��
                SPI_Write_Reg(CH_NO|0X20, ch);

                tmp_cnt --;
                if(tmp_cnt == 0)
                {
                	//����ͽ��յ�ִ����ɣ������þ���Ĵ������رյ�Դ���˳�BLE_TRX ����
                    BLE_Set_Xtal(0);
                    BLE_Mode_PwrDn();
                    break; //exit from while(1)
                }
                else
                    BLE_Mode_Wakeup();//���δִ����ɣ������뻽��״̬��
            }

        }
        else if(tick == 0)  //robustness, in case no int 
        {
        	//tick ==0.һ����ʱ��û�κ��жϷ�������ǿ�н���˯��ģʽ��
            BLE_Mode_Sleep();
        }

    }
}

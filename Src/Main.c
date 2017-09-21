/**
  ******************************************************************************
  * @file    :Main.c
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
/* Private variables ---------------------------------------------------------*/
//TX ����������
uint8_t txcnt = 0;
//RX ����������
uint8_t rxcnt = 0;


/* Private function prototypes -----------------------------------------------*/


/*******************************************************************************
* Function	 :		main ������
* Parameter  :		void
* Returns	 :		void
* Description:
* Note: 	 :
*******************************************************************************/
void main(void)
{
    //init mcu system  ��ʼ��MCU 
    Init_System();
	//�����
    LED_RED_ON();
	//��ʱ30MS
	Delay_ms(30);

    //BLE initial  BLE ��ʼ��
	BLE_Init();
    Uart_Send_String("BLE init ok.\r\n");

    while(1)
    {

        //////user proc
        //todo

        //////ble rtx api
        txcnt=3; //txcnt=0 is for rx only application ���÷�������
        rxcnt=6; //rxcnt=0 is for tx only application ���ý�������
        //����ִ��3�� ���䣬 6�� ���� �����˳�
        BLE_TRX();
        //Uart_Send_String("BLE trx done.\r\n");

        //delay to set ble tx interval  ����BLE ���䡢���ռ��
        Delay_ms(100);

    }
}


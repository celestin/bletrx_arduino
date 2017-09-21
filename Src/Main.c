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
//TX 次数计数器
uint8_t txcnt = 0;
//RX 次数计数器
uint8_t rxcnt = 0;


/* Private function prototypes -----------------------------------------------*/


/*******************************************************************************
* Function	 :		main 主函数
* Parameter  :		void
* Returns	 :		void
* Description:
* Note: 	 :
*******************************************************************************/
void main(void)
{
    //init mcu system  初始化MCU 
    Init_System();
	//红灯亮
    LED_RED_ON();
	//延时30MS
	Delay_ms(30);

    //BLE initial  BLE 初始化
	BLE_Init();
    Uart_Send_String("BLE init ok.\r\n");

    while(1)
    {

        //////user proc
        //todo

        //////ble rtx api
        txcnt=3; //txcnt=0 is for rx only application 设置发射数量
        rxcnt=6; //rxcnt=0 is for tx only application 设置接收数量
        //函数执行3次 发射， 6次 接收 ，才退出
        BLE_TRX();
        //Uart_Send_String("BLE trx done.\r\n");

        //delay to set ble tx interval  设置BLE 发射、接收间隔
        Delay_ms(100);

    }
}


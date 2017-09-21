/**
  ******************************************************************************
  * @file    :Uart.c
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

#define Hex2Ascii(data)  (data < 10)? ('0' + data) : ('A' + data - 10)

//Usart config data
//波特率
#define	Usart_BaudRate	115200UL
//数据长度
//USART_WordLength_8D: 8 bits Data
//USART_WordLength_9D: 9 bits Data	
#define Usart_Data_Len	USART_WordLength_8D
//停止喂
//USART_StopBits_1: One stop bit is transmitted at the end of frame
//USART_StopBits_2: Two stop bits are transmitted at the end of frame
#define	Usart_StopBit	USART_StopBits_1	
//USART_Parity_No: No Parity
//USART_Parity_Even: Even Parity
//USART_Parity_Odd: Odd Parity	 
//校验位
#define Usart_Parity 	USART_Parity_No
//USART_Mode_Rx: Receive Enable
//USART_Mode_Tx: Transmit Enable
//USART 模式
#define	Usart_Mode		(USART_Mode_Rx|USART_Mode_Tx)



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//set usart buffer len  BUFFER 长度
#define UART_BUF_LEN	20
//RX BUFFER
volatile uint8_t Rx_Buffer[UART_BUF_LEN] = {0}; 

//RX 计数器
volatile uint8_t Rx_Tx_Buffer_Cnt = 0;

/* Private function prototypes -----------------------------------------------*/


/*******************************************************************************
* Function   :     	Init_Uart	                                                                                                     
* Parameter  :     	void                                                                                        
* Returns    :     	void            
* Description:                               		                    
* Note:      :                                                                                                                   
*******************************************************************************/
void Init_Uart(void)
{
	//reset usart  复位USART
	USART_DeInit();
	//enalbe TX RX PULL Resistor  使能TX RX 上啦电阻
	GPIO_ExternalPullUpConfig(RX_PORT,RX|TX, ENABLE);
	//GPIO_ExternalPullUpConfig(TX_PORT,TX, ENABLE);
	
	//usart config data  配置USART 参数
	USART_Init((uint32_t)Usart_BaudRate,Usart_Data_Len,Usart_StopBit,Usart_Parity,(USART_Mode_TypeDef)Usart_Mode);
	//配置USART 数据格式
	USART_ClockInit(USART_Clock_Disable, USART_CPOL_Low, USART_CPHA_2Edge,USART_LastBit_Disable);

	//enable usart rx interrup  使能RX中断
	USART_ITConfig(USART_IT_RXNE, ENABLE);
	//不使能TX 中断和发送完成中断
	USART_ITConfig(USART_IT_TXE, DISABLE);
	USART_ITConfig(USART_IT_TC, DISABLE);

	//set RX interrupt Level 1.  设置中断优先级
	ITC_SetSoftwarePriority(USART_RX_IRQn,ITC_PriorityLevel_3);

	//enable usart function 使能USART
	USART_Cmd(ENABLE);
}
/*******************************************************************************
* Function   :     	Uart_Send_Byte	                                                                                                     
* Parameter  :     	char data                                                                                        
* Returns    :     	void            
* Description:                               		                    
* Note:      :                                                                                                                   
*******************************************************************************/
void Uart_Send_Byte(char data)
{
    uint8_t temp;
	uint8_t temp0[2];

	//1byte hex to asdcii 2byte.  ASCII 转换
    temp = data & 0x0F;
    temp0[0] = Hex2Ascii(temp);
    
    temp = data >> 4;
    temp0[1] = Hex2Ascii(temp);

	//usart send data  发送数据
	USART_SendData8(temp0[1]);

	/* Wait until end of transmit */
	while (USART_GetFlagStatus(USART_FLAG_TC) == RESET)
	{
		;
	}
	//发送数据
	USART_SendData8(temp0[0]);
	/* Wait until end of transmit */
	while (USART_GetFlagStatus(USART_FLAG_TC) == RESET)
	{
		;
	}

}


/*******************************************************************************
* Function   :     	Uart_Send_String	                                                                                                     
* Parameter  :     	char *data                                                                                        
* Returns    :     	void            
* Description:                               		                    
* Note:      :                                                                                                                   
*******************************************************************************/
void Uart_Send_String(char *data)
{	
	//check string end char
	while(*data != '\0')
	{
		//USART_SendData8(*data);
		  USART->DR = *data;
		/* Wait until end of transmit */
		while (USART_GetFlagStatus(USART_FLAG_TC) == RESET)
		{
			;
		}
		
		data++;	
	}
}






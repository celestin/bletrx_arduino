/**
  ******************************************************************************
  * @file    :Init.c
  * @author  :MG Team
  * @version :V1.0
  * @date    
  * @brief   
  ******************************************************************************
***/

/* Includes ------------------------------------------------------------------*/
#include "Includes.h"

///* Private typedef -----------------------------------------------------------*/
///* Private define ------------------------------------------------------------*/
//
////mcu fre:16MHz
//#define	SOFTWARE_MAJOR	1
//#define	SOFTWARE_MINOR	0
//
//
//
//// (1/((1/(16000000/1000))*64))  Downcounter.TIM2  time:1ms
//#define TIM2_1MS_VALUE	0x00FD
//
//
//
////mcu fre:16MHz
//#define	FREOSC	16000000UL
//
//
//
///* Private macro -------------------------------------------------------------*/
///* Private variables ---------------------------------------------------------*/
///* Private function prototypes -----------------------------------------------*/
//
//
///*******************************************************************************
//* Function	 :		Delay_us																									 
//* Parameter  :		delayCnt																						
//* Returns	 :		void			
//* Description:		Delay_us(10). delay time: 10us													
//* Note: 	 :																												 
//*******************************************************************************/
//void Delay_us(uint16_t delayCnt)
//{
//  delayMicroseconds(delayCnt);
//}
//
///*******************************************************************************
//* Function	 :		Delay_ms																									 
//* Parameter  :		delayCnt																						
//* Returns	 :		void			
//* Description:		Delay_ms(10)	time: 10ms											
//* Note: 	 :																													 
//*******************************************************************************/
//void Delay_ms(uint16_t delayCnt)
//{
//  delay(delayCnt);
//}
//
///*******************************************************************************
//* Function   :     	Init_Gpio	                                                                                                     
//* Parameter  :     	void                                                                                        
//* Returns    :     	void            
//* Description:      all GPIO init .                    		                    
//* Note:      :                                                                                                                   
//*******************************************************************************/
//void Init_Gpio(void)
//{
////	//reset allo gpio ��λGPIO��
////	GPIO_DeInit(GPIOA);
////	GPIO_DeInit(GPIOB);
////	GPIO_DeInit(GPIOC);
////    GPIO_DeInit(GPIOD);
//    
//	//key set: input mode . no interrupt  ����ģʽ�����ն�
////	GPIO_Init(KEY_PORT,KEY,GPIO_Mode_In_FL_No_IT);
//    pinMode(KEY, INPUT);
//
//	//LED set: ouput mode. LED1 LED2 ���ģʽ
//    pinMode(LED1, OUTPUT);
//    pinMode(LED2, OUTPUT);
//
//	//BLE soft_spi. CSN\ SCK\ MOSI_MISO\ :output mode . IRQ :input mode.no interrupt
//	//SPI ���� CSN SCK MOSI_MISO ���ģʽ��IRQ ����ģʽ�����ж�
////    GPIO_Init(BLE_CSN_PORT,BLE_CSN,GPIO_Mode_Out_PP_High_Slow);
////    GPIO_Init(BLE_SCK_PORT,BLE_SCK,GPIO_Mode_Out_PP_High_Slow);
////    GPIO_Init(BLE_MOSI_MISO_PORT,BLE_MOSI_MISO,GPIO_Mode_Out_PP_High_Slow);
////    GPIO_Init(BLE_IRQ_PORT,BLE_IRQ,GPIO_Mode_In_PU_No_IT);
//    pinMode(BLE_CSN,OUTPUT);
//    pinMode(BLE_SCK,OUTPUT);
//    pinMode(BLE_MOSI_MISO,OUTPUT);
//    pinMode(BLE_IRQ,INPUT_PULLUP);
//
//	//PB0 ��������
////    GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_In_PU_No_IT); //PB0 input pull-up
////    GPIO_Init(GPIOD,GPIO_Pin_0,GPIO_Mode_In_PU_No_IT); //PB0 input pull-up
//
////	//RX ��������
////    GPIO_Init(RX_PORT,RX,GPIO_Mode_In_PU_No_IT);
////	//TX ��� 
////    GPIO_Init(TX_PORT,TX,GPIO_Mode_Out_PP_High_Slow);
//
//	//set LED off status  LED1 LED2 �ص�
//	GPIO_ResetBits(LED1_PORT,LED1);
//	GPIO_ResetBits(LED2_PORT,LED2);
//
//	//spi :csn : high  close spi .     sck:low   CSN:�ߵ�ƽ    SCK:�͵�ƽ 
//	BLE_CSN_SET();
//	BLE_SCK_CLR();
//}
///*******************************************************************************
//* Function   :     	Init_clock	                                                                                                     
//* Parameter  :     	void                                                                                        
//* Returns    :     	void            
//* Description:                            		                    
//* Note:      :                                                                                                                   
//*******************************************************************************/
////void Init_Clock(void)
////{
////	//reset system clock  ��λϵͳʱ��
////	CLK_DeInit();
////	//diable clock output ��ʹ��ʱ�����
////	CLK_CCOCmd(DISABLE);
////	//HSI Prescaler 1. HSI ʱ�ӷ�Ƶ1
////	CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv1);
////
////	// TIM2&USART clock enable .ʹ��TIM2 ��USART ʱ��
////	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,ENABLE);
////	CLK_PeripheralClockConfig(CLK_Peripheral_USART,ENABLE);
////}
//
///*******************************************************************************
//* Function   :     	Init_Timer	                                                                                                     
//* Parameter  :     	void                                                                                        
//* Returns    :     	void            
//* Description:                               		                    
//* Note:      :      TIM2                                                                                                              
//*******************************************************************************/
////void Init_Timer(void)
////{
////	//TIM2 reset TIM2 ��λ
////	TIM2_DeInit();
////	
////	//tim2 prescaler_64.CounterMode Down ,  tim2 64��Ƶ��������ģʽ����
////	TIM2_TimeBaseInit(TIM2_Prescaler_64, TIM2_CounterMode_Down,TIM2_1MS_VALUE);
////	//enable tim2 flag .enable interrupt flag  ʹ��TIM2 �ж�
////	TIM2_ITConfig(TIM2_IT_Update, ENABLE);
////	
////    // disable TIM2 
////	//TIM2_Cmd(DISABLE);
////	//TIM2 �ж�ʹ��
////    TIM2_Cmd(ENABLE);
////}
//
///*******************************************************************************
//* Function   :     	Init_System	                                                                                                     
//* Parameter  :     	void                                                                                        
//* Returns    :     	void            
//* Description:                               		                    
//* Note:      :                                                                                                                   
//*******************************************************************************/
//void Init_System(void)
//{
//    //disable all interrupt   ��ʹ��ȫ���ж�
////	disableInterrupts();
//	//��ʼ�� ʱ��
////    Init_Clock();
//	//��ʼ�� GPIO
//	Init_Gpio();
//	//��ʼ�� ��ʱ��
////	Init_Timer();
//	//��ʼ�� UART
//	Init_Uart();
//
//	Uart_Send_String("\r\n");
//	Uart_Send_String("----------------------------------------------------------------------------------\r\n");
//	Uart_Send_String("----------------------------------------------------------------------------------\r\n");
//	Uart_Send_String("----------------------------------------------------------------------------------\r\n");
//	Uart_Send_String("----------------------------------------------------------------------------------\r\n");
//	Uart_Send_String("------------------------Arduino nano for MG127 by liuhao--------------------------\r\n");
//	Uart_Send_String("--------------------------------arduino nano        ------------------------------\r\n");
//	
//	Uart_Send_String("Software Version:V");
//	Uart_Send_Byte(SOFTWARE_MAJOR);
//	Uart_Send_String(".");
//	Uart_Send_Byte(SOFTWARE_MINOR);
//	Uart_Send_String("\r\n");
//
//
//	Uart_Send_String("    ����: ");
//	//C macro
//	Uart_Send_String(__DATE__);
//	Uart_Send_String("         ʱ��: ");
//	//C macro
//	Uart_Send_String(__TIME__);
//	Uart_Send_String("\r\n");
//	Uart_Send_String("\r\n");
//	Uart_Send_String("\r\n");
//
//	//enable all interrupt
////	enableInterrupts();
//}
//


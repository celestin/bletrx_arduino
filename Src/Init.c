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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

//mcu fre:16MHz
#define	SOFTWARE_MAJOR	1
#define	SOFTWARE_MINOR	0



// (1/((1/(16000000/1000))*64))  Downcounter.TIM2  time:1ms
#define TIM2_1MS_VALUE	0x00FD



//mcu fre:16MHz
#define	FREOSC	16000000UL



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/*******************************************************************************
* Function	 :		Delay_us																									 
* Parameter  :		delayCnt																						
* Returns	 :		void			
* Description:		Delay_us(10). delay time: 10us													
* Note: 	 :																												 
*******************************************************************************/
void Delay_us(uint16_t delayCnt)
{
	uint16_t temp0 =0;
	for(temp0 =0; temp0 < delayCnt;temp0++)
	{
        __asm("nop");
        __asm("nop");
        __asm("nop");
        __asm("nop");
        __asm("nop");
        __asm("nop");
	}
}

/*******************************************************************************
* Function	 :		Delay_ms																									 
* Parameter  :		delayCnt																						
* Returns	 :		void			
* Description:		Delay_ms(10)	time: 10ms											
* Note: 	 :																													 
*******************************************************************************/
void Delay_ms(uint16_t delayCnt)
{
	uint16_t temp0 =0;
	for(temp0 =0; temp0 < delayCnt;temp0++)
	{
        Delay_us(1132);
	}
}

/*******************************************************************************
* Function   :     	Init_Gpio	                                                                                                     
* Parameter  :     	void                                                                                        
* Returns    :     	void            
* Description:      all GPIO init .                    		                    
* Note:      :                                                                                                                   
*******************************************************************************/
void Init_Gpio(void)
{
	//reset allo gpio 复位GPIO口
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOD);
    
	//key set: input mode . no interrupt  输入模式。非终端
	GPIO_Init(KEY_PORT,KEY,GPIO_Mode_In_FL_No_IT);

	//LED set: ouput mode. LED1 LED2 输出模式
    GPIO_Init(LED1_PORT,LED1,GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(LED2_PORT,LED2,GPIO_Mode_Out_PP_Low_Slow);

	//BLE soft_spi. CSN\ SCK\ MOSI_MISO\ :output mode . IRQ :input mode.no interrupt
	//SPI 设置 CSN SCK MOSI_MISO 输出模式，IRQ 输入模式，非中断
    GPIO_Init(BLE_CSN_PORT,BLE_CSN,GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(BLE_SCK_PORT,BLE_SCK,GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(BLE_MOSI_MISO_PORT,BLE_MOSI_MISO,GPIO_Mode_Out_PP_High_Slow);
    GPIO_Init(BLE_IRQ_PORT,BLE_IRQ,GPIO_Mode_In_PU_No_IT);

	//PB0 输入上啦
    GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_In_PU_No_IT); //PB0 input pull-up
    GPIO_Init(GPIOD,GPIO_Pin_0,GPIO_Mode_In_PU_No_IT); //PB0 input pull-up

	//RX 输入上拉
    GPIO_Init(RX_PORT,RX,GPIO_Mode_In_PU_No_IT);
	//TX 输出 
    GPIO_Init(TX_PORT,TX,GPIO_Mode_Out_PP_High_Slow);

	//set LED off status  LED1 LED2 关灯
	GPIO_ResetBits(LED1_PORT,LED1);
	GPIO_ResetBits(LED2_PORT,LED2);

	//spi :csn : high  close spi .     sck:low   CSN:高电平    SCK:低电平 
	BLE_CSN_SET();
	BLE_SCK_CLR();
}
/*******************************************************************************
* Function   :     	Init_clock	                                                                                                     
* Parameter  :     	void                                                                                        
* Returns    :     	void            
* Description:                            		                    
* Note:      :                                                                                                                   
*******************************************************************************/
void Init_Clock(void)
{
	//reset system clock  复位系统时钟
	CLK_DeInit();
	//diable clock output 不使能时钟输出
	CLK_CCOCmd(DISABLE);
	//HSI Prescaler 1. HSI 时钟分频1
	CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv1);

	// TIM2&USART clock enable .使能TIM2 和USART 时钟
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,ENABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_USART,ENABLE);
}

/*******************************************************************************
* Function   :     	Init_Timer	                                                                                                     
* Parameter  :     	void                                                                                        
* Returns    :     	void            
* Description:                               		                    
* Note:      :      TIM2                                                                                                              
*******************************************************************************/
void Init_Timer(void)
{
	//TIM2 reset TIM2 复位
	TIM2_DeInit();
	
	//tim2 prescaler_64.CounterMode Down ,  tim2 64分频，计数器模式：下
	TIM2_TimeBaseInit(TIM2_Prescaler_64, TIM2_CounterMode_Down,TIM2_1MS_VALUE);
	//enable tim2 flag .enable interrupt flag  使能TIM2 中断
	TIM2_ITConfig(TIM2_IT_Update, ENABLE);
	
    // disable TIM2 
	//TIM2_Cmd(DISABLE);
	//TIM2 中断使能
    TIM2_Cmd(ENABLE);
}

/*******************************************************************************
* Function   :     	Init_System	                                                                                                     
* Parameter  :     	void                                                                                        
* Returns    :     	void            
* Description:                               		                    
* Note:      :                                                                                                                   
*******************************************************************************/
void Init_System(void)
{
    //disable all interrupt   不使能全局中断
	disableInterrupts();
	//初始化 时钟
    Init_Clock();
	//初始化 GPIO
	Init_Gpio();
	//初始化 定时器
	Init_Timer();
	//初始化 UART
	Init_Uart();

	Uart_Send_String("\r\n");
	Uart_Send_String("----------------------------------------------------------------------------------\r\n");
	Uart_Send_String("----------------------------------------------------------------------------------\r\n");
	Uart_Send_String("                                      蓝牙4.0 测试程序                            \r\n");
	Uart_Send_String("----------------------------------------------------------------------------------\r\n");
	Uart_Send_String("----------------------------------------------------------------------------------\r\n");
	Uart_Send_String("------------------------STM8L10x_StdPeriph_Driver V1.2.1--------------------------\r\n");
	Uart_Send_String("--------------------------------IAR for stm8 V2.20.2------------------------------\r\n");
	
	Uart_Send_String("Software Version:V");
	Uart_Send_Byte(SOFTWARE_MAJOR);
	Uart_Send_String(".");
	Uart_Send_Byte(SOFTWARE_MINOR);
	Uart_Send_String("\r\n");


	Uart_Send_String("    日期: ");
	//C macro
	Uart_Send_String(__DATE__);
	Uart_Send_String("         时间: ");
	//C macro
	Uart_Send_String(__TIME__);
	Uart_Send_String("\r\n");
	Uart_Send_String("\r\n");
	Uart_Send_String("\r\n");

	//enable all interrupt
	enableInterrupts();
}



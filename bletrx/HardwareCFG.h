#ifndef _HARDWARECFG_H_
#define _HARDWARECFG_H_

#define GPIOA 1
#define GPIOB 2
#define GPIOC 3

/*****************************************************
KEY: GPIOC
*****************************************************/
#define KEY_PORT       		GPIOC
#define KEY             	8
#define KEY_GET()       	GPIO_ReadInputDataBit(KEY_PORT,KEY)

/*****************************************************
LED: 
LED1		:PA2  	RED
LED2		:PA3	GREEN
*****************************************************/

#define LED1_PORT    		GPIOA
#define LED2_PORT       GPIOA

#define LED1             	13
#define LED2             	A1

#define	LED_RED_ON()		GPIO_SetBits(LED1_PORT,LED1)
#define	LED_RED_OFF()		GPIO_ResetBits(LED1_PORT,LED1)

#define	LED_GREEN_ON()		GPIO_SetBits(LED2_PORT,LED2)
#define	LED_GREEN_OFF()		GPIO_ResetBits(LED2_PORT,LED2)


/*****************************************************
BLE Hardware SPI:
BLE_CSN       	: GPIOB
BLE_SCK      	: GPIOB
BLE_MOSI_MISO   : GPIOB
BLE_IRQ   	    : GPIOC
*****************************************************/
#define BLE_CSN_PORT    	GPIOB
#define BLE_SCK_PORT    	GPIOB
#define BLE_MOSI_MISO_PORT  GPIOB
#define BLE_IRQ_PORT    	GPIOC

#define BLE_CSN			    12
#define BLE_SCK      		11
#define BLE_MOSI_MISO   	10
#define BLE_IRQ   		    9

#define BLE_CSN_CLR()     	GPIO_ResetBits(BLE_CSN_PORT,BLE_CSN)
#define BLE_CSN_SET()    	GPIO_SetBits(BLE_CSN_PORT,BLE_CSN)

#define BLE_SCK_CLR()     	GPIO_ResetBits(BLE_SCK_PORT,BLE_SCK)
#define BLE_SCK_SET()    	GPIO_SetBits(BLE_SCK_PORT,BLE_SCK)

#define BLE_MOSI_MISO_CLR()	GPIO_ResetBits(BLE_MOSI_MISO_PORT,BLE_MOSI_MISO)
#define BLE_MOSI_MISO_SET()	GPIO_SetBits(BLE_MOSI_MISO_PORT,BLE_MOSI_MISO)

#define BLE_MOSI_MISO_GET()	GPIO_ReadInputDataBit(BLE_MOSI_MISO_PORT,BLE_MOSI_MISO)

#define BLE_IRQ_GET()  		GPIO_ReadInputDataBit(BLE_IRQ_PORT,BLE_IRQ)


/*****************************************************
USART: 
RX		:PC2  	
TX		:PC3	
*****************************************************/

#define RX_PORT    			GPIOC
#define TX_PORT    			GPIOC

#define RX             		GPIO_Pin_2
#define TX             		GPIO_Pin_3


#endif

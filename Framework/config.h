#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "main.h"

#define APP_CRC_ADDR   0x308000

enum USART_CHN{//串口通道
    USART_1=0,USART_2,USART_3,UART_5,USART_6,
    NUM_UARTCHN
};//UART1 UART2 UART3 UART5 UART6

#define NUM_UARTCHANNEL               5  //串口通道总数量
#define SCI_BUF_MAXLEN              256  //串口接收缓冲区最大字节数量

/******************************************************************************************/
#define LED2_ON                         HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
#define LED2_OFF                        HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
#define LED3_ON                         HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
#define LED3_OFF                        HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET);

#define USART1_485_TX_ENABLE            HAL_GPIO_WritePin(USART1_485_EN_GPIO_Port,USART1_485_EN_Pin,GPIO_PIN_SET);
#define USART1_485_RX_ENABLE            HAL_GPIO_WritePin(USART1_485_EN_GPIO_Port,USART1_485_EN_Pin,GPIO_PIN_RESET);
#define UART5_485_TX_ENABLE             HAL_GPIO_WritePin(UART5_485_EN_GPIO_Port,UART5_485_EN_Pin,GPIO_PIN_SET);
#define UART5_485_RX_ENABLE             HAL_GPIO_WritePin(UART5_485_EN_GPIO_Port,UART5_485_EN_Pin,GPIO_PIN_RESET);


//RTU通道配置
#define UART_CHN_POWER_CTRL       						USART_1
#define UART_CHN_AD_SAMPLE               			USART_3
#define RTU_ENABLE_LIST \
{\
    TRUE,FALSE,TRUE,FALSE,FALSE \
}//UART1 UART2 UART3 UART5 UART6




/******************************************************************************************/


#define OS_ENTER_CRITICAL() 								taskENTER_CRITICAL()
#define OS_EXIT_CRITICAL() 									taskEXIT_CRITICAL()

#define ON   														1u
#define OFF 														0u

#define TRUE  													1u
#define FALSE 													0u


#endif



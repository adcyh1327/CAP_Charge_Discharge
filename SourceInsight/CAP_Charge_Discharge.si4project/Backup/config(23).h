#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "main.h"

#define APP_CRC_ADDR   0x308000
#define SER_PDU_SIZE_MAX 500u

enum USART_CHN{//串口和CAN的通道编号
    RS485_1=0,RS232_1,RS232_2,RS232_3,
    NUM_UARTCHN
};//UART1 UART2 UART3 UART5


/******************************************************************************************/
#define LED2_ON             HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
#define LED2_OFF            HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
#define LED3_ON             HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
#define LED3_OFF            HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET);

//RTUͨ������
#define UART_CHN_LCD_DISPLAY       						RS232_2
#define UART_CHN_AD_SAMPLE               			RS232_1
#define RTU_ENABLE_LIST \
{\
    FALSE,FALSE,TRUE,FALSE \
}//UART1 UART2 UART3 UART5




/******************************************************************************************/


#define OS_ENTER_CRITICAL() 								taskENTER_CRITICAL()
#define OS_EXIT_CRITICAL() 									taskEXIT_CRITICAL()

#define ON   														1u
#define OFF 														0u

#define TRUE  													1u
#define FALSE 													0u

#define NUM_UARTCHANNEL               4  //串口总通道
#define SCI_BUF_MAXLEN              256  //串口发送、接收缓冲区的最大长度,根据需要可能会变更






#endif



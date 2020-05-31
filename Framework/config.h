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
#define LED2_SETTING(n)    \
  { if(n==1) {HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);} \
    else{HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);}
#define LED3_ON                         HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
#define LED3_OFF                        HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET);
#define LED3_SETTING(n)    \
  { if(n==1) {HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);} \
    else{HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET);}

    

//MAX5541
#define MAX5541_SCLK_L()                HAL_GPIO_WritePin(CLK_5541_GPIO_Port,CLK_5541_Pin,GPIO_PIN_RESET)
#define MAX5541_SCLK_H()                HAL_GPIO_WritePin(CLK_5541_GPIO_Port,CLK_5541_Pin,GPIO_PIN_SET)
#define MAX5541_MOSI_H()                HAL_GPIO_WritePin(DIN_5541_GPIO_Port,DIN_5541_Pin,GPIO_PIN_RESET)
#define MAX5541_MOSI_L()                HAL_GPIO_WritePin(DIN_5541_GPIO_Port,DIN_5541_Pin,GPIO_PIN_SET)
#define MAX5541_CS_ENABLE()             HAL_GPIO_WritePin(CS1_5541_GPIO_Port,CS1_5541_Pin,GPIO_PIN_RESET)
#define MAX5541_CS_DISABLE()            HAL_GPIO_WritePin(CS1_5541_GPIO_Port,CS1_5541_Pin,GPIO_PIN_SET)


//输入信号
#define READ_ENABLE_STATUS              HAL_GPIO_ReadPin(INPUT_1_GPIO_Port,INPUT_1_Pin) //使能信号
#define READ_START_STATUS               HAL_GPIO_ReadPin(INPUT_2_GPIO_Port,INPUT_2_Pin) //启动信号
  
//输出信号
#define PULSE_OUTPUT

#define DEV_RUNNING_SETTING(n)    \
  { if(n==1) {HAL_GPIO_WritePin(RELAY_1_GPIO_Port,RELAY_1_Pin,GPIO_PIN_SET);} \
    else{HAL_GPIO_WritePin(RELAY_1_GPIO_Port,RELAY_1_Pin,GPIO_PIN_RESET);} //设备运行指示灯
#define DEV_WARNING_SETTING(n)    \
      { if(n==1) {HAL_GPIO_WritePin(RELAY_2_GPIO_Port,RELAY_2_Pin,GPIO_PIN_SET);} \
        else{HAL_GPIO_WritePin(RELAY_2_GPIO_Port,RELAY_2_Pin,GPIO_PIN_RESET);} //设备报警指示灯
#define TEST_PASS_SETTING(n)    \
  { if(n==1) {HAL_GPIO_WritePin(RELAY_3_GPIO_Port,RELAY_3_Pin,GPIO_PIN_SET);} \
    else{HAL_GPIO_WritePin(RELAY_3_GPIO_Port,RELAY_3_Pin,GPIO_PIN_RESET);} //良品信号
#define TEST_FAILED_SETTING(n)    \
  { if(n==1) {HAL_GPIO_WritePin(RELAY_4_GPIO_Port,RELAY_4_Pin,GPIO_PIN_SET);} \
    else{HAL_GPIO_WritePin(RELAY_4_GPIO_Port,RELAY_4_Pin,GPIO_PIN_RESET);} //异常信号

#define USART1_485_TX_ENABLE            HAL_GPIO_WritePin(USART1_485_EN_GPIO_Port,USART1_485_EN_Pin,GPIO_PIN_SET);
#define USART1_485_RX_ENABLE            HAL_GPIO_WritePin(USART1_485_EN_GPIO_Port,USART1_485_EN_Pin,GPIO_PIN_RESET);
#define UART5_485_TX_ENABLE             HAL_GPIO_WritePin(UART5_485_EN_GPIO_Port,UART5_485_EN_Pin,GPIO_PIN_SET);
#define UART5_485_RX_ENABLE             HAL_GPIO_WritePin(UART5_485_EN_GPIO_Port,UART5_485_EN_Pin,GPIO_PIN_RESET);


//RTU通道配置
#define UART_CHN_POWER_CTRL               		UART_5      //直流电源通讯通道

//自定义协议
#define UART_CHN_LCD_DISPLAY       						USART_3     //7 inch显示屏通讯通道
#define UART_CHN_AD_SAMPLE               			USART_6     //高频采样板通讯通道
#define UART_CHN_PC_COMM               			  USART_1     //上位机通讯通道


#define RTU_ENABLE_LIST \
{\
    FALSE,FALSE,FALSE,TRUE,FALSE \
}//UART1 UART2 UART3 UART5 UART6




/******************************************************************************************/


#define OS_ENTER_CRITICAL() 								taskENTER_CRITICAL()
#define OS_EXIT_CRITICAL() 									taskEXIT_CRITICAL()

#define ON   														1u
#define OFF 														0u

#define TRUE  													1u
#define FALSE 													0u


#endif



#ifndef __CONFIG_H_
#define __CONFIG_H_

#define APP_CRC_ADDR   0x308000
#define SER_PDU_SIZE_MAX 500u



#define OS_ENTER_CRITICAL() 								taskENTER_CRITICAL()
#define OS_EXIT_CRITICAL() 									taskEXIT_CRITICAL()

#define ON   														1u
#define OFF 														0u

#define TRUE  													1u
#define FALSE 													0u

#define NUM_UARTCHANNEL               4  //串口总通道
#define SCI_BUF_MAXLEN              256  //串口发送、接收缓冲区的最大长度,根据需要可能会变更

enum USART_CHN{//串口和CAN的通道编号
    RS485_1=0,RS232_1,RS232_2,RS232_3,
    NUM_UARTCHN
};




#endif



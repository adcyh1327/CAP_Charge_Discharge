#ifndef __DR_UART_H
#define __DR_UART_H

#include "stm32f2xx_hal.h"
#include "interface.h"

#define QUE_WAIT_TIME                    50u

typedef struct USARTCAN_Recv_info
{
		uint8_t newupd;//数据更新标志位
    uint16_t lenth; //字节数量
    uint8_t databuf[SCI_BUF_MAXLEN];//有效数据
}USARTCHN_Recv_t;
extern USARTCHN_Recv_t USARTCHN_Recv[NUM_UARTCHN];

typedef struct Interface_Info_t
{
    uint8_t Usart[NUM_UARTCHANNEL][uartcfgnum];  //串口配置数组
    const struct ProtType_t UsartProt[NUM_UARTCHANNEL];//串口帧头帧尾配置
    uint32_t   addr;//modbus起始地址
    uint32_t   datalen;//每个通道mb的长度，实际长度为此值-2，因为有两个字为协议内容，一个为标志位，第二个为字节数量
    uint8_t  	sid;//站地址
}Interface_Info;
extern Interface_Info UsartCHN_Data;//


#if 0
void USART1_IRQ(unsigned char data);
void USART2_IRQ(unsigned char data);
void USART3_IRQ(unsigned char data);
void UART4_IRQ(unsigned char data);
void UART5_IRQ(unsigned char data);

void USART1_Send_Data(uint8_t *send_buff,uint16_t length);
void USART2_Send_Data(uint8_t *send_buff,uint16_t length);
void USART3_Send_Data(uint8_t *send_buff,uint16_t length);
void UART4_Send_Data(uint8_t *send_buff,uint16_t length);
void UART5_Send_Data(uint8_t *send_buff,uint16_t length);

#endif

void FM_Usart_Init(void);
void USART_Timer100us(void);
void UsartRecieveData(uint8_t channel,uint8_t recdata);
void USART3_Send_Data(uint8_t *send_buff,uint16_t length);
void UART5_Send_Data(uint8_t *send_buff,uint16_t length);




#endif



#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include "stm32f2xx_hal.h"
#include "config.h"

struct UART_Conf_Data {
    uint32_t baudrate;
    uint16_t slave_addr;
    uint16_t checksum;
};

typedef union {
    uint8_t Byte;
    struct {
        uint8_t  bit0              :1;
        uint8_t  bit1              :1;
        uint8_t  bit2              :1;
				uint8_t  bit3              :1;
        uint8_t  bit4              :1;
				uint8_t  bit5              :1;
        uint8_t  bit6              :1;
				uint8_t  bit7              :1;
    }Bits;
}Tdef_Byte;


enum UsartType{//串口配置数组各元素的定义，有增加项时在下边第一行往后增加
    EnUart=0,uartBaudrate,Databits,Chkbits,Stopbits,Flowctrl,uartDatatype,tmout,
    uartcfgnum
};


typedef union {
  uint16_t T_byte;
  struct {
		uint8_t  btn              :4;   //串口帧头或帧尾的字节个数
		uint8_t  bit4             :1;
		uint8_t  bit5             :1;
		uint8_t  bit6             :1;
		uint8_t  en               :1;   //串口帧头或帧尾标志
    uint16_t reserve      :8;   
	} Bits;
}Tdef_Prot;
#define FrameStartEn                  0x80u  //帧头使能宏
#define FrameEndEn                    0x80u  //帧尾使能宏
enum numbyte{
  byte_1=1,byte_2,byte_3 ,byte_4 ,byte_5 ,byte_6 ,byte_7 ,byte_8 ,byte_9,byte_10     //帧头或帧尾的字节数量
};
enum checksum_t{//校验和方式
    CheckSum_None=0,ChkSum_And,ChkSum_Crc16
};
/*********************************************************************************************/ 
/*********************************************************************************************/ 
/*********************************************************************************************/ 

struct ProtType_t
{
    Tdef_Prot FrameStartInfo;//最高位表示是否使能，低4位表示字节数，只有使能了字节数才有效
    uint8_t FrameStart[8];        //帧开始符字节数
    Tdef_Prot FrameEndInfo; //最高位表示是否使能，低4位表示字节数，只有使能了字节数才有效
    uint8_t FrameEnd[8];         //帧结束符字节数
    uint8_t checksum;          //校验(0-无，1-和，2-crc16)
    uint8_t inteval;           //字符间隔时间，单位100us
};



extern const uint8_t auchCRC16_Hi[256];
extern const uint8_t auchCRC16_Low[256];

typedef struct UartOpFuncTyp
{
	void (*_send)  (uint8_t *sendbuf, uint16_t lenth);
	void (*_recv)  (uint8_t channel, uint8_t data);
}UartOpFunc_t;
extern UartOpFunc_t UartOpFunc[NUM_UARTCHANNEL];


extern uint16_t g_u16_TCPIPsendlen;           //tcpip报文发送长度

extern char platform_version[];

uint16_t Get_rtuCrc16(uint8_t *puchMsg,uint16_t usDataLen);
unsigned char AscToHex(unsigned char aChar);
unsigned char HexToAsc(unsigned char aHex);



#endif



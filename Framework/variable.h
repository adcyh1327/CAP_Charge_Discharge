//#ifndef __VARIABLE_H_
//#define __VARIABLE_H_

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
}BitStatus;


enum UsartType{//串口配置数组各元素的定义，有增加项时请在下边第一行往后增加
    EnUart=0,uartBaudrate,Databits,Chkbits,Stopbits,Flowctrl,uartDatatype,tmout,
    uartcfgnum
};

enum CANType{//CAN的配置数组各元素定义，有增加向时请在下边第一行往后增加
    EnCAN=0,canBaudrate,LocalID,DeviceID,IDNum,canDatatype,
    cancfgnum
};

enum DataType_t{//数据类型，有增加向时请在下边第一行往后增加
    t_HEX=0,t_ASCII,t_BOOL,
    t_typemax
};
extern char DataType[t_typemax][20];

#define NUM_UARTCHANNEL               5  //串口总通道
#define SCI_BUF_MAXLEN              256  //串口发送、接收缓冲区的最大长度,根据需要可能会变更

enum USARTCAN_CHN{//串口和CAN的通道编号
    RS232_1=0,RS485_1,RS485_2,RS485_3,RS485_4,CAN_CHN,
    NUM_UARTCAN
};

/*********************************************************************************************/   
/*********************************************************************************************/ 
/*********************************************************************************************/    
typedef union {
  uint16_t T_byte;
  struct {
		uint8_t  btn              :4;   //串口帧头或帧尾的字节个数
		uint8_t  bit4             :1;
		uint8_t  bit5             :1;
		uint8_t  bit6             :1;
		uint8_t  en               :1;   //串口帧头或帧尾标志
        uint16_t reserve          :8;   
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

typedef struct USARTCAN_Recv_info
{
	uint8_t newupd;//数据更新标志位
    uint16_t lenth; //字节数量
    uint8_t datatype;//数据类型
    uint8_t databuf[SCI_BUF_MAXLEN];//有效数据
}USARTCAN_Recv_t;
extern USARTCAN_Recv_t USARTCAN_Recv[NUM_UARTCAN];

extern volatile BitStatus Keyboard_Status;
#define CMD_Download_LocalCfg               Keyboard_Status.Bits.bit0
#define CMD_ParaDownload_Independent        Keyboard_Status.Bits.bit1


extern uint16_t g_u16_TCPIPsendlen;           //tcpip报文发送长度

extern char platform_version[];
extern char funcTion[];
extern const uint32_t RS232_baud[12] ;
extern const uint16_t RS232_lenth[2] ;
extern const uint16_t RS232_stop[2] ;
extern const uint16_t RS232_parity[3] ;
extern const uint16_t RS232_FlowCntl[4] ;

uint16_t Get_rtuCrc16(uint8_t *puchMsg,uint16_t usDataLen);
USARTCAN_Recv_t GET_UsartCAN_Recv_Result(uint8_t chanel);
unsigned char AscToHex(unsigned char aChar);
unsigned char HexToAsc(unsigned char aHex);

float GET_ADC_Result(uint8_t chanel);




//#endif



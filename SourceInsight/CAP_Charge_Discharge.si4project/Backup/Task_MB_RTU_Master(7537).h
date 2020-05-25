#ifndef MBRTU_M_H
#define MBRTU_M_H			  

#include <stdint.h>
#include "t_list.h"
#include "t_list_impl.h"
#include "interface.h"

#define RTU_SEND_LENGTH             120 * 2          //发送缓冲区大小
#define RTU_RECE_LENGTH             120 * 2        //接受缓冲区大小

//RTU通道配置
#define UART_CHN_LCD_DISPLAY       						RS232_2
#define UART_CHN_AD_SAMPLE               			RS232_1
#define RTU_ENABLE_LIST \
{\
    FALSE,FALSE,TRUE,FALSE \
}//UART1 UART2 UART3 UART5

//RTU时间参数配置
//参数1：RTU周期发送请求帧的最小间隔时间
//参数2：RTU请求超时时间，超过该时间认为从节点未反应
#define TIMER_PARAMETR_LIST \
{\
    {1000,2000},\
    {20,1000},\
    {20,1000},\
    {20,1000},\
}


enum RTU_Add_Type_t
{
	Add_New,Add_Cycle
};

enum RTU_FSM_t
{
	RTU_REQ,RTU_WAITRESP
};

enum RTU_Result_t
{
    EXCUTE_IDLE=0,EXCUTE_SUCCESS=1,EXCUTE_START=2,EXCUTE_FAIL=0x81
};

enum RTU_TYPE_t
{
    READ,WRITE
};

typedef enum MB_FUNC_TYPE{
	FUNC_RD_COILSTATUS=0X01,
	FUNC_RD_INPUTSTATUS,
	FUNC_RD_HOLDREG,
	FUNC_RD_INREG,
	FUNC_WR_SGCOIL,
	FUNC_WR_SGREG,
	FUNC_WR_SGREG_FREQ,
	FUNC_WR_MULCOIL=0x0F,
	FUNC_WR_MULREG,
	FUNC_CFG_REG='m',
	CFG_RESP_POS=0x55,
	CFG_RESP_NEG=0xFF,
	MB_RESP_NEG=0x80
}MB_FUNC_TYPE_t;

enum MB_TCP_ErrorStatusTyp{
  Sta_OK=0,
  Err_FunCode,
  Err_DataAddr,
  Err_DataValue,
  Err_MBcmd,
  Err_Busy,
  Err_CfgUnmatch=0x0A, 
  Err_FlashWrFail, 
};

enum STATUS_EVENT_t
{
	EV_NONE,EV_REQ,EV_TO,EV_RX_OK,EV_RX_CRC_ERR
};

enum PPI_FSM {
    FSM_REQ,
    FSM_WAIT_ACK,
    FSM_FETCH,
    FSM_WAIT_DATA
};

typedef struct RTU_ReqBlock {
	struct list_head Entry;
	uint8_t Excute_Num;   //执行次数，若设置为0，则表示永久执行
  enum USART_CHN chnindex;  //操作通道
	uint8_t sta_addr;          //站地址
	MB_FUNC_TYPE_t FuncCode;  //功能码
	uint8_t Status;            //执行状态
	uint16_t RegAddr;          //需要操作的首地址
	uint8_t  RegNum;           //操作的寄存器数量
	uint16_t* mappedBuff;      //存放读寄存器的结果或写寄存器的值
}RTU_ReqBlock_t;
extern RTU_ReqBlock_t RTU_Req_Read00; //RTU寄存器请求块


struct RTU_Ctx {
	volatile enum STATUS_EVENT_t event;
	volatile enum RTU_FSM_t fsm_state;
	volatile enum RTU_FSM_t fsm_next_state;
	struct list_head head;
	struct RTU_ReqBlock* curr;//当前的请求块
	uint8_t OP_Mode;
	volatile uint16_t TOtimer;//超时定时器
	volatile uint16_t Polltimer;//轮询定时器
	uint8_t framestart;//帧起始标志
	volatile enum STATUS_EVENT_t Pollevent;//轮询状态
	uint16_t guard_time;//超时时间
	uint16_t poll_interval;//轮询时间
	uint16_t txindex; //发送缓冲区元素编号，从0开始累加，最大值即为发送长度
	uint8_t  txbuff[RTU_SEND_LENGTH];//发送缓冲区
	uint8_t  rxindex;//接收缓冲区元素编号，从0开始累加，最大值即为发送长度
	uint8_t  rxbuff[RTU_RECE_LENGTH];//接收缓冲区
};
extern struct RTU_Ctx rtu_ctx[NUM_UARTCHANNEL];

void RTU_Init(uint8_t chn);
void RTU_AddReqBlock_Front(struct RTU_Ctx* _rtuctx, struct RTU_ReqBlock* _req);
void RTU_AddReqBlock(struct RTU_Ctx* _rtuctx, struct RTU_ReqBlock* _req);
void RTU_Read(struct RTU_Ctx* _rtuctx);
void RTU_Write(struct RTU_Ctx* _rtuctx);
void RTU_CyclicTask(uint8_t chn);
void UART_RTU_Recv(uint8_t chn,uint8_t     l_u8ReceData);
void RTU_Timer1ms_Handler(void);
void MBRTU_Master_MainFunction(void *p_arg);

#endif

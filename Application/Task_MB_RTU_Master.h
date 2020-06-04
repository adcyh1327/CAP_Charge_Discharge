#ifndef MBRTU_M_H
#define MBRTU_M_H			  

#include <stdint.h>
#include "t_list.h"
#include "t_list_impl.h"
#include "interface.h"

#define RTU_SEND_LENGTH             120 * 2          //���ͻ�������С
#define RTU_RECE_LENGTH             120 * 2        //���ܻ�������С

#define RTU_ENABLE_LIST \
{\
    FALSE,FALSE,TRUE,FALSE \
}//UART1 UART2 UART3 UART5

//RTUʱ���������
//����1��RTU���ڷ�������֡����С���ʱ��
//����2��RTU����ʱʱ�䣬������ʱ����Ϊ�ӽڵ�δ��Ӧ
#define TIMER_PARAMETR_LIST \
{\
    {1000,2000},\
    {20,1000},\
    {20,1000},\
    {20,1000},\
    {20,1000},\
}//UART1 UART2 UART3 UART5 UART6


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

enum RTU_FSM {
    FSM_REQ,
    FSM_WAIT_ACK,
    FSM_FETCH,
    FSM_WAIT_DATA
};

typedef struct RTU_ReqBlock {
	struct list_head Entry;
	uint8_t Excute_Num;   //ִ�д�����������Ϊ0�����ʾ����ִ��
  enum USART_CHN chnindex;  //����ͨ��
	uint8_t sta_addr;          //վ��ַ
	MB_FUNC_TYPE_t FuncCode;  //������
	uint8_t Status;            //ִ��״̬
	uint16_t RegAddr;          //��Ҫ�������׵�ַ
	uint8_t  RegNum;           //�����ļĴ�������
	uint16_t* mappedBuff;      //��Ŷ��Ĵ����Ľ����д�Ĵ�����ֵ
}RTU_ReqBlock_t;
extern RTU_ReqBlock_t RTU_Req_Read00; //RTU�Ĵ��������


struct RTU_Ctx {
	volatile enum STATUS_EVENT_t event;
	volatile enum RTU_FSM_t fsm_state;
	volatile enum RTU_FSM_t fsm_next_state;
	struct list_head head;
	struct RTU_ReqBlock* curr;//��ǰ�������
	uint8_t OP_Mode;
	volatile uint16_t TOtimer;//��ʱ��ʱ��
	volatile uint16_t Polltimer;//��ѯ��ʱ��
	uint8_t framestart;//֡��ʼ��־
	volatile enum STATUS_EVENT_t Pollevent;//��ѯ״̬
	uint16_t guard_time;//��ʱʱ��
	uint16_t poll_interval;//��ѯʱ��
	uint16_t txindex; //���ͻ�����Ԫ�ر�ţ���0��ʼ�ۼӣ����ֵ��Ϊ���ͳ���
	uint8_t  txbuff[RTU_SEND_LENGTH];//���ͻ�����
	uint8_t  rxindex;//���ջ�����Ԫ�ر�ţ���0��ʼ�ۼӣ����ֵ��Ϊ���ͳ���
	uint8_t  rxbuff[RTU_RECE_LENGTH];//���ջ�����
};
extern struct RTU_Ctx rtu_ctx[NUM_UARTCHANNEL];

uint16_t Get_rtuCrc16(uint8_t *puchMsg,uint16_t usDataLen);
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

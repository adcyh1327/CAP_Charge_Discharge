#include "config.h"
#include "cmsis_os.h"
#include "stm32f2xx.h"                  // Device header
#include "string.h"
#include "Task_MB_RTU_Master.h"

extern uint8_t RTU_CHN_ENABLE[NUM_UARTCHANNEL];
struct Time_Parameter_t
{
    uint16_t interval;
    uint16_t timerout;
};
struct Time_Parameter_t Time_Parameter[NUM_UARTCHANNEL] = TIMER_PARAMETR_LIST;

struct RTU_Ctx rtu_ctx[NUM_UARTCHANNEL];//变量定义

uint8_t RTU_Active[NUM_UARTCHANNEL]={OFF,OFF,OFF,OFF,OFF};
static uint16_t CommErrCnt[NUM_UARTCHANNEL]; //通讯错误计数器

static volatile Tdef_Prot    	        	_RTUSystemStatus[NUM_UARTCHANNEL];
#define SystemStatus(n)                 _RTUSystemStatus[n].Byte
#define Manual_Req(n)                   _RTUSystemStatus[n].Bits.bit0

//CRC高位字节值表 
static const uint8_t auchCRC16_Hi[256] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
}; 
//CRC 低位字节值表 
static const uint8_t auchCRC16_Lo[256]={ 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
}; 


void UART_RTU_Recv(uint8_t channel, uint8_t l_u8ReceData);

//======================================================================
//获得CRC16值 
//puchMsg:要校验的数组 
//usDataLen:数组长度 
//======================================================================
uint16_t Get_rtuCrc16(uint8_t *puchMsg,uint16_t usDataLen) 
{ 
    uint8_t uchCRCHi=0xFF; 		//高CRC 字节初始化 
    uint8_t uchCRCLo=0xFF; 		//低CRC 字节初始化  
    uint32_t uIndex; 	 		//CRC 循环中的索引 
  	while(usDataLen--) 	//传输消息缓冲区 
  	{ 
  		uIndex=uchCRCHi^*puchMsg++; //计算CRC  
  		uchCRCHi=uchCRCLo^auchCRC16_Hi[uIndex]; 
  		uchCRCLo=auchCRC16_Lo[uIndex]; 
  	} 
  	return (uchCRCHi<<8|uchCRCLo); 
}


/********************************************************************************/
/*函数名：  Record_ErrCnt                                               */
/*功能说明：错误计数                                                */
/*输入参数：无                                         */
/*输出参数：无                                                                   */
/********************************************************************************/
void Record_ErrCnt(uint8_t chn)
{
    if(CommErrCnt[chn] < 0xFFFE)
    {
        CommErrCnt[chn]++;
    }
}

/********************************************************************************/
/*函数名：  RTU_AddReqBlock_Front                                               */
/*功能说明：将新请求加到队列的头部                                                */
/*输入参数：_rtuctx-目标队列，_req-新请求                                         */
/*输出参数：无                                                                   */
/********************************************************************************/
void RTU_AddReqBlock_Front(struct RTU_Ctx* _rtuctx, struct RTU_ReqBlock* _req)
{//将当前请求放到消息队列的首位
    struct list_head* p;
    p = &_rtuctx->head;
    do
    {
        if(p == _req->Entry.next)
        {
            return;
        }
        p = p->next;
    }
    while(p != &_rtuctx->head);
	__disable_irq();
    list_add(&_req->Entry, &_rtuctx->head);
	__enable_irq();
	//_rtuctx->event=EV_REQ ;
}

/********************************************************************************/
/*函数名：  RTU_AddReqBlock                                                      */
/*功能说明：将新请求加到队列的尾部                                                */
/*输入参数：_rtuctx-目标队列，_req-新请求                                         */
/*输出参数：无                                                                   */
/********************************************************************************/
void RTU_AddReqBlock(struct RTU_Ctx* _rtuctx, struct RTU_ReqBlock* _req)
{//将当前请求加入到队列
    struct list_head* p;
    p = &_rtuctx->head;
    do
    {
        if(p == _req->Entry.next)
        {
            return;
        }
        p = p->next;
    }
    while(p != &_rtuctx->head);
		OS_ENTER_CRITICAL();
    _req->Status = EXCUTE_IDLE;
		list_add_tail(&_req->Entry, &_rtuctx->head);
		OS_EXIT_CRITICAL();
    //_rtuctx->event=EV_REQ ;
}

/********************************************************************************/
/*函数名：  RTU_DelReqBlock                                                      */
/*功能说明：删除队列的第一个元素                                                  */
/*输入参数：_rtuctx-目标队列                                                     */
/*输出参数：删除第一个成员后的最前边一个成员地址                                   */
/********************************************************************************/
static struct RTU_ReqBlock* RTU_DelReqBlock(struct RTU_Ctx* _rtu_ctx)
{//删除消息队列的首个请求
    struct RTU_ReqBlock* req = 0;
    OS_ENTER_CRITICAL();
    if (!list_empty(&_rtu_ctx->head))
    {
        req = (struct RTU_ReqBlock*)_rtu_ctx->head.next;
        if (req)
        {
            list_del(_rtu_ctx->head.next);
        }
    }
    OS_EXIT_CRITICAL();
    return req;
}

/********************************************************************************/
/*函数名：  BSP_RTU_StartSend                                                   */
/*功能说明：调用BSP发送数据                                                      */
/*输入参数：_rtuctx-目标队列                                                     */
/*输出参数：无                                                                   */
/********************************************************************************/
static void BSP_RTU_StartSend(struct RTU_Ctx* _rturtx)
{//启动串口发送
	UartOpFunc[_rturtx->curr->chnindex]._send(_rturtx->txbuff,_rturtx->txindex);
	_rturtx->txindex=0;
	_rturtx->TOtimer = _rturtx->guard_time;
	_rturtx->rxindex = 0;
}
	
/********************************************************************************/
/*函数名：  RTU_Init                                                             */
/*功能说明：模块初始化函数                                                        */
/*输入参数：chn-初始化通道号*/
/*输出参数：无                                                                   */
/********************************************************************************/
void RTU_Init(uint8_t chn)
{//模块初始化
	//memset(&rtu_ctx, 0, sizeof(rtu_ctx));
	rtu_ctx[chn].fsm_state = RTU_REQ;//初始化成请求模式
	rtu_ctx[chn].guard_time = Time_Parameter[chn].timerout;//请求的最大允许响应时间，否则为超时
	rtu_ctx[chn].poll_interval=Time_Parameter[chn].interval;//连续两个请求间隔时间
	INIT_LIST_HEAD(&rtu_ctx[chn].head);
	RTU_Active[chn] = ON;
	rtu_ctx[chn].TOtimer = rtu_ctx[chn].poll_interval;
	rtu_ctx[chn].event = EV_TO;
	rtu_ctx[chn].Pollevent = EV_NONE;
	UartOpFunc[chn]._recv=UART_RTU_Recv;
	memset(CommErrCnt,0,NUM_UARTCHANNEL);
}

/********************************************************************************/
/*函数名：  RTU_HandleReply                                                     */
/*功能说明：接收数据解析                                                         */
/*输入参数：_rtuctx-目标队列                                                     */
/*输出参数：无                                                                   */
/********************************************************************************/
void RTU_HandleReply(struct RTU_Ctx* _rtuctx)
{
	uint8_t i,j;
	_rtuctx->TOtimer=0;
	if((_rtuctx->rxbuff[1]&MB_RESP_NEG)!=MB_RESP_NEG)
	{
		_rtuctx->curr->Status = EXCUTE_SUCCESS;
	}
	else
	{
		_rtuctx->curr->Status = EXCUTE_FAIL;
		Record_ErrCnt(_rtuctx->curr->chnindex);//记录一次错误
		//Error_Indicator(80);  //LED亮80ms后灭
	}
	if(_rtuctx->OP_Mode==READ)
	{
		OS_ENTER_CRITICAL();
		if((_rtuctx->curr->FuncCode == FUNC_RD_COILSTATUS)||(_rtuctx->curr->FuncCode == FUNC_RD_INPUTSTATUS))
		{
			for(i=0;i<_rtuctx->rxbuff[2];i++)
			{
				for(j=0;j<8;j++)
				{
					_rtuctx->curr->mappedBuff[(8*2*i)+(2*j)]=0x00 + (_rtuctx->rxbuff[3+i]>>j)&0x01;
				}
			}
		}
		else
		{
			for(j=0;j<_rtuctx->rxbuff[2];j=j+2)
			{
				_rtuctx->curr->mappedBuff[j/2] = (_rtuctx->rxbuff[3+j]<<8) + _rtuctx->rxbuff[3+j+1];
			}
		}
		OS_EXIT_CRITICAL();
	}
	memset(_rtuctx->rxbuff,0,_rtuctx->rxindex+1);
	_rtuctx->rxindex=0;
}

/********************************************************************************/
/*函数名：  RTU_Timer1ms_Handler                                                 */
/*功能说明：超时状态管理                                                          */
/*输入参数：无                                                                   */
/*输出参数：无                                                                   */
/********************************************************************************/
void RTU_Timer1ms_Handler(void)
{
    uint8_t i;
    for(i=0;i<NUM_UARTCHANNEL;i++)
    {
        if(RTU_CHN_ENABLE[i] == TRUE)
        {
            if(RTU_Active[i] == ON)
            {
        		if((rtu_ctx[i].TOtimer>0)&&(!--rtu_ctx[i].TOtimer))
        		{
        			rtu_ctx[i].event = EV_TO;
        		}
        		
        		if(rtu_ctx[i].fsm_state == FSM_REQ)
        		{
        			if((rtu_ctx[i].Polltimer>0)&&(!--rtu_ctx[i].Polltimer))
        			{
        				rtu_ctx[i].Pollevent = EV_TO;
        			}
        		}
        		else
        		{
        			//rtu_ctx.Polltimer=0;
        		}
            }
        }
    }
}

/********************************************************************************/
/*函数名：  UART_RTU_Recv                                                        */
/*功能说明：接收到的数据解析，判断每个字节是否有效，若无效则抛弃该帧数据             */
/*输入参数：l_u8ReceData，当前接收的数据                                          */
/*输出参数：无                                                                   */
/********************************************************************************/
void UART_RTU_Recv(uint8_t chn, uint8_t l_u8ReceData)
{
	uint16_t CrcCheck;
	
	//接收
	if((rtu_ctx[chn].framestart==OFF)&&(l_u8ReceData==rtu_ctx[chn].curr->sta_addr))
	{
		rtu_ctx[chn].framestart=ON;
		rtu_ctx[chn].rxindex=0;
		rtu_ctx[chn].rxbuff[rtu_ctx[chn].rxindex]=l_u8ReceData;
	}
	
	if(rtu_ctx[chn].framestart==ON)
	{
		rtu_ctx[chn].rxbuff[rtu_ctx[chn].rxindex]=l_u8ReceData;
	}
	
	if(((rtu_ctx[chn].rxbuff[2]+5)==(rtu_ctx[chn].rxindex+1))&&(rtu_ctx[chn].OP_Mode==READ))
	{
		CrcCheck=Get_rtuCrc16(rtu_ctx[chn].rxbuff,rtu_ctx[chn].rxbuff[2]+3);
		if(((CrcCheck>>8) == rtu_ctx[chn].rxbuff[rtu_ctx[chn].rxindex-1]) && ((CrcCheck%256) == rtu_ctx[chn].rxbuff[rtu_ctx[chn].rxindex]))
		{
			rtu_ctx[chn].framestart=OFF;
			rtu_ctx[chn].event = EV_RX_OK;
		}
		else
		{
			rtu_ctx[chn].event = EV_RX_CRC_ERR;
		}
	}
	else if((rtu_ctx[chn].rxindex==7)&&(rtu_ctx[chn].OP_Mode==WRITE))
	{
		rtu_ctx[chn].framestart=OFF;
		rtu_ctx[chn].event = EV_RX_OK;
	}
	else
	{
		
	}
	rtu_ctx[chn].rxindex++;
}

/********************************************************************************/
/*函数名：  RTU_Read                                                             */
/*功能说明：读请求数据打包                                                        */
/*输入参数：_rtuctx，当前队列数据信息                                             */
/*输出参数：无                                                                   */
/********************************************************************************/
void RTU_Read(struct RTU_Ctx* _rtuctx)
{
    uint16_t CrcCheck;
    _rtuctx->OP_Mode=READ;
    _rtuctx->curr->FuncCode =(_rtuctx->curr->FuncCode==(MB_FUNC_TYPE_t)0)? (MB_FUNC_TYPE_t)0x03 : _rtuctx->curr->FuncCode ;
    _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->sta_addr;
    _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->FuncCode;
    _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->RegAddr/256;
    _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->RegAddr%256;
    _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->RegNum/256;
    _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->RegNum%256;
    CrcCheck = Get_rtuCrc16(_rtuctx->txbuff,_rtuctx->txindex);
    _rtuctx->txbuff[_rtuctx->txindex++] =  CrcCheck>>8;
    _rtuctx->txbuff[_rtuctx->txindex++] =  CrcCheck;
    BSP_RTU_StartSend(_rtuctx);
}

/********************************************************************************/
/*函数名：  RTU_Write                                                            */
/*功能说明：写请求数据打包                                                        */
/*输入参数：_rtuctx，当前队列数据信息                                             */
/*输出参数：无                                                                   */
/********************************************************************************/
void RTU_Write(struct RTU_Ctx* _rtuctx)
{
    uint16_t CrcCheck;
    uint8_t temp,i;
    _rtuctx->OP_Mode=WRITE;
    _rtuctx->curr->FuncCode =(_rtuctx->curr->FuncCode==0u)? (MB_FUNC_TYPE_t)0x10 : _rtuctx->curr->FuncCode ;
    _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->sta_addr;
    _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->FuncCode;
    _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->RegAddr/256;
    _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->RegAddr%256;
    if(_rtuctx->curr->FuncCode == FUNC_WR_SGCOIL)
    {
        temp=2;
        _rtuctx->curr->mappedBuff[0] = (_rtuctx->curr->mappedBuff[0]==0x01)?0xff00:0x0000;
    }
    else if(_rtuctx->curr->FuncCode == FUNC_WR_MULCOIL)
    {
        temp = (_rtuctx->curr->RegNum/8)+((_rtuctx->curr->RegNum%8)==0?0:1);
    }
    else if(_rtuctx->curr->FuncCode == FUNC_WR_MULREG)
    {
        _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->RegNum/256;
        _rtuctx->txbuff[_rtuctx->txindex++] = _rtuctx->curr->RegNum%256;
        temp = _rtuctx->curr->RegNum * 2;
        _rtuctx->txbuff[_rtuctx->txindex++] = temp;
    }
    else
    {
        temp = _rtuctx->curr->RegNum * 2;
    }
    for(i=0;i<temp;i++)
    {
        if(i%2 == 0)
        {
            _rtuctx->txbuff[_rtuctx->txindex++]=_rtuctx->curr->mappedBuff[i/2]>>8;
        }
        else
        {
            _rtuctx->txbuff[_rtuctx->txindex++]=_rtuctx->curr->mappedBuff[i/2]&0xff;
        }
    }
    CrcCheck = Get_rtuCrc16(_rtuctx->txbuff,_rtuctx->txindex);
    _rtuctx->txbuff[_rtuctx->txindex++] =  CrcCheck>>8;
    _rtuctx->txbuff[_rtuctx->txindex++] =  CrcCheck;
    BSP_RTU_StartSend(_rtuctx);
}

/********************************************************************************/
/*函数名：  RTU_CyclicTask                                                      */
/*功能说明：RTU状态机管理                                                        */
/*输入参数：无                                                                  */
/*输出参数：无                                                                  */
/*******************************************************************************/
void RTU_CyclicTask(uint8_t chn)
{
    if (rtu_ctx[chn].event == EV_NONE)
    {//空闲状态
        return;
    }
	
		switch (rtu_ctx[chn].fsm_state)//有超时事件或者有新block请求
    { 
        case RTU_REQ://请求发送状态
            rtu_ctx[chn].curr = RTU_DelReqBlock(&rtu_ctx[chn]);
            if (rtu_ctx[chn].curr == NULL)
            {
                return;
            }
            rtu_ctx[chn].curr->Status=EXCUTE_START; //正在处理中
            if ((rtu_ctx[chn].curr->FuncCode == FUNC_RD_COILSTATUS)||(rtu_ctx[chn].curr->FuncCode == FUNC_RD_INPUTSTATUS)||
                (rtu_ctx[chn].curr->FuncCode == FUNC_RD_HOLDREG)||(rtu_ctx[chn].curr->FuncCode == FUNC_RD_INREG))
            {//读寄存器

                RTU_Read(&rtu_ctx[chn]);
            }
            else if((rtu_ctx[chn].curr->FuncCode == FUNC_WR_SGCOIL)||(rtu_ctx[chn].curr->FuncCode == FUNC_WR_SGREG)||(rtu_ctx[chn].curr->FuncCode == FUNC_WR_SGREG_FREQ)||
                (rtu_ctx[chn].curr->FuncCode == FUNC_WR_MULCOIL)||(rtu_ctx[chn].curr->FuncCode == FUNC_WR_MULREG))
            {//写寄存器
                RTU_Write(&rtu_ctx[chn]);
            }
            else
            {

            }
            rtu_ctx[chn].TOtimer = rtu_ctx[chn].guard_time;//每次发送请求时设置超时时间
            rtu_ctx[chn].fsm_state = RTU_WAITRESP;
            break;
    case RTU_WAITRESP:
        if(rtu_ctx[chn].event == EV_TO)
        {//发送的命令没有收到响应
            rtu_ctx[chn].fsm_state = RTU_REQ;
            rtu_ctx[chn].curr->Status=EXCUTE_FAIL;
            rtu_ctx[chn].TOtimer = rtu_ctx[chn].poll_interval;
            if (rtu_ctx[chn].curr->Excute_Num > 1u)
            {//判断是否有重发的配置，0表示需要周期发送的请求，非0表示需要发送的次数
                rtu_ctx[chn].curr->Excute_Num--;
                RTU_AddReqBlock(&rtu_ctx[chn],rtu_ctx[chn].curr);
            }
            else if(rtu_ctx[chn].curr->Excute_Num == 0u)
            {
                RTU_AddReqBlock(&rtu_ctx[chn],rtu_ctx[chn].curr);
            }
            else
            {

            }
            Record_ErrCnt(rtu_ctx[chn].curr->chnindex);//记录一次错误
            //Error_Indicator(80);  //LED亮80ms后灭
        }
        else if(rtu_ctx[chn].event == EV_RX_OK)
        {//正常收到从节点的响应数据
            RTU_HandleReply(&rtu_ctx[chn]);
            rtu_ctx[chn].fsm_state = RTU_REQ;
            rtu_ctx[chn].TOtimer = rtu_ctx[chn].poll_interval;
            if (rtu_ctx[chn].curr->Excute_Num > 1u)
            {//判断是否有重发的配置，0表示需要周期发送的请求，非0表示需要发送的次数
                rtu_ctx[chn].curr->Excute_Num--;
                RTU_AddReqBlock(&rtu_ctx[chn],rtu_ctx[chn].curr);
            }
            else if(rtu_ctx[chn].curr->Excute_Num == 0u)
            {
                RTU_AddReqBlock(&rtu_ctx[chn],rtu_ctx[chn].curr);
            }
            else
            {

            }
        }
        else
        {

        }
        break;
        default:break;
    }
    rtu_ctx[chn].event = EV_NONE;
}

/********************************************************************************/
/*函数名：  Task_MBRTU_Master                                                       */
/*功能说明：RTU master主task, 分时调用，减少CPU load                                         */
/*输入参数：无                                                                        */
/*输出参数：无                                                                        */
/*******************************************************************************/
void MBRTU_Master_MainFunction(void *p_arg)
{
    static uint8_t chn_index;
   	
    if(chn_index >= NUM_UARTCHANNEL)
    {
      chn_index = 0;
    }
    if(RTU_CHN_ENABLE[chn_index] == TRUE)
    {
        RTU_CyclicTask(chn_index);
    }

    chn_index++;
}


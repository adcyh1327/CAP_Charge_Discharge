#include "main.h" 
#include "string.h"
#include "FM_Uart.h"
#include "cmsis_os.h"
//#include "Task_MB_RTU_Master.h"

volatile  Tdef_Byte                          _SystemFlag[NUM_UARTCHANNEL];
#define SystemFlag(n)                        _SystemFlag[n].Byte
#define g_bit_SCIFrameStart(n)               _SystemFlag[n].Bits.bit0    //串口帧起始
#define g_bit_SCIFrameEnd(n)                 _SystemFlag[n].Bits.bit1    //串口帧结束
#define g_bit_SCIFramePreciseStart(n)        _SystemFlag[n].Bits.bit2    //
#define g_bit_SCI_DMA_Send(n)                _SystemFlag[n].Bits.bit3    //DMA发送中标志

extern osMessageQId Que_UartLCDHandle;
extern osMessageQId Que_UartExtDevHandle;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;


static osMessageQId* Que_UartID[NUM_UARTCHANNEL] = {&Que_UartLCDHandle,&Que_UartExtDevHandle};
static uint16_t g_u16_SCISingalFrameRecTime[NUM_UARTCHANNEL];             //串口单帧数据接收时间计时，单位ms
static uint16_t g_u16_Message_Length[NUM_UARTCHANNEL];                     //当前已接收到的数据个数
static uint8_t l_u8_Receive_Buffer[NUM_UARTCHANNEL][SCI_BUF_MAXLEN];          //用于保存串口接收到的数据
static uint16_t byteidx_faramend[NUM_UARTCHANNEL];//帧结束的字节序号
static uint16_t l_DMA_SendTime[NUM_UARTCHANNEL];

static USARTCHN_Recv_t USARTCHN_Recv[NUM_UARTCHANNEL];//串口或CAN的接收备份缓冲区，识别到有效帧后存取，可直接拷贝至以太网缓冲区
Interface_Info UsartCHN_Data;//

enum Framestatus_t{
  frame_idle=0,frame_head,frame_data,frame_end,frame_chk  
};
enum Framestatus_t FrameStatus[NUM_UARTCHANNEL];

void CopyRecData(uint8_t channel);


void USART_Timer100us(void)
{
    uint8_t channel;
    
	for(channel=0;channel<NUM_UARTCHANNEL;channel++)
	{
        if(g_bit_SCI_DMA_Send(channel) == ON)
        {
            if(++l_DMA_SendTime[channel] >=100)
            {
                g_bit_SCI_DMA_Send(channel) = OFF;
                l_DMA_SendTime[channel] = 0;
#if 0
                DMA_Cmd(DMA1_Channel2, DISABLE);
                DMA_ClearITPendingBit(DMA1_IT_GL2 | DMA1_IT_TC2 | DMA1_IT_HT2 | DMA1_IT_TE2);
                USART3_485_RX_ENABLE;
#endif
            }
            
        }
        else
        {
            l_DMA_SendTime[channel] = 0;
        }
    
		if(FrameStatus[channel] != frame_idle)
		{
			if(g_u16_SCISingalFrameRecTime[channel]<=0xfff0)
			{
				g_u16_SCISingalFrameRecTime[channel]++;
			}
            if(g_u16_SCISingalFrameRecTime[channel]>UsartCHN_Data.Usart[channel][tmout])
            {
                if(((UsartCHN_Data.UsartProt[channel].FrameEndInfo.T_byte & FrameEndEn) != FrameEndEn)&&(FrameStatus[channel] == frame_data))
                {
                    //g_u16_SCISingalFrameRecTime[channel]=0;
                    FrameStatus[channel] = frame_idle;
                    CopyRecData(channel);
                }
                else
                {
                    FrameStatus[channel] = frame_idle;
                    //g_u16_SCISingalFrameRecTime[channel]=0;
                    g_u16_Message_Length[channel]=0;
                    //LED_BLINK_ONCE(S_FAULT);
                    //errorcode
                }
            }
		}
		else
		{
			//g_u16_SCISingalFrameRecTime[channel]=0;
		}
	}
}

void CopyRecData(uint8_t channel)
{
	uint8_t *start;
	if((UsartCHN_Data.UsartProt[channel].FrameStartInfo.T_byte & FrameStartEn) == FrameStartEn)
	{
		start = &l_u8_Receive_Buffer[channel][UsartCHN_Data.UsartProt[channel].FrameStartInfo.Bits.btn];
		USARTCHN_Recv[channel].lenth = g_u16_Message_Length[channel] + 1 -UsartCHN_Data.UsartProt[channel].FrameStartInfo.Bits.btn;
	}
	else
	{
		start = &l_u8_Receive_Buffer[channel][0];
        USARTCHN_Recv[channel].lenth = g_u16_Message_Length[channel] + 1;
	}
	if((UsartCHN_Data.UsartProt[channel].FrameStartInfo.T_byte & FrameEndEn) == FrameEndEn)
	{
		USARTCHN_Recv[channel].lenth = USARTCHN_Recv[channel].lenth - UsartCHN_Data.UsartProt[channel].FrameEndInfo.Bits.btn;
	}
	else if(UsartCHN_Data.UsartProt[channel].checksum == ChkSum_And)
	{
		USARTCHN_Recv[channel].lenth = USARTCHN_Recv[channel].lenth-2;
	}
	else
	{
		USARTCHN_Recv[channel].lenth = g_u16_Message_Length[channel]+1;
	}
	//g_u16_SCISingalFrameRecTime[channel]=0;
    memcpy(USARTCHN_Recv[channel].databuf,start,USARTCHN_Recv[channel].lenth);
    memset(l_u8_Receive_Buffer[channel],0,SCI_BUF_MAXLEN);
    USARTCHN_Recv[channel].newupd=ON;
    //OSMboxPost(mBOX_Uart_Recv[channel],(void *)&USARTCHN_Recv[channel].newupd);
    xQueueSend(*Que_UartID[channel], (const void *)&USARTCHN_Recv[channel] , QUE_WAIT_TIME);
}

void UsartRecieveData(uint8_t channel,uint8_t recdata)
{
	uint8_t Temp=0;
	Temp = (uint8_t) recdata;
    if(UartOpFunc[channel]._recv != NULL)
    {
        UartOpFunc[channel]._recv(channel,recdata);
        return;
    }
    
	if(FrameStatus[channel] == frame_idle)
	{
        if(g_u16_SCISingalFrameRecTime[channel]>UsartCHN_Data.Usart[channel][tmout])
        {
            g_u16_Message_Length[channel]=0;
    		if((UsartCHN_Data.UsartProt[channel].FrameStartInfo.T_byte & FrameStartEn) == FrameStartEn)//有帧头
    		{
    			if(Temp != UsartCHN_Data.UsartProt[channel].FrameStart[g_u16_Message_Length[channel]])
    			{
    				return;
    			}
                if(UsartCHN_Data.UsartProt[channel].FrameStartInfo.Bits.btn==byte_1)
                {
                    FrameStatus[channel] = frame_data;
                }
                else
                {
                    FrameStatus[channel] = frame_head;
                }
            }
            else
            {
                FrameStatus[channel] = frame_data;
            }
            l_u8_Receive_Buffer[channel][g_u16_Message_Length[channel]] = Temp;
        }
    }
    else if(FrameStatus[channel] == frame_head)
    {
        g_u16_Message_Length[channel]++;
		l_u8_Receive_Buffer[channel][g_u16_Message_Length[channel]] = Temp;
        if(Temp != UsartCHN_Data.UsartProt[channel].FrameStart[g_u16_Message_Length[channel]])
        {
            FrameStatus[channel] = frame_idle;
            g_u16_SCISingalFrameRecTime[channel]=0;
            g_u16_Message_Length[channel]=0;
            return;
        }
        if((g_u16_Message_Length[channel]+1) == UsartCHN_Data.UsartProt[channel].FrameStartInfo.Bits.btn)
        {
            FrameStatus[channel] = frame_data;
        }
    }
    else if(FrameStatus[channel] == frame_data)
    {
        g_u16_Message_Length[channel]++;
		    l_u8_Receive_Buffer[channel][g_u16_Message_Length[channel]] = Temp;
        if((UsartCHN_Data.UsartProt[channel].FrameEndInfo.T_byte & FrameEndEn) == FrameEndEn)
		    {
            if(Temp==UsartCHN_Data.UsartProt[channel].FrameEnd[0])
			{
                if(UsartCHN_Data.UsartProt[channel].FrameEndInfo.Bits.btn==byte_1)
                {
                    if(UsartCHN_Data.UsartProt[channel].checksum ==CheckSum_None)
                    {
                        FrameStatus[channel] = frame_idle;
                        CopyRecData(channel);
                    }
                    else
                    {
                        FrameStatus[channel] = frame_chk;//此功能暂未做
                    }
                }
                else
                {
                    byteidx_faramend[channel]=g_u16_Message_Length[channel];
                    FrameStatus[channel] = frame_end;
                }
            }
        }
    }
    else if(FrameStatus[channel] == frame_end)
    {
        g_u16_Message_Length[channel]++;
        l_u8_Receive_Buffer[channel][g_u16_Message_Length[channel]] = Temp;
        if(Temp!=UsartCHN_Data.UsartProt[channel].FrameEnd[g_u16_Message_Length[channel]-byteidx_faramend[channel]])
        {
            FrameStatus[channel] = frame_data;
            byteidx_faramend[channel]=0;
        }
        if((g_u16_Message_Length[channel]-byteidx_faramend[channel]+1) == UsartCHN_Data.UsartProt[channel].FrameEndInfo.Bits.btn)
        {
            if(UsartCHN_Data.UsartProt[channel].checksum ==CheckSum_None)
            {
                FrameStatus[channel] = frame_idle;
                CopyRecData(channel);
            }
            else
            {
                FrameStatus[channel] = frame_chk;//此功能暂未做
            }
        }
    }
    else if(FrameStatus[channel] == frame_data)
    {
        //此功能暂未做
    }
    else
    {
        FrameStatus[channel] = frame_idle;
        byteidx_faramend[channel]=0;
        g_u16_Message_Length[channel]=0;
    }
    g_u16_SCISingalFrameRecTime[channel]=0;
}

#if 0
void USART1_IRQ(uint8_t data)/*  */
{
    UsartRecieveData(RS232_1,data);
	USART1_RecieveData(data);
}

void USART2_IRQ(uint8_t data)/*  */
{
    UsartRecieveData(RS485_2,data);
	USART3_RecieveData(data);
}

void USART3_IRQ(uint8_t data)/*  */
{
    UsartRecieveData(RS485_1,data);
	USART2_RecieveData(data);
}

void UART4_IRQ(uint8_t data)/*  */
{
    UsartRecieveData(RS485_3,data);
	UART4_RecieveData(data);
}

void UART5_IRQ(uint8_t data)/*  */
{
    UsartRecieveData(RS485_4,data);
	UART5_RecieveData(data);
}
#endif




void USART3_Send_Data(uint8_t *send_buff,uint16_t length)
{
	HAL_UART_Transmit_DMA(&huart3,send_buff,length);
}
void UART5_Send_Data(uint8_t *send_buff,uint16_t length)
{
	HAL_UART_Transmit_DMA(&huart5,send_buff,length);
}



void FM_Usart_Init(void)
{
	uint8_t i;
	for(i=0; i<NUM_UARTCHANNEL; i++)
	{
		g_u16_SCISingalFrameRecTime[i] = 0xfff0;
	}
}


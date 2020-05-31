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
extern UART_HandleTypeDef huart6;


static uint16_t g_u16_SCISingalFrameRecTime[NUM_UARTCHANNEL];             //串口单帧数据接收时间计时，单位ms
static uint16_t g_u16_Message_Length[NUM_UARTCHANNEL];                     //当前已接收到的数据个数
static uint8_t l_u8_Receive_Buffer[NUM_UARTCHANNEL][SCI_BUF_MAXLEN];          //用于保存串口接收到的数据
static uint16_t byteidx_faramend[NUM_UARTCHANNEL];//帧结束的字节序号
static uint16_t l_DMA_SendTime[NUM_UARTCHANNEL];
static USARTCHN_Recv_t USARTCHN_RecvBuff[NUM_UARTCHANNEL];//串口或CAN的接收备份缓冲区，识别到有效帧后存取，可直接拷贝至以太网缓冲区
//static xQueueHandle Que_CopyData;//请求copy缓冲区数据

Interface_Info UsartCHN_Cfg=
{
  .UsartProt[USART_1]=
  {
      .FrameStartInfo = 0,//FrameStartEn|byte_1,  //本配置为0时则为无协议，所有数据均为有效数据，当字节间时间超过配置时间后认为是完整一帧
      .FrameStart = {0x02},
      .FrameEndInfo = 0,//FrameStartEn|byte_2,//同帧头配置类似，前边的宏使能才表示有帧尾，后边一个为字节数量
      .FrameEnd = {0x0d,0x0a,},
      .checksum = CheckSum_None,//当前是0,根据实际需要可能要更改
      .timeoout_100us = 100//单位100us,串口帧传输过程中字节间时间的最大允许间隔
  },
  .UsartProt[USART_2]=
  {
      .FrameStartInfo = 0,//FrameStartEn|byte_1,  //本配置为0时则为无协议，所有数据均为有效数据，当字节间时间超过配置时间后认为是完整一帧
      .FrameStart = {0x02},
      .FrameEndInfo = 0,//FrameStartEn|byte_2,//同帧头配置类似，前边的宏使能才表示有帧尾，后边一个为字节数量
      .FrameEnd = {0x0d,0x0a,},
      .checksum = CheckSum_None,//当前是0,根据实际需要可能要更改
      .timeoout_100us = 100//单位100us,串口帧传输过程中字节间时间的最大允许间隔
  },
  .UsartProt[USART_3]=
  {
      .FrameStartInfo = 0,//FrameStartEn|byte_1,  //本配置为0时则为无协议，所有数据均为有效数据，当字节间时间超过配置时间后认为是完整一帧
      .FrameStart = {0x02},
      .FrameEndInfo = 0,//FrameStartEn|byte_2,//同帧头配置类似，前边的宏使能才表示有帧尾，后边一个为字节数量
      .FrameEnd = {0x0d,0x0a,},
      .checksum = CheckSum_None,//当前是0,根据实际需要可能要更改
      .timeoout_100us = 10//单位100us,串口帧传输过程中字节间时间的最大允许间隔
  },
  .UsartProt[UART_5]=
  {
      .FrameStartInfo = 0,//FrameStartEn|byte_1,  //本配置为0时则为无协议，所有数据均为有效数据，当字节间时间超过配置时间后认为是完整一帧
      .FrameStart = {0x02},
      .FrameEndInfo = 0,//FrameStartEn|byte_2,//同帧头配置类似，前边的宏使能才表示有帧尾，后边一个为字节数量
      .FrameEnd = {0x0d,0x0a,},
      .checksum = CheckSum_None,//当前是0,根据实际需要可能要更改
      .timeoout_100us = 100//单位100us,串口帧传输过程中字节间时间的最大允许间隔
  },
  .UsartProt[USART_6]=
  {
      .FrameStartInfo = 0,//FrameStartEn|byte_1,  //本配置为0时则为无协议，所有数据均为有效数据，当字节间时间超过配置时间后认为是完整一帧
      .FrameStart = {0x02},
      .FrameEndInfo = 0,//FrameStartEn|byte_2,//同帧头配置类似，前边的宏使能才表示有帧尾，后边一个为字节数量
      .FrameEnd = {0x0d,0x0a,},
      .checksum = CheckSum_None,//当前是0,根据实际需要可能要更改
      .timeoout_100us = 100//单位100us,串口帧传输过程中字节间时间的最大允许间隔
  },
};



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
            if(g_u16_SCISingalFrameRecTime[channel]>UsartCHN_Cfg.UsartProt[channel].timeoout_100us)
            {
                if(((UsartCHN_Cfg.UsartProt[channel].FrameEndInfo.T_byte & FrameEndEn) != FrameEndEn)&&(FrameStatus[channel] == frame_data))
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
  portBASE_TYPE xHigherPriorityTaskWoken;
	if((UsartCHN_Cfg.UsartProt[channel].FrameStartInfo.T_byte & FrameStartEn) == FrameStartEn)
	{
		start = &l_u8_Receive_Buffer[channel][UsartCHN_Cfg.UsartProt[channel].FrameStartInfo.Bits.btn];
		USARTCHN_RecvBuff[channel].lenth = g_u16_Message_Length[channel] + 1 -UsartCHN_Cfg.UsartProt[channel].FrameStartInfo.Bits.btn;
	}
	else
	{
		start = &l_u8_Receive_Buffer[channel][0];
    USARTCHN_RecvBuff[channel].lenth = g_u16_Message_Length[channel] + 1;
	}
	if((UsartCHN_Cfg.UsartProt[channel].FrameStartInfo.T_byte & FrameEndEn) == FrameEndEn)
	{
		USARTCHN_RecvBuff[channel].lenth = USARTCHN_RecvBuff[channel].lenth - UsartCHN_Cfg.UsartProt[channel].FrameEndInfo.Bits.btn;
	}
	else if(UsartCHN_Cfg.UsartProt[channel].checksum == ChkSum_And)
	{
		USARTCHN_RecvBuff[channel].lenth = USARTCHN_RecvBuff[channel].lenth-2;
	}
	else
	{
		USARTCHN_RecvBuff[channel].lenth = g_u16_Message_Length[channel]+1;
	}
	//g_u16_SCISingalFrameRecTime[channel]=0;
	  memset(USARTCHN_RecvBuff[channel].databuf,0,sizeof(USARTCHN_RecvBuff[channel].databuf));
    memcpy(USARTCHN_RecvBuff[channel].databuf,start,USARTCHN_RecvBuff[channel].lenth);
    memset(l_u8_Receive_Buffer[channel],0,SCI_BUF_MAXLEN);
    //USARTCHN_Recv[channel].newupd=ON;
    xQueueSendFromISR(Que_UartID[channel], (void *)&USARTCHN_RecvBuff[channel], &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    //xQueueSend(Que_UartID[channel], (void *)&USARTCHN_RecvBuff[channel], ( portTickType ) 10 );
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
        if(g_u16_SCISingalFrameRecTime[channel]>UsartCHN_Cfg.Usart[channel][tmout])
        {
            g_u16_Message_Length[channel]=0;
    		if((UsartCHN_Cfg.UsartProt[channel].FrameStartInfo.T_byte & FrameStartEn) == FrameStartEn)//有帧头
    		{
    			if(Temp != UsartCHN_Cfg.UsartProt[channel].FrameStart[g_u16_Message_Length[channel]])
    			{
    				return;
    			}
                if(UsartCHN_Cfg.UsartProt[channel].FrameStartInfo.Bits.btn==byte_1)
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
        if(Temp != UsartCHN_Cfg.UsartProt[channel].FrameStart[g_u16_Message_Length[channel]])
        {
            FrameStatus[channel] = frame_idle;
            g_u16_SCISingalFrameRecTime[channel]=0;
            g_u16_Message_Length[channel]=0;
            return;
        }
        if((g_u16_Message_Length[channel]+1) == UsartCHN_Cfg.UsartProt[channel].FrameStartInfo.Bits.btn)
        {
            FrameStatus[channel] = frame_data;
        }
    }
    else if(FrameStatus[channel] == frame_data)
    {
        g_u16_Message_Length[channel]++;
		    l_u8_Receive_Buffer[channel][g_u16_Message_Length[channel]] = Temp;
        if((UsartCHN_Cfg.UsartProt[channel].FrameEndInfo.T_byte & FrameEndEn) == FrameEndEn)
		    {
            if(Temp==UsartCHN_Cfg.UsartProt[channel].FrameEnd[0])
			{
                if(UsartCHN_Cfg.UsartProt[channel].FrameEndInfo.Bits.btn==byte_1)
                {
                    if(UsartCHN_Cfg.UsartProt[channel].checksum ==CheckSum_None)
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
        if(Temp!=UsartCHN_Cfg.UsartProt[channel].FrameEnd[g_u16_Message_Length[channel]-byteidx_faramend[channel]])
        {
            FrameStatus[channel] = frame_data;
            byteidx_faramend[channel]=0;
        }
        if((g_u16_Message_Length[channel]-byteidx_faramend[channel]+1) == UsartCHN_Cfg.UsartProt[channel].FrameEndInfo.Bits.btn)
        {
            if(UsartCHN_Cfg.UsartProt[channel].checksum ==CheckSum_None)
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


void USART1_Send_Data(uint8_t *send_buff,uint16_t length)
{
  USART1_485_TX_ENABLE;
	HAL_UART_Transmit_DMA(&huart1,send_buff,length);
	//HAL_UART_Transmit(&huart1,send_buff,length,1000);
}
void USART2_Send_Data(uint8_t *send_buff,uint16_t length)
{
	HAL_UART_Transmit_DMA(&huart2,send_buff,length);
}
void USART3_Send_Data(uint8_t *send_buff,uint16_t length)
{
	HAL_UART_Transmit_DMA(&huart3,send_buff,length);
  //HAL_UART_Transmit(&huart3,send_buff,length,1000);
}
void UART5_Send_Data(uint8_t *send_buff,uint16_t length)
{
  UART5_485_TX_ENABLE;
	HAL_UART_Transmit_DMA(&huart5,send_buff,length);
}
void USART6_Send_Data(uint8_t *send_buff,uint16_t length)
{
	HAL_UART_Transmit_DMA(&huart6,send_buff,length);
  //HAL_UART_Transmit(&huart6,send_buff,length,1000);
}



void FM_Usart_Init(void)
{
	uint8_t i;
	for(i=0; i<NUM_UARTCHANNEL; i++)
	{
		g_u16_SCISingalFrameRecTime[i] = 0xfff0;
	}
  //Que_CopyData =xQueueCreate( 10, sizeof(USARTCHN_Recv_t ) );
}

void FM_Usart_Mainfunction(void *p_arg)
{
  uint8_t channel;
  //if( Que_CopyData != 0)    
  {// 从创建的队列中接收一个消息，如果消息无效，阻塞在此处        
    //if(xQueueReceive( Que_CopyData, &(channel), ( portTickType ) 10 ) )        
    {// 现在pcRxedMessage 指向由vATask任务投递进来的结构体Amessage变量        }    }
     // CopyRecData(channel);
    }
	}
}



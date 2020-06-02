#include "config.h"
#include "cmsis_os.h"
#include "interface.h"
#include "string.h"
#include "Task_LCD_Display.h"
#include "Task_MB_RTU_Master.h"
#include "FM_Uart.h"
#include "string.h"
#include "stdio.h"

#define SENDBUF_LENTH_MAX                                     32

static uint8_t lcdregister=0;
static uint8_t lcdRAM=1;

static uint8_t LCD_HandShakeStatus = FALSE;

static uint8_t LCD_Sendbuf[SENDBUF_LENTH_MAX]={0};
static uint8_t LCD_RecvBuf[SENDBUF_LENTH_MAX]={0};

/********************************************************************************/
/*函数名：  LCD_Display_Init                                                             */
/*功能说明：模块初始化函数                                                        */
/*输入参数：无*/
/*输出参数：无                                                                   */
/********************************************************************************/
void LCD_Display_Init(void)
{//模块初始化
  LCD_HandShakeStatus = FALSE;
  //RTU_AddReqBlock(&rtu_ctx[RTU_Req_ReadCurrent_Thres.chnindex],&RTU_Req_ReadCurrent_Thres);
  lcdregister =5;
}

void READ_Register(uint8_t addr,uint8_t num)
{
  uint8_t index=0;
  uint16_t crc;
  LCD_Sendbuf[index++]=0x5a;
  LCD_Sendbuf[index++]=0xa5;
  LCD_Sendbuf[index++]=0x05;
  LCD_Sendbuf[index++]=Func_ReadRegister;
  LCD_Sendbuf[index++]=addr;
  LCD_Sendbuf[index++]=num;
  crc = Get_rtuCrc16(&LCD_Sendbuf[3],index-3);
  LCD_Sendbuf[index++]=(uint8_t)((crc>>8)&0xff);
  LCD_Sendbuf[index++] = (uint8_t)(crc&0xff);
  UartOpFunc[UART_CHN_LCD_DISPLAY]._send(LCD_Sendbuf,index);
}

void Write_Register(uint8_t addr,uint8_t *buf,uint8_t num)
{
  uint8_t index=0;
  uint8_t i;
  uint16_t crc;
  LCD_Sendbuf[index++]=0x5a;
  LCD_Sendbuf[index++]=0xa5;
  LCD_Sendbuf[index++]=0x04 + num;
  LCD_Sendbuf[index++]=Func_WriteRegister;
  LCD_Sendbuf[index++]=addr;
  for(i=0;i<num;i++)
  {
    LCD_Sendbuf[index++]=buf[i];
  }
  crc = Get_rtuCrc16(&LCD_Sendbuf[3],index-3);
  LCD_Sendbuf[index++]=(uint8_t)((crc>>8)&0xff);
  LCD_Sendbuf[index++] = (uint8_t)(crc&0xff);
  UartOpFunc[UART_CHN_LCD_DISPLAY]._send(LCD_Sendbuf,index);
}

void READ_RAM(uint16_t addr,uint8_t num)
{
  uint8_t index=0;
  uint16_t crc;
  LCD_Sendbuf[index++]=0x5a;
  LCD_Sendbuf[index++]=0xa5;
  LCD_Sendbuf[index++]=0x06;
  LCD_Sendbuf[index++]=Func_ReadRAM;
  LCD_Sendbuf[index++]=(uint8_t)((addr>>8)&0xff);
  LCD_Sendbuf[index++]=(uint8_t)(addr&0xff);
  LCD_Sendbuf[index++]=num;
  crc = Get_rtuCrc16(&LCD_Sendbuf[3],index-3);
  LCD_Sendbuf[index++]=(uint8_t)((crc>>8)&0xff);
  LCD_Sendbuf[index++] = (uint8_t)(crc&0xff);
  UartOpFunc[UART_CHN_LCD_DISPLAY]._send(LCD_Sendbuf,index);
}

void WRITE_RAM(uint16_t addr,uint16_t *buf,uint8_t num)
{
  uint8_t index=0;
  uint8_t i;
  uint16_t crc;
  LCD_Sendbuf[index++]=0x5a;
  LCD_Sendbuf[index++]=0xa5;
  LCD_Sendbuf[index++]=0x05+ num * 2;
  LCD_Sendbuf[index++]=Func_WriteRAM;
  LCD_Sendbuf[index++]=(uint8_t)((addr>>8)&0xff);
  LCD_Sendbuf[index++]=(uint8_t)(addr&0xff);
  for(i=0;i<num;i++)
  {
    LCD_Sendbuf[index++]=((buf[i]>>8)&0xff);
    LCD_Sendbuf[index++]=buf[i]&0xff;
  }
  crc = Get_rtuCrc16(&LCD_Sendbuf[3],index-3);
  LCD_Sendbuf[index++]=(uint8_t)((crc>>8)&0xff);
  LCD_Sendbuf[index++] = (uint8_t)(crc&0xff);
  UartOpFunc[UART_CHN_LCD_DISPLAY]._send(LCD_Sendbuf,index);
}
/*
检查屏幕通信状态
*/
void LCD_HandShake_Process(void)
{
  READ_Register(0x00,1); /*Read the DGUS SW Version*/
}

void LCD_ReadRegister_Response_Process(void)
{
  uint8_t addr = LCD_RecvBuf[1];
  switch (addr)
  {
    case 0x00:/* DGUS version */
      LCD_HandShakeStatus = TRUE;
    break;
  
    default:
    break;
  }
}
/*
LCD串口返回数据的CRC 比对
*/
void LCD_Buffer_Data_Process(void)
{
  uint8_t command = LCD_RecvBuf[0];
  switch (command)
  {
    case 0x80:
    /* code */
    break;
    case 0x81:
      LCD_ReadRegister_Response_Process();
    break;
    case 0x82:
    /* code */
    break;   
    case 0x83:
    /* code */
    break;

    default:
    break;
  }
}
/*
LCD串口返回数据的处理
*/
void LCD_Recv_Data_Process(USARTCHN_Recv_t *pSrcData)
{
  uint8_t i = 0;
  uint8_t len = 0;
  uint8_t *pFrameData = NULL;
  uint16_t crc = 0;
  uint16_t *pRcv_crc = NULL;
  uint8_t j;
  /* find the frame head 0x5aa5 */
  do
  {
    /* find the frame head 0x5aa5 */
    while(0x5A != pSrcData->databuf[i]){i++;} /* looking for head first Byte */
    if(0xA5 == pSrcData->databuf[i+1])/* found the frame first head , and also head second byte */
    {
      len = pSrcData->databuf[i+2];
      pFrameData = &pSrcData->databuf[i+3];
      pRcv_crc = (uint16_t *)(&pFrameData[len-2]);
      crc = Get_rtuCrc16(pFrameData,len-2);
      if( crc == *pRcv_crc )
      {
        memcpy(&LCD_RecvBuf[0],pFrameData,len-2); /* copy the data to recv buffer*/
        printf("Received Frame Data: ");
        for(j=0;j < len; j++)
        {
          printf("%x ",LCD_RecvBuf[j]);
        }
        printf("\n");
        LCD_Buffer_Data_Process();
        break; /* break out */
      }
      else
      {
        /*drop this frame, and trying to found again from next frame*/
        i = i+1+len;
      }
    }
    else/* found the frame first head , but head second byte is wrong*//* not frame head */
    {
      /*drop it, and trying to found again from next byte*/
      i = i+1;
    }
  } while (i < pSrcData->lenth);

}
/********************************************************************************/
/*函数名：  LCD_Display_MainFunction                                                       */
/*功能说明：RTU master主task, 分时调用，减少CPU load                                         */
/*输入参数：无                                                                        */
/*输出参数：无                                                                        */
/*******************************************************************************/
void LCD_Display_MainFunction(void *p_arg)
{
  static USARTCHN_Recv_t Recv_Data;
  static uint8_t addr=0;
  static uint8_t num = 1;
  static uint8_t data8[6];
  static uint16_t data16[20];
  if( Que_UartID[UART_CHN_LCD_DISPLAY] != 0)    
  {// 从创建的队列中接收一个消息
    if(xQueueReceive( Que_UartID[UART_CHN_LCD_DISPLAY], &(Recv_Data), ( portTickType ) 10 ) )        
    {// 现在pcRxedMessage 指向由vATask任务投递进来的结构体Amessage变量        }    }
        LCD_Display_Init();
        LCD_Recv_Data_Process(&Recv_Data);
    }
	}

  if(lcdregister == 1)
  {
    lcdregister = 0;
    READ_Register(addr,num);
  } 
  else if(lcdregister==2)
  {
    lcdregister = 0;
    Write_Register(addr,data8,num);
  }
  else
  {
    
  }
  
  if(lcdRAM==1)
  {
    lcdRAM = 0;
    READ_RAM(addr,num);
  } 
  else if(lcdRAM==2)
  {
    lcdRAM = 0;
    WRITE_RAM(addr,data16,num);
  }
  else
  {
    
  }
}







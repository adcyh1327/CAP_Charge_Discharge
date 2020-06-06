#include "config.h"
#include "cmsis_os.h"
#include "interface.h"
#include "string.h"
#include "Task_LCD_Display.h"
#include "Task_MB_RTU_Master.h"
#include "FM_Uart.h"
#include "string.h"
#include "stdio.h"
#include "gui_page.h"

#define SENDBUF_LENTH_MAX                                     32

/* MCU 与 LCD的状态 */
#define LCD_VERSION 0xFF
#define MCU_LCD_STATUS_INACTIVE   0x00
#define MCU_LCD_STATUS_CONNECTING 0x01
#define MCU_LCD_STATUS_CONNECTED  0x02
#define MCU_LCD_STATUS_ERROR      0x03

static uint8_t McuLcdStatus = MCU_LCD_STATUS_INACTIVE;
static uint8_t LcdVersionInfo = 0x00;
static uint8_t lcdregister=0;
static uint8_t lcdRAM=1;

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
  McuLcdStatus = MCU_LCD_STATUS_INACTIVE;
  LcdVersionInfo = 0xFF;
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

void LCD_ReadRegister_Response_Process(void)
{
  uint8_t addr = LCD_RecvBuf[1]; /* 地址 */
  uint8_t length = LCD_RecvBuf[2]; /* 数据长度 */
  switch (addr)
  {
    case 0x00:/* DGUS version */
      if(0x1 == length)
      {
        LcdVersionInfo = LCD_RecvBuf[3];
      }
      else
      {
        printf("read lcd version error;\n");
      }
    break;
  
    default:
    break;
  }
}


void LCD_Key_Process(void)
{
  if(0==pGUI_Current_Pages->page_id)
  {
    switch(pGUI_Current_Pages->pWdgRetKeyCode->ret_key_code)
    {
      case ePage_0_Key_SysSetting:
      break;
      case ePage_0_Key_Save:
      break;
      case ePage_0_Key_Exit:
      break;
      default:
      break;
    }
  }
  else if(1==pGUI_Current_Pages->page_id)
  {

  }
  else
  {
    /* code */
  }

  pGUI_Current_Pages->pWdgRetKeyCode->ret_key_code = 0x0;
}
void LCD_ReadRam_Response_Process(void)
{
	uint8_t i;
  uint8_t addr_h = LCD_RecvBuf[1];
  uint8_t addr_l = LCD_RecvBuf[2];
  uint16_t addr = ((addr_h << 8) & 0xFF00) | (addr_l);
  uint8_t length = LCD_RecvBuf[3];

  /*检查是不是按键消息*/
  if(addr == pGUI_Current_Pages->pWdgRetKeyCode->ret_key_code_addr) /* Key Pressed */
  {
    pGUI_Current_Pages->pWdgRetKeyCode->ret_key_code = ((LCD_RecvBuf[4] << 8) & 0xFF00) | (LCD_RecvBuf[5]); /* Key Val */
    LCD_Key_Process();
  }
  else
  {
    for(i=0;i<pGUI_Current_Pages->WdgDataDisplayCnt;i++) /*检查是不是数据显示消息*/
    {
      if(addr == pGUI_Current_Pages->pWdgDataDispaly[i].data_display_addr) /* Data Display */
      {
        pGUI_Current_Pages->pWdgDataDispaly[i].data_val = ((LCD_RecvBuf[4] << 8) & 0xFF00) | (LCD_RecvBuf[5]);
        return;
      }
    }

    for(i=0;i<pGUI_Current_Pages->WdgDataInputCnt;i++) /*检查是不是数据输入消息*/
    {
      if(addr == pGUI_Current_Pages->pWdgDatInput[i].data_input_addr) /* Data Input */
			{
				pGUI_Current_Pages->pWdgDatInput[i].data_val = ((LCD_RecvBuf[4] << 8) & 0xFF00) | (LCD_RecvBuf[5]);
				return;
			}
    }
		
		/* useless addr received, code */
		printf("useless addr received, addr = %x;\n",addr);
  }
}
/*
LCD中的buffer数据取出来处理
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
      LCD_ReadRam_Response_Process();
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
  while (i < pSrcData->lenth)
  {
    /* find the frame head 0x5aa5 */
    while(0x5A != pSrcData->databuf[i]){i++;} /* looking for head first Byte */
    if(0xA5 == pSrcData->databuf[i+1])/* found the frame first head , and also head second Byte */
    {
      len = pSrcData->databuf[i+2];
      pFrameData = &pSrcData->databuf[i+3];
      pRcv_crc = (uint16_t *)(&pFrameData[len-2]);
      crc = Get_rtuCrc16(pFrameData,len-2);
      printf("The calculated CRC: %x, Received CRC: %x\n",crc,*pRcv_crc);
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
  } 

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

  switch(McuLcdStatus)
  {
    case MCU_LCD_STATUS_INACTIVE:
      /*    检查屏幕通信状态    */
      READ_Register(0x00,1); /*Read the DGUS SW Version, 通过读取版本号来判断通信是否正常*/
      McuLcdStatus = MCU_LCD_STATUS_CONNECTING;
    break;

    case MCU_LCD_STATUS_CONNECTING:
      if(LCD_VERSION == LcdVersionInfo)
      {
        McuLcdStatus = MCU_LCD_STATUS_CONNECTED;
      }
      else
      {
        /* waiting, unitl receive the right version */
      }
    break;

    case MCU_LCD_STATUS_CONNECTED:
    /* 周期性的读取屏幕上的值，或者更新LCd上的显示 */
    break;

    case MCU_LCD_STATUS_ERROR:
    default:
      printf("McuLcdStatus(=%d) error;\n",McuLcdStatus);
    break;
  }
}







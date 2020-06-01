#include "config.h"
#include "cmsis_os.h"
#include "interface.h"
#include "string.h"
#include "Task_LCD_Display.h"
#include "Task_MB_RTU_Master.h"
#include "FM_Uart.h"

#define SENDBUF_LENTH_MAX                                     32

static uint8_t lcdregister=0;
static uint8_t lcdRAM=1;

static uint8_t LCD_Sendbuf[SENDBUF_LENTH_MAX];

/********************************************************************************/
/*函数名：  LCD_Display_Init                                                             */
/*功能说明：模块初始化函数                                                        */
/*输入参数：无*/
/*输出参数：无                                                                   */
/********************************************************************************/
void LCD_Display_Init(void)
{//模块初始化
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

void READ_DisplayRAM(uint16_t addr,uint8_t num)
{
  uint8_t index=0;
  uint16_t crc;
  LCD_Sendbuf[index++]=0x5a;
  LCD_Sendbuf[index++]=0xa5;
  LCD_Sendbuf[index++]=0x06;
  LCD_Sendbuf[index++]=Func_ReadDisplayRAM;
  LCD_Sendbuf[index++]=(uint8_t)((addr>>8)&0xff);
  LCD_Sendbuf[index++]=(uint8_t)(addr&0xff);
  LCD_Sendbuf[index++]=num;
  crc = Get_rtuCrc16(&LCD_Sendbuf[3],index-3);
  LCD_Sendbuf[index++]=(uint8_t)((crc>>8)&0xff);
  LCD_Sendbuf[index++] = (uint8_t)(crc&0xff);
  UartOpFunc[UART_CHN_LCD_DISPLAY]._send(LCD_Sendbuf,index);
}

void Write_DisplayRAM(uint16_t addr,uint16_t *buf,uint8_t num)
{
  uint8_t index=0;
  uint8_t i;
  uint16_t crc;
  LCD_Sendbuf[index++]=0x5a;
  LCD_Sendbuf[index++]=0xa5;
  LCD_Sendbuf[index++]=0x05+ num * 2;
  LCD_Sendbuf[index++]=Func_WriteDisplayRAM;
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
    READ_DisplayRAM(addr,num);
  } 
  else if(lcdRAM==2)
  {
    lcdRAM = 0;
    Write_DisplayRAM(addr,data16,num);
  }
  else
  {
    
  }
}







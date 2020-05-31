#include "config.h"
#include "cmsis_os.h"
#include "interface.h"
#include "string.h"
#include "Task_LCD_Display.h"
#include "FM_Uart.h"

static uint8_t index;

/********************************************************************************/
/*函数名：  LCD_Display_Init                                                             */
/*功能说明：模块初始化函数                                                        */
/*输入参数：无*/
/*输出参数：无                                                                   */
/********************************************************************************/
void LCD_Display_Init(void)
{//模块初始化
  //RTU_AddReqBlock(&rtu_ctx[RTU_Req_ReadCurrent_Thres.chnindex],&RTU_Req_ReadCurrent_Thres);
  index++;
}

/********************************************************************************/
/*函数名：  LCD_Display_MainFunction                                                       */
/*功能说明：RTU master主task, 分时调用，减少CPU load                                         */
/*输入参数：无                                                                        */
/*输出参数：无                                                                        */
/*******************************************************************************/
void LCD_Display_MainFunction(void *p_arg)
{
  USARTCHN_Recv_t Recv_Data;
  if( Que_UartID[UART_CHN_LCD_DISPLAY] != 0)    
  {// 从创建的队列中接收一个消息，如果消息无效，阻塞在此处    
    if(xQueueReceive( Que_UartID[UART_CHN_LCD_DISPLAY], &(Recv_Data), ( portTickType ) 10 ) )        
    {// 现在pcRxedMessage 指向由vATask任务投递进来的结构体Amessage变量        }    }
      index++;
      if(index%3 == 0)
      {
        LCD_Display_Init();
      }
    }
	}
}


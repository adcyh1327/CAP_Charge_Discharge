#include "config.h"
#include "cmsis_os.h"
#include "interface.h"
#include "string.h"
#include "Task_HF_SampleBoard.h"
#include "Task_MB_RTU_Master.h"



struct data
{
  float  Ua;        //A路电压
  float  Ia;        //A路电流
  float  Pa;        //A路功率
  float  PFa;       //A路功率因素
  float  Fa;        //A路频率
    
  float  Ub;        //B路电压
  float  Ib;        //B路电流
  float  Pb;        //B路功率
  float  PFb;       //B路功率因素
  float  Fb;        //B路频率
  
  float  Uc;        //C路电压
  float  Ic;        //C路电流
  float  Pc;        //C路功率
  float  PFc;       //C路功率因素
  float  Fc;        //C路频率
};

//Species 1电流1路 2电流2路
//CH 1 A相 2 B相 3 C相 
//num 第几个校准点
//data 数据
void Save_Adj_Data(uint8_t Species ,uint8_t CH, uint8_t num,uint8_t *_pWriteBuf)
{
  uint8_t   _ucLen;
  uint16_t  CrcCheck;
  uint8_t   Tbuffer[20];
  _ucLen=0;
  Tbuffer[_ucLen++]=0x5A;
  Tbuffer[_ucLen++]=0x10;
  Tbuffer[_ucLen++]=Species;  // Species 1电流1路 2电流2路
  Tbuffer[_ucLen++]=CH;     //CH 1 A相 2 B相 3 C相 
  Tbuffer[_ucLen++]=num;   //num 第几个校准点
  Tbuffer[_ucLen++]=_pWriteBuf[0];
  Tbuffer[_ucLen++]=_pWriteBuf[1];
  Tbuffer[_ucLen++]=_pWriteBuf[2];
  Tbuffer[_ucLen++]=_pWriteBuf[3];
  CrcCheck = Get_rtuCrc16(Tbuffer,_ucLen);
  Tbuffer[_ucLen++] =  CrcCheck>>8;
  Tbuffer[_ucLen++] =  CrcCheck;
  UartOpFunc[UART_CHN_AD_SAMPLE]._send(Tbuffer,_ucLen);
}


/********************************************************************************/
/*函数名：  HF_SampleBoard_Init                                                             */
/*功能说明：模块初始化函数                                                        */
/*输入参数：无*/
/*输出参数：无                                                                   */
/********************************************************************************/
void HF_SampleBoard_Init(void)
{//模块初始化
  //RTU_AddReqBlock(&rtu_ctx[RTU_Req_ReadCurrent_Thres.chnindex],&RTU_Req_ReadCurrent_Thres);
}

/********************************************************************************/
/*函数名：  Task_MBRTU_Master                                                       */
/*功能说明：RTU master主task, 分时调用，减少CPU load                                         */
/*输入参数：无                                                                        */
/*输出参数：无                                                                        */
/*******************************************************************************/
void HF_SampleBoard_MainFunction(void *p_arg)
{

}


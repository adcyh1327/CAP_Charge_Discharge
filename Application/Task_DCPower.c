#include "config.h"
#include "cmsis_os.h"
#include "interface.h"
#include "string.h"
#include "Task_DCPower.h"
#include "Task_MB_RTU_Master.h"

enum off_outputstatus{
  off_PowerStatus, off_Voltagge, off_Current, off_Power,
  off_StatusTotal
};//电源状态，输出电压、电流、功率测量值
static uint16_t DC_OutputStatus[off_StatusTotal];

enum off_Setting{
  off_SetVoltage, off_SetCurrent, off_SetOutSts, off_SetRunMode, off_SetPowerReset, off_SetPowerCtrlMode,
  off_SettingTotal
};//电压设置/电流设置/输出状态设置/电源运行模式/电源复位/电源远端或本地控制
static uint16_t DC_SettingValue[off_SettingTotal];

static RTU_ReqBlock_t RTU_Req_Read_DC_OutputStatus = //RTU数据读请求块-输出测量：电源状态，输出电压、电流、功率测量值
{
  LIST_HEAD_INIT(RTU_Req_Read_DC_OutputStatus.Entry),
  0,                                          //执行次数，0-无限次
  UART_CHN_AD_SAMPLE,                         //执行通道
	SLAVEID_DC_POWER,                           //从节点站地址
	FUNC_RD_HOLDREG,                            //功能码
	EXCUTE_SUCCESS,                             //执行结果
	200,                                        //操作寄存器地址
	off_StatusTotal,                            //操作寄存器数量
	(uint16_t*)&DC_OutputStatus[off_PowerStatus]     //执行的数据，读取的寄存器数据或写操作的数据
};

static RTU_ReqBlock_t RTU_Req_Setting_Voltage = //RTU数据写请求块-电压设置
{
  LIST_HEAD_INIT(RTU_Req_Setting_Voltage.Entry),
  1,                                          //执行次数，0-无限次
  UART_CHN_AD_SAMPLE,                         //执行通道
	SLAVEID_DC_POWER,                           //从节点站地址
	FUNC_WR_SGREG,                              //功能码
	EXCUTE_SUCCESS,                             //执行结果
	100,                                        //操作寄存器地址
	1,                                          //操作寄存器数量
	(uint16_t*)&DC_SettingValue[off_SetVoltage]     //执行的数据，读取的寄存器数据或写操作的数据
};

static RTU_ReqBlock_t RTU_Req_Setting_Current = //RTU数据写请求块-电流设置
{
  LIST_HEAD_INIT(RTU_Req_Setting_Current.Entry),
  1,                                          //执行次数，0-无限次
  UART_CHN_AD_SAMPLE,                         //执行通道
	SLAVEID_DC_POWER,                           //从节点站地址
	FUNC_WR_SGREG,                              //功能码
	EXCUTE_SUCCESS,                             //执行结果
	101,                                        //操作寄存器地址
	1,                                          //操作寄存器数量
	(uint16_t*)&DC_SettingValue[off_SetCurrent]     //执行的数据，读取的寄存器数据或写操作的数据
};

static RTU_ReqBlock_t RTU_Req_Setting_OutStatus = //RTU数据写请求块-输出状态设置
{
  LIST_HEAD_INIT(RTU_Req_Setting_OutStatus.Entry),
  1,                                          //执行次数，0-无限次
  UART_CHN_AD_SAMPLE,                         //执行通道
	SLAVEID_DC_POWER,                           //从节点站地址
	FUNC_WR_SGREG,                              //功能码
	EXCUTE_SUCCESS,                             //执行结果
	110,                                        //操作寄存器地址
	1,                                          //操作寄存器数量
	(uint16_t*)&DC_SettingValue[off_SetOutSts]     //执行的数据，读取的寄存器数据或写操作的数据
};

static RTU_ReqBlock_t RTU_Req_Setting_RunMode = //RTU数据写请求块-电源运行模式
{
  LIST_HEAD_INIT(RTU_Req_Setting_RunMode.Entry),
  1,                                          //执行次数，0-无限次
  UART_CHN_AD_SAMPLE,                         //执行通道
	SLAVEID_DC_POWER,                           //从节点站地址
	FUNC_WR_SGREG,                              //功能码
	EXCUTE_SUCCESS,                             //执行结果
	111,                                        //操作寄存器地址
	1,                                          //操作寄存器数量
	(uint16_t*)&DC_SettingValue[off_SetRunMode]     //执行的数据，读取的寄存器数据或写操作的数据
};

static RTU_ReqBlock_t RTU_Req_Setting_PowerReset = //RTU数据写请求块-电源复位
{
  LIST_HEAD_INIT(RTU_Req_Setting_PowerReset.Entry),
  1,                                          //执行次数，0-无限次
  UART_CHN_AD_SAMPLE,                         //执行通道
	SLAVEID_DC_POWER,                           //从节点站地址
	FUNC_WR_SGREG,                              //功能码
	EXCUTE_SUCCESS,                             //执行结果
	112,                                        //操作寄存器地址
	1,                                          //操作寄存器数量
	(uint16_t*)&DC_SettingValue[off_SetPowerReset]     //执行的数据，读取的寄存器数据或写操作的数据
};

static RTU_ReqBlock_t RTU_Req_Setting_PowerCtrlMode = //RTU数据写请求块-电源远端或本地控制
{
  LIST_HEAD_INIT(RTU_Req_Setting_PowerCtrlMode.Entry),
  1,                                          //执行次数，0-无限次
  UART_CHN_AD_SAMPLE,                         //执行通道
	SLAVEID_DC_POWER,                           //从节点站地址
	FUNC_WR_SGREG,                              //功能码
	EXCUTE_SUCCESS,                             //执行结果
	113,                                        //操作寄存器地址
	1,                                          //操作寄存器数量
	(uint16_t*)&DC_SettingValue[off_SetPowerCtrlMode]     //执行的数据，读取的寄存器数据或写操作的数据
};


/********************************************************************************/
/*函数名：  HF_SampleBoard_Init                                                             */
/*功能说明：模块初始化函数                                                        */
/*输入参数：无*/
/*输出参数：无                                                                   */
/********************************************************************************/
void DC_POWER_Init(void)
{//模块初始化
  RTU_AddReqBlock(&rtu_ctx[RTU_Req_Read_DC_OutputStatus.chnindex],&RTU_Req_Read_DC_OutputStatus);
}

//输出电压值
//参数值：存储当前读回的电压值
//返回值：主板和电源通讯正常则为TRUE，否则为FALSE
uint8_t Read_DC_Power_Output_Voltage(uint16_t* Voltage)
{
  uint8_t ret=FALSE;
  if(RTU_Req_Read_DC_OutputStatus.Status == EXCUTE_SUCCESS)
  {
    ret = TRUE;
  }
  *Voltage = DC_OutputStatus[off_PowerStatus];
  return ret;
}

/********************************************************************************/
/*函数名：  Task_MBRTU_Master                                                       */
/*功能说明：RTU master主task, 分时调用，减少CPU load                                         */
/*输入参数：无                                                                        */
/*输出参数：无                                                                        */
/*******************************************************************************/
void DC_POWER_MainFunction(void *p_arg)
{
  uint8_t ret = FALSE;
  uint16_t read_val=0;
  
  ret = Read_DC_Power_Setting_Voltage(&read_val);
  if(ret == TRUE)
  {
    DC_SettingValue[off_SetVoltage] = read_val;
    RTU_AddReqBlock(&rtu_ctx[RTU_Req_Setting_Voltage.chnindex],&RTU_Req_Setting_Voltage);
  }

  ret = Read_DC_Power_Setting_Current(&read_val);
  if(ret == TRUE)
  {
    DC_SettingValue[off_SetCurrent] = read_val;
    RTU_AddReqBlock(&rtu_ctx[RTU_Req_Setting_Current.chnindex],&RTU_Req_Setting_Current);
  }

  ret = Read_DC_Power_Setting_OutputStatus(&read_val);
  if(ret == TRUE)
  {
    DC_SettingValue[off_SetOutSts] = read_val;
    RTU_AddReqBlock(&rtu_ctx[RTU_Req_Setting_OutStatus.chnindex],&RTU_Req_Setting_OutStatus);
  }

  ret = Read_DC_Power_Setting_RunMode(&read_val);
  if(ret == TRUE)
  {
    DC_SettingValue[off_SetRunMode] = read_val;
    RTU_AddReqBlock(&rtu_ctx[RTU_Req_Setting_RunMode.chnindex],&RTU_Req_Setting_RunMode);
  }

  ret = Read_DC_Power_Setting_PowerReset(&read_val);
  if(ret == TRUE)
  {
    DC_SettingValue[off_SetPowerReset] = read_val;
    RTU_AddReqBlock(&rtu_ctx[RTU_Req_Setting_PowerReset.chnindex],&RTU_Req_Setting_PowerReset);
  }

  ret = Read_DC_Power_Setting_PowerCtrlMode(&read_val);
  if(ret == TRUE)
  {
    DC_SettingValue[off_SetPowerCtrlMode] = read_val;
    RTU_AddReqBlock(&rtu_ctx[RTU_Req_Setting_PowerCtrlMode.chnindex],&RTU_Req_Setting_PowerCtrlMode);
  }
}


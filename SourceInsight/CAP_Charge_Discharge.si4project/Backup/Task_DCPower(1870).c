#include "config.h"
#include "cmsis_os.h"
#include "interface.h"
#include "string.h"
#include "Task_DCPower.h"


#if 0
static RTU_ReqBlock_t RTU_Req_ReadWeight_30 = //RTU数据读请求块-毛重 1
{
  LIST_HEAD_INIT(RTU_Req_ReadWeight_30.Entry),
  0,                                          //执行次数，0-无限次
  UART_CHN_AD_SAMPLE,                     //执行通道
	SLAVEID_SAMPLE_BOARD,                             //从节点站地址
	FUNC_RD_HOLDREG,                            //功能码
	EXCUTE_SUCCESS,                             //执行结果
	30,                                        //操作寄存器地址
	0x02,                                       //操作寄存器数量
	(u16*)&Weight_Sensor_Data[off_Weight_H]               //执行的数据，读取的寄存器数据或写操作的数据
};
#endif


/********************************************************************************/
/*函数名：  HF_SampleBoard_Init                                                             */
/*功能说明：模块初始化函数                                                        */
/*输入参数：无*/
/*输出参数：无                                                                   */
/********************************************************************************/
void DC_POWER_Init(void)
{//模块初始化
  //RTU_AddReqBlock(&rtu_ctx[RTU_Req_ReadCurrent_Thres.chnindex],&RTU_Req_ReadCurrent_Thres);
}

/********************************************************************************/
/*函数名：  Task_MBRTU_Master                                                       */
/*功能说明：RTU master主task, 分时调用，减少CPU load                                         */
/*输入参数：无                                                                        */
/*输出参数：无                                                                        */
/*******************************************************************************/
void DC_POWER_MainFunction(void *p_arg)
{

}


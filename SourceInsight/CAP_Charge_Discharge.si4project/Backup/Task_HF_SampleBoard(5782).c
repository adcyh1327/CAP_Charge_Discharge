#include "config.h"
//#include "cmsis_os.h"
#include "string.h"
#include "Task_HF_SampleBoard.h"


static struct RTU_ReqBlock RTU_Req_ReadVoltage_Thres= //RTU脢媒戮脻脟毛脟贸驴茅,露脕脠隆碌卤脟掳碌莽禄煤脭脣脨脨鹿娄脗脢
{
	LIST_HEAD_INIT(RTU_Req_ReadVoltage_Thres.Entry),
  0,                                          //鎵ц娆℃暟锛?-鏃犻檺娆?
	UART_CHN_AD_SAMPLE,                      	//鎵ц閫氶亾
	0x01,                                       //浠庤妭鐐圭珯鍦板潃
	FUNC_RD_HOLDREG,                            //鍔熻兘鐮?3
	EXCUTE_SUCCESS,                             //鎵ц缁撴灉,鐢ㄤ簬璇?
	0x0005,                                     //鎿嶄綔瀵勫瓨鍣ㄥ湴鍧�
	0x01,                                       //鎿嶄綔瀵勫瓨鍣ㄦ暟閲?
	(uint16_t*)&VoltageData[0]      	//鎵ц鐨勬暟鎹紝璇诲彇鐨勫瘎瀛樺櫒鏁版嵁鎴栧啓鎿嶄綔鐨勬暟鎹?
};


void Platform_Init(void)
{
	uint8_t i;
  for(i=0;i<NUM_UARTCHANNEL;i++)
	{
			if(RTU_CHN_ENABLE[i] == TRUE)
			{
					RTU_Init(i);//绗竴涓弬鏁拌〃绀哄彂閫侀棿闅旀椂闂达紝绗簩涓弬鏁拌〃绀鸿秴鏃舵椂闂?
			}
	}
  UartOpFunc[USART_1]._send = USART1_Send_Data;
  UartOpFunc[USART_2]._send = USART2_Send_Data;
  UartOpFunc[USART_3]._send = USART3_Send_Data;
  UartOpFunc[UART_5]._send = UART5_Send_Data;
  UartOpFunc[USART_6]._send = USART6_Send_Data;
  RTU_AddReqBlock(&rtu_ctx[RTU_Req_ReadCurrent_Thres.chnindex],&RTU_Req_ReadCurrent_Thres);
	RTU_AddReqBlock(&rtu_ctx[RTU_Req_ReadVoltage_Thres.chnindex],&RTU_Req_ReadVoltage_Thres);
}


	
/********************************************************************************/
/*函数名：  HF_SampleBoard_Init                                                             */
/*功能说明：模块初始化函数                                                        */
/*输入参数：无*/
/*输出参数：无                                                                   */
/********************************************************************************/
void HF_SampleBoard_Init(void)
{//模块初始化

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


#include "config.h"
#include "cmsis_os.h"
#include "interface.h"
#include "string.h"
#include "Task_APPLogic.h"


/*
MACRO
*/
#define IO_TEST_CAP_NORMAL_OUT_HI() 
#define IO_TEST_CAP_NORMAL_OUT_LO() 

#define IO_TEST_DEVICE_ABNORMAL_OUT_HI()    
#define IO_TEST_DEVICE_ABNORMAL_OUT_LO()    

#define IO_DEVICE_WARNING_INDCR_OUT_HI()    LED3_ON
#define IO_DEVICE_WARNING_INDCR_OUT_LO()    LED3_OFF

#define IO_DEVICE_RUNNING_INDCR_OUT_HI()    LED2_ON
#define IO_DEVICE_RUNNING_INDCR_OUT_LO()    LED2_OFF

/*
静态全局
*/
static uint8_t deviceEnabled = FALSE; /* 使能信号 */
static uint8_t deviceStarted = FALSE; /* 启动信号 */
static uint8_t deviceAbnormal = FALSE; /* 设备异常信号 */
static uint8_t testCapNormal = FALSE; /* 被测电容良品信号 */
static uint8_t deviceWarningIndcr = FALSE; /* 设备报警指示灯 */
static uint8_t deviceRunningIndcr = FALSE; /* 设备运行指示灯 */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

}


/********************************************************************************/
/*函数名：  APP_Logic_Init                                                             */
/*功能说明：模块初始化函数                                                        */
/*输入参数：无*/
/*输出参数：无                                                                   */
/********************************************************************************/
void APP_Logic_Init(void)
{//模块初始化
  //RTU_AddReqBlock(&rtu_ctx[RTU_Req_ReadCurrent_Thres.chnindex],&RTU_Req_ReadCurrent_Thres);
}

/********************************************************************************/
/*函数名：  Task_APP_Logic                                                       */
/*功能说明：RTU master主task, 分时调用，减少CPU load                                         */
/*输入参数：无                                                                        */
/*输出参数：无                                                                        */
/*******************************************************************************/
void APP_Logic_MainFunction(void *p_arg)
{
    app_get_device_enable_signal();
    app_get_device_start_signal();



    app_device_warning_indicator_out();
    app_device_warning_indicator_out();
    app_test_cap_normal_signal_out();
    app_device_abnormal_signal_out();
}

/*
周期获取使能硬线信号
*/
uint8_t app_get_device_enable_signal(void)
{
    GPIO_PinState enable_status_pin = GPIO_PIN_RESET;
    enable_status_pin = READ_ENABLE_STATUS;

    if(GPIO_PIN_SET == enable_status_pin)
    {
        deviceEnabled = TRUE;
    }
    else
    {
        deviceEnabled = FALSE;
    }
    return deviceEnabled;
}
/*
周期获取启动硬线信号
*/
uint8_t app_get_device_start_signal(void)
{
    GPIO_PinState start_status_pin = GPIO_PIN_RESET;
    start_status_pin = READ_START_STATUS;

    if((GPIO_PIN_SET == start_status_pin)
        &&(FALSE == deviceAbnormal))
    {
        deviceStarted = TRUE;
    }
    else
    {
        deviceStarted = FALSE;
    }
    return deviceStarted;
}

/*
良品信号、硬线输出控制
*/
uint8_t app_test_cap_normal_signal_out(void)
{
	  if(TRUE == testCapNormal)
    {
        IO_TEST_CAP_NORMAL_OUT_HI();
    }
    else
    {
        IO_TEST_CAP_NORMAL_OUT_LO();
    }
    return TRUE;
}
/*
异常信号硬线输出控制
*/
uint8_t app_device_abnormal_signal_out(void)
{
	if(TRUE == deviceAbnormal)
    {
        IO_TEST_DEVICE_ABNORMAL_OUT_HI();
    }
    else
    {
        IO_TEST_DEVICE_ABNORMAL_OUT_LO();
    }
    return TRUE;
}
/*
设备报警指示灯输出控制
*/
uint8_t app_device_warning_indicator_out(void)
{
	if(TRUE == deviceWarningIndcr)
    {
        IO_DEVICE_WARNING_INDCR_OUT_HI();
    }
    else
    {
        IO_DEVICE_WARNING_INDCR_OUT_LO();
    }
    return TRUE;
}
/*
设备运行指示灯输出控制
*/
uint8_t app_device_running_indicator_out(void)
{
	if(TRUE == deviceRunningIndcr)
    {
        IO_DEVICE_RUNNING_INDCR_OUT_HI();
    }
    else
    {
        IO_DEVICE_RUNNING_INDCR_OUT_LO();
    }
    return TRUE;
}

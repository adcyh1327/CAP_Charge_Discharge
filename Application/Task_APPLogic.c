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
��̬ȫ��
*/
static uint8_t deviceEnabled = FALSE; /* ʹ���ź� */
static uint8_t deviceStarted = FALSE; /* �����ź� */
static uint8_t deviceAbnormal = FALSE; /* �豸�쳣�ź� */
static uint8_t testCapNormal = FALSE; /* ���������Ʒ�ź� */
static uint8_t deviceWarningIndcr = FALSE; /* �豸����ָʾ�� */
static uint8_t deviceRunningIndcr = FALSE; /* �豸����ָʾ�� */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

}


/********************************************************************************/
/*��������  APP_Logic_Init                                                             */
/*����˵����ģ���ʼ������                                                        */
/*�����������*/
/*�����������                                                                   */
/********************************************************************************/
void APP_Logic_Init(void)
{//ģ���ʼ��
  //RTU_AddReqBlock(&rtu_ctx[RTU_Req_ReadCurrent_Thres.chnindex],&RTU_Req_ReadCurrent_Thres);
}

/********************************************************************************/
/*��������  Task_APP_Logic                                                       */
/*����˵����RTU master��task, ��ʱ���ã�����CPU load                                         */
/*�����������                                                                        */
/*�����������                                                                        */
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
���ڻ�ȡʹ��Ӳ���ź�
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
���ڻ�ȡ����Ӳ���ź�
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
��Ʒ�źš�Ӳ���������
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
�쳣�ź�Ӳ���������
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
�豸����ָʾ���������
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
�豸����ָʾ���������
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

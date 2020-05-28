#ifndef APP_LOGIC_H
#define APP_LOGIC_H




void APP_Logic_Init(void);
void APP_Logic_MainFunction(void *p_arg);
uint8_t app_device_warning_indicator_out(void);
uint8_t app_device_warning_indicator_out(void);
uint8_t app_test_cap_normal_signal_out(void);
uint8_t app_device_abnormal_signal_out(void);
uint8_t app_get_device_enable_signal(void);
uint8_t app_get_device_start_signal(void);

#endif

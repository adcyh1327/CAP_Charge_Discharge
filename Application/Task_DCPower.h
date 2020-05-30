#ifndef DC_POWER_H
#define DC_POWER_H			  


#define SLAVEID_DC_POWER                  0x01

void DC_POWER_Init(void);
uint8_t Read_DC_Power_Output_Voltage(uint16_t* Voltage);

void DC_POWER_MainFunction(void *p_arg);

#endif

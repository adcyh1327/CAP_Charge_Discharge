#ifndef __CONFIG_H_
#define __CONFIG_H_

#define APP_CRC_ADDR   0x308000
#define SER_PDU_SIZE_MAX 500u



#define OS_ENTER_CRITICAL() 								taskENTER_CRITICAL()
#define OS_EXIT_CRITICAL() 									taskEXIT_CRITICAL()

#define ON   														1u
#define OFF 														0u

#define TRUE  													1u
#define FALSE 													0u


#endif



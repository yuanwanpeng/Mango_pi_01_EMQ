/*
 * uart.h
 *
 *  Created on: 2018��7��24��
 *      Author: Administrator
 */

#ifndef INC_UART_H_
#define INC_UART_H_
#include "stdint.h"
#include <string.h>
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "SIM800c.h"
//#include "dma.h"
#define MAX_RECV_LEN 256



extern SemaphoreHandle_t xSemaphore;

void Send_To_Uart2_Str(int8_t* str,uint32_t len);

char *itoa(char *string, int32_t value, int radix);




#endif /* INC_UART_H_ */



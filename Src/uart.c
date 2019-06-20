/*
 * uart.c
 *
 *  Created on: 2018��7��24��
 *      Author: Administrator
 */
#include "uart.h"
#include <stdarg.h>
#include "cmsis_os.h"
#include "semphr.h"
#define PDATA_SIZE 256
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void USARTx_Printf(UART_HandleTypeDef* huartx, char *Data,...);

uint8_t pData[PDATA_SIZE];
uint8_t aRxBuffer;			//�����жϻ���
uint8_t Uart1_RxBuff[64];		//���ջ���
uint8_t Uart1_Rx_Cnt = 0;		//���ջ������
uint8_t	cAlmStr[] = "�������(����256)\r\n";

uint8_t a2RxBuffer;			//�����жϻ���
uint8_t Uart2_RxBuff[MAX_RECV_LEN];		//���ջ���
uint8_t Uart2_Rx_Cnt = 0;		//���ջ������
extern uint8_t *G_p_index_start,*G_p_index_end;

void Reset_Uart_DMA(void)
{
	HAL_UART_DMAStop(&huart2);
	memset(pData,'\0',PDATA_SIZE);
	HAL_UART_Receive_DMA(&huart2, pData, PDATA_SIZE);
	G_p_index_start = pData;
	G_p_index_end = pData;
}


/*
 *     ����2����
 */
void Send_To_Uart2_Str(int8_t* str,uint32_t len)
{	
	while(HAL_UART_Transmit_DMA(&huart2, (uint8_t*)str, len) != HAL_OK)
	{
		osDelay(100);
	}
	osDelay(100);
}





/********************************************************************************
 *  @brief  HAL_UART_RxCpltCallback  �����жϺ�HAL�������������
* 	@param  ���ھ��
*	 @retval NONE
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	BaseType_t xHigherPriorityTaskWoken;
	if(USART1 == huart->Instance)//���Դ���1
	{
		if(Uart1_Rx_Cnt >= (MAX_RECV_LEN+1))  //����ж�
		{
			Uart1_Rx_Cnt = 0;
			memset(Uart1_RxBuff,0x00,sizeof(Uart1_RxBuff));
			HAL_UART_Transmit(&huart1, (uint8_t *)&cAlmStr, sizeof(cAlmStr),0xFFFF);
		}else{
			Uart1_RxBuff[Uart1_Rx_Cnt++] = aRxBuffer;   //��������ת��

			if((Uart1_RxBuff[Uart1_Rx_Cnt-1] == 0x0A)&&(Uart1_RxBuff[Uart1_Rx_Cnt-2] == 0x0D)) //�жϽ���λ
			{

				xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
				//HAL_UART_Transmit(&huart1, (uint8_t *)&Uart1_RxBuff, Uart1_Rx_Cnt,0xFFFF); //���յ�����Ϣ���ͳ�ȥ
				//Uart1_Rx_Cnt = 0;
				//memset(Uart1_RxBuff,0x00,sizeof(Uart1_RxBuff)); //�������
			}
		}
		HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);   //�ٿ��������ж�
	}

	if(USART2 == huart->Instance)//wifiģ�鴮��2
	{
//		if(Uart2_Rx_Cnt >= (MAX_RECV_LEN+1))  //����ж�
//		{
//			Uart2_Rx_Cnt = 0;
//			memset(Uart2_RxBuff,0x00,sizeof(Uart2_RxBuff));
//			//HAL_UART_Transmit(&huart1, (uint8_t *)&cAlmStr, sizeof(cAlmStr),0xFFFF);//���͸�����һ
//		}else{
//			Uart2_RxBuff[Uart2_Rx_Cnt++] = a2RxBuffer;   //��������ת��**********�ص�

//			if(a2RxBuffer == 0x0D)
//			{	
//					xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
//			}
//		}
//		HAL_UART_Receive_IT(&huart2, (uint8_t *)&a2RxBuffer, 1);   //�ٿ��������ж�
	}
}



/****************************************************************************
* ������	:	itoa
* ����  :	����������ת�����ַ���
* ����  :	-radix =10 ��ʾ10���ƣ��������Ϊ0
*        	-value Ҫת����������
*        	-buf ת������ַ���
* ���  :	��
* ����  :	��
* ����  :	��USARTx_printf()����
***************************************************************************/
char *itoa(char *string, int32_t value, int radix)
{
    int32_t     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 1000000000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

void USARTx_Send_Dat(UART_HandleTypeDef* huartx, uint16_t dat)
{
	//USART_SendData(USARTx,dat);
	HAL_UART_Transmit(huartx, (uint8_t*)&dat, 1, 0xFFFF);
	while( __HAL_UART_GET_FLAG(huartx, UART_FLAG_TXE) == RESET );
	//while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
}

/****************************************************************************
*	������	:	USARTx_Printf
*	����	:	��ʽ�������������C���е�printf��������û���õ�C��
* ����	:	-USARTx ����ͨ��������ֻ�õ��˴���2����USART2
*		     	-Data   Ҫ���͵����ڵ����ݵ�ָ��
*			  	 -...   ��������
* ���	:	��
* ����	:	�� 
* ����	:	�ⲿ����
*					����Ӧ��	USARTx_printf( USART1, "\r\n this is a demo \r\n" );
*									USARTx_printf( USART2, "\r\n %d \r\n", i );
*									USARTx_printf( USART3, "\r\n %s \r\n", j );
****************************************************************************/
void USARTx_Printf(UART_HandleTypeDef* huartx, char *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5C )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
					USARTx_Send_Dat(huartx, 0x0D);
					Data ++;
					break;

				case 'n':							          //���з�
					USARTx_Send_Dat(huartx, 0x0A);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{	
			switch ( *++Data )
			{				
				case 's':										  //�ַ���
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
						USARTx_Send_Dat(huartx,*s);
					Data++;
          break;

        case 'd':										 //ʮ����
          d = va_arg(ap, int);
          itoa(buf ,d, 10);
          for (s = buf; *s; s++) 
						USARTx_Send_Dat(huartx,*s);
					Data++;
          break;
				
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else 
			USARTx_Send_Dat(huartx, *Data++);
	}
}



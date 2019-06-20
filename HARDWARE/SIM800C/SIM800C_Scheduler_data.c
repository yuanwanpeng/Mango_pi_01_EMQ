/*
 * SIM800C_Scheduler_data.c
 *
 *  Created on: 2019��5��5��
 *      Author: Administrator
 */

#include "SIM800C_Scheduler_data.h"
#include <string.h>
#include "SIM800C.h"
extern uint8_t pData[];
extern uint8_t G_Sim800C_Signal;
extern char Send_data_recv[];
extern uint8_t Recv_cmd_data[];
extern osThreadId Start_Scheduler_data_Task_TaskHandle;
extern osThreadId Start_Send_State_data_TaskHandle;	//�������ݵ���λ�����
extern osThreadId Start_Recv_Onenet_data_TaskHandle;	//�������ݵ���λ�����
char *G_p_index_start = pData,*G_p_index_end = pData;			//��ʾһ֡���ݵ�ǰ��β

/*
 *���յ�SEND OK\r\n ����
 */
void Recv_SEND_OK_Opt(void)
{
	BaseType_t err;
	memset(Send_data_recv,'\0',32);
	strcpy(Send_data_recv,"\r\nSEND OK\r\n");
	
	taskENTER_CRITICAL();				//�����ٽ���
	G_p_index_start = strstr((const char*)G_p_index_end,"\r\nSEND OK\r\n");
	if(G_p_index_end < pData)
	{
		printf("Recv_SEND_OK_Opt =addr= 0x04");
	}
	G_p_index_end = strstr((const char*)G_p_index_end,"\r\nSEND OK\r\n");
	if(G_p_index_end < pData)
	{
		printf("Recv_SEND_OK_Opt =addr= 0x04");
	}
	/*��������û������*/
	uint8_t end_data_len;	//��β������ݳ���
	G_p_index_end += (strlen("\r\nSEND OK\r\n"));	//���һ���ֽ�\r\nSEND OK\r\n_����׸�λ��
	end_data_len = strlen((const char*)G_p_index_end);			//�жϵ׸ܺ�����û������
	
	
	if(Start_Send_State_data_TaskHandle != NULL)
	{
		err = xTaskNotify((TaskHandle_t)Start_Send_State_data_TaskHandle,
			(uint32_t)RCV_SEND_OK,
			(eNotifyAction)eSetValueWithOverwrite);//����SEND OK�ź�
	}
	if(end_data_len == 0)	//(=0��ʾOK\r\n ֮��һ���ֽ����ݵ�ַ)�ж��Ƿ����0 0 ��ʾ����û������
	{
		Reset_Uart_DMA();
	}//��������� Ҳû�й�ϵ���жϵ�ʱ���G_p_index_end��ʼ�ж�;
	taskEXIT_CRITICAL();				//�˳��ٽ���
}
/*
 * ���յ��ź�ǿ���źŲ���
 */
void Recv_CSQ_Opt(void)
{
	char *str = strstr((const char*)G_p_index_end,"\r\n+CSQ: ");//������ź���Ϣ
	G_Sim800C_Signal = atoi(str+7);
//	free(str);
//	xTaskNotify((TaskHandle_t)Start_SIM800C_TaskHandle,//�������������߳�
//		(uint32_t)RCV_OK,
//		(eNotifyAction)eSetValueWithOverwrite);
	taskENTER_CRITICAL();				//�����ٽ���
	G_p_index_start = strstr((const char*)G_p_index_end,"\r\n+CSQ: ");
	G_p_index_end = strstr((const char*)G_p_index_end,"OK\r\n");

//	if(p_index == pData)//�ж�"\r\n+CSQ:"���׵�ַ֪����pData����ȱ�ʾ����һ���ǵ�һ���ֽ�
//	{
//		memset(pData+p_index,"\0",p_index);
//	}
	/*��������û������*/
	uint8_t end_data_len;	//��β������ݳ���
//	printf("G_p_index_end = %s\r\n",G_p_index_end);
	G_p_index_end += (strlen("OK\r\n")+1);	//���һ���ֽ�OK\r\n_����׸�λ��
	end_data_len = strlen((const char*)G_p_index_end);	//�жϵ׸ܺ�����û������
//	printf("end_data_len = %d,G_p_index_end = %s,strlen = %d\r\n",end_data_len,G_p_index_end,(strlen("OK\r\n")+1));
	
	if(end_data_len == 0)	//(��ʾOK\r\n ֮��һ���ֽ����ݵ�ַ)�ж��Ƿ����0 0 ��ʾ����û������
	{
//		printf("Recv_CSQ_Opt Reset_Uart_DMA\r\n");
		Reset_Uart_DMA();
	}//��������� Ҳû�й�ϵ���жϵ�ʱ���G_p_index_end��ʼ�ж�;
	taskEXIT_CRITICAL();				//�˳��ٽ���
}
/*
 *	\r\n> ���Ų���
 */
void Recv_Symbol_Opt(void)
{
	BaseType_t err;
	char *str;
	memset(Send_data_recv,'\0',128);
	strcpy(Send_data_recv,">");
	taskENTER_CRITICAL();				//�����ٽ���
	G_p_index_start = strstr((const char*)G_p_index_end,"\r\n>");
		if(G_p_index_end < pData)
	{
		printf("Recv_Symbol_Opt =addr= 0x04");
	}
	G_p_index_end = strstr((const char*)G_p_index_end,"\r\n> ");
	if(G_p_index_end < pData)
	{
		printf("Recv_Symbol_Opt =addr= 0x04");
	}
	/*��������û������*/
	uint8_t end_data_len;	//��β������ݳ���
	G_p_index_end += (strlen("\r\n> "));	//���һ���ֽ�OK\r\n_����׸�λ��
	end_data_len = strlen((const char*)G_p_index_end);	//�жϵ׸ܺ�����û������
	
	err = xTaskNotify((TaskHandle_t)Start_Send_State_data_TaskHandle,//�������������߳�
		(uint32_t)RCV_WRITE_SYMBEL,
		(eNotifyAction)eSetValueWithOverwrite);
//	printf(">G_p_index_end = %s,end_data_len = %d,err = %d\r\n",G_p_index_end,end_data_len,err);
	if(end_data_len == 0)	//(��ʾOK\r\n ֮��һ���ֽ����ݵ�ַ)�ж��Ƿ����0 0 ��ʾ����û������
	{
//		printf("Reset_Uart_DMA\r\n");
		Reset_Uart_DMA();
	}//��������� Ҳû�й�ϵ���жϵ�ʱ���G_p_index_end��ʼ�ж�;
	
	taskEXIT_CRITICAL();				//�˳��ٽ���
}
/*
 *  ���յ�������'0'��ͷ
 */
void Recv_Data_Opt(void)
{
	//������ݽ�������
	uint8_t i = 0;
	uint32_t encodedByte;
	uint8_t check_len = strlen(G_p_index_end+1);//���ʹ�ü����ֽڵó����ݳ����4�ֽ�
	uint8_t mqtt_len[4] = {0x00,0x00,0x00,0x00};
	uint16_t TopicNameLen = 0x0000;
	if(check_len<=4){
		strcpy((char*)mqtt_len,(const char*)(G_p_index_end+1));
		uint32_t multiplier = 1;
		uint32_t value = 0;
		do{
			encodedByte = mqtt_len[i++];
			value += (encodedByte & 127) * multiplier;
			multiplier *= 128;
			if (multiplier > 128*128*128)
				break;
		}while ((encodedByte & 128) != 0);
		printf("value = %d\r\n",value);//���ݳ���
		TopicNameLen |= (G_p_index_end[check_len+1]<<8);	//��һλ
		TopicNameLen |= G_p_index_end[check_len+2];			//�ڶ�λ
		printf("TopicNameLen = %d\r\n",TopicNameLen);
	//	printf("rcv_dat = %s\r\n",(pData+1+check_len+2+TopicNameLen));
		strcpy(Recv_cmd_data,(const char*)(G_p_index_end+1+check_len+2+TopicNameLen));
		G_p_index_end += strlen((const char*)(G_p_index_end+1+check_len+2+TopicNameLen))+1;//����ָ���±굽���һ���ֽڵ���һ���ֽ�
		//root = cJSON_Parse((const char*)(pData+1+check_len+2+TopicNameLen));
		xTaskNotify((TaskHandle_t)Start_Recv_Onenet_data_TaskHandle,
					(uint32_t)RCV_CMD_DAT,
					(eNotifyAction)eSetValueWithOverwrite);

		uint8_t end_data_len = strlen((const char*)G_p_index_end);	//�жϵ׸ܺ�����û������

		taskENTER_CRITICAL();				//�����ٽ���
		if(end_data_len == 0)	//(��ʾOK\r\n ֮��һ���ֽ����ݵ�ַ)�ж��Ƿ����0 0 ��ʾ����û������
		{
			Reset_Uart_DMA();
		}//��������� Ҳû�й�ϵ���жϵ�ʱ���G_p_index_end��ʼ�ж�;
		taskEXIT_CRITICAL();				//�˳��ٽ���
	}else{
	}
}
/*
 * ����sim800C�������ݵ�task
 */
void Start_Scheduler_data_Task(void const * argument)
{
	char *str;
	Reset_Uart_DMA();
	while(1)
	{
		//���pData����  G_p_index_endָ����Զָ��һ֡���������ݵ����Ժ�һ���ֽ�
		if(strstr((const char *)G_p_index_end,"OK\r\n"))	//���յ���OK
		{
			str = strstr((const char*)G_p_index_end,"\r\n+CSQ: ");//������ź���Ϣ
			if(str != NULL)
			{
				printf("into csq = %s\r\n",G_p_index_end);
				Recv_CSQ_Opt();			//���յ��ź�ǿ���źŲ���
			}
			str = strstr((const char*)G_p_index_end,"\r\nSEND OK\r\n");//������ź���Ϣ
			if(str != NULL)
			{
				Recv_SEND_OK_Opt();		//���յ�SEND OK����
			}
		}
		if(strstr((const char*)G_p_index_end,"\r\n> "))//������>����
		{
			//printf("pData> = %s\r\n",pData);
			str = strstr((const char*)G_p_index_end,"\r\n> ");//����Ǵ����ͷ�����Ϣ
			if(str != NULL)
			{
				Recv_Symbol_Opt();
			}
		}
		if(G_p_index_end[0] == '0')//0x30��ͷ
		{
			Recv_Data_Opt();
		}
	}
}



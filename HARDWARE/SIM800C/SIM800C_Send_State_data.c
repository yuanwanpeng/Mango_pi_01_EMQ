/*
 * SIM800C_Send_State_data.c
 *
 *  Created on: 2019��5��1��
 *      Author: Administrator
 */
#include "SIM800C.h"
#include "SIM800C_Send_State_data.h"
#include "SIM800C_Scheduler_data.h"
#include "main.h"
#include "HUMI.h"
#include "relay.h"
#include "Mqtt.h"
#include "GPRS.h"
#include "LCD12864.h"
extern uint8_t g_send_Onenet_buf[];
extern osThreadId Start_Reset_Sim800c_Task_TaskHandle;
extern osThreadId Start_Send_State_data_TaskHandle;
extern Delete_Task_struct G_Delete_Task_struct;//ɾ������Ľṹ��
uint8_t g_init_send = ERROR;
void Start_Send_State_data_Task(void const * argument)
{
	xSemaphoreTake(Sim800c_Semaphore,portMAX_DELAY);//��ȡ�����ź���
	Connect_OneNet();	//SIM800C�����ϵ����Ҫ����ONENET��MQTT������
	xSemaphoreGive(Sim800c_Semaphore);
	//�����Ͽ���������ONENET֮��Ҫ�����豸��ϢVersion_Information
	if(g_init_send==ERROR)//ȷ��ֻ�е�һ�ο���ʱ�����豸��Ϣ
	{
		xSemaphoreTake(Sim800c_Semaphore,portMAX_DELAY);//��ȡ�����ź���
		g_init_send = Send_Version_Information();//�����豸��Ϣ
		xSemaphoreGive(Sim800c_Semaphore);
	}
	printf("CONNECT_onenet Send_Version_Information\r\n");
	xSemaphoreGive(Sim800c_Semaphore);
	while(1)
	{
		xSemaphoreTake(Sim800c_Semaphore,portMAX_DELAY);//��ȡ�����ź���
		Send_Temp_Humi_F_R();
		xSemaphoreGive(Sim800c_Semaphore);
		osDelay(5000);
	}
}
/*
 * ���͵�ONENET��ʪ�Ⱥ͵�ǰ����ת״̬
 */
void Send_Temp_Humi_F_R(void)
{
	uint16_t Pack_Len,i = 0;
	BaseType_t err = 1;
	static uint8_t cnt = 0;
	uint32_t NotifyValue;
	uint8_t cmp;

	while(GPRS_AT_CIPSEND(AT_CIPSEND,">")==ERROR)
	{
		printf("Send_Temp_Humi_F_R CIPSEND reset\r\n");
		xSemaphoreGive(Sim800c_Semaphore);
		taskENTER_CRITICAL();				//�����ٽ���
		osThreadDef(Reset_Sim800c_Task, Start_Reset_Sim800c_Task, osPriorityNormal, 0, 128);	//��������Sim800C��ʼ�����Ⱥ���
		Start_Reset_Sim800c_Task_TaskHandle = osThreadCreate(osThread(Reset_Sim800c_Task), NULL);
		taskEXIT_CRITICAL();				//�˳��ٽ���
		//ɾ���Լ�Ҫ���¿�ʼ
		G_Delete_Task_struct.D_Task = Start_Send_State_data_TaskHandle;
		Start_Send_State_data_TaskHandle = NULL;
		G_Delete_Task_struct.sign = ENABLE;
	}
	Pack_Len = init_OneNet_Temp_Humi_F_R_Pack();
	Send_To_Uart2_Str((int8_t*)g_send_Onenet_buf,Pack_Len);
	//���ҵȴ����SEND OK��ʱ�����ò�ѯ�źŷ�������
	err = xTaskNotifyWait((uint32_t)0x00,
			(uint32_t)0xffffffff,
			(uint32_t*)&NotifyValue,
			(TickType_t)15000);//����15����ʱ��
	cmp = strcmp(Send_data_recv,"\r\nSEND OK\r\n");//�Ա��ǲ����������
	if(err == pdFALSE){
		cnt++;
		if(cnt == 3){
			cnt = 0;
			printf("Send_Temp_Humi_F_R = err = %d\r\n",err);
			xSemaphoreGive(Sim800c_Semaphore);
			taskENTER_CRITICAL();				//�����ٽ���
			osThreadDef(Reset_Sim800c_Task, Start_Reset_Sim800c_Task, osPriorityNormal, 0, 128);	//��������Sim800C��ʼ�����Ⱥ���
			Start_Reset_Sim800c_Task_TaskHandle = osThreadCreate(osThread(Reset_Sim800c_Task), NULL);
			taskEXIT_CRITICAL();	
			//ɾ���Լ�Ҫ���¿�ʼ
			G_Delete_Task_struct.D_Task = Start_Send_State_data_TaskHandle;
			Start_Send_State_data_TaskHandle = NULL;
			G_Delete_Task_struct.sign = ENABLE;
		}
		//return ERROR;
	}else if((err == pdTRUE)&&(NotifyValue == RCV_SEND_OK)&&(cmp == 0)){
		cnt = 0;
	//	return SUCCESS;
	}
}

/*
 * ��ʪ�Ⱥ�����ת״̬���ع�ȥ
 */
uint16_t init_OneNet_Temp_Humi_F_R_Pack(void)
{
	uint8_t index = 0;
	uint8_t JsonStr[128];
	G_Send_Pack.Pack_type = 0x30;
	G_Send_Pack.TopicNameLen = 0x0003;
	memcpy(G_Send_Pack.TopicName,"$dp",strlen("$dp"));
	G_Send_Pack.DpType = 0x03;

	sprintf(JsonStr,"{\"Temp\":\"%02d.%d\",\"Humi\":\"%d\"}",(uint16_t)temp/10,(uint16_t)temp%10,(uint16_t)g_rh);

	G_Send_Pack.JsonStr = cJSON_Parse(JsonStr);
	if(!G_Send_Pack.JsonStr){
		printf("get root faild !\r\n");
	}

	cJSON_AddStringToObject(G_Send_Pack.JsonStr,"State","0");

	char* s;
	s = cJSON_PrintUnformatted(G_Send_Pack.JsonStr);
	cJSON_Delete(G_Send_Pack.JsonStr);
	if (G_Send_Pack.JsonStr)
		cJSON_Delete(G_Send_Pack.JsonStr);
	index = Create_Send_Pack(s);
	return index;
}





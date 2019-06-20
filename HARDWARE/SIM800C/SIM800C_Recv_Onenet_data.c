/*
 * SIM800C_Recv_Onenet_data.c
 *
 *  Created on: 2019��5��5��
 *      Author: Administrator
 */


#include "SIM800C_Recv_Onenet_data.h"
#include "main.h"
#include "HUMI.h"
#include "relay.h"
#include "Mqtt.h"
#include "GPRS.h"
#include <string.h>
uint8_t Recv_cmd_data[512];

/*
 * �����������ݵĺ���
 */
void Start_Recv_Onenet_data_Task(void const * argument)
{
	BaseType_t err = 1;
	uint32_t NotifyValue;
	cJSON* item_cmd;
	cJSON* root;
	int8_t* p_data;
	while(1)
	{
		err = xTaskNotifyWait((uint32_t)0x00,
				(uint32_t)0xffffffff,
				(uint32_t*)&NotifyValue,
				(TickType_t)portMAX_DELAY);
		root = cJSON_Parse((const char*)Recv_cmd_data);
		item_cmd = cJSON_GetObjectItem(root, "cmd");//{"cmd":"set_cmd","max_route":"100"}TopicNameLen = 42
		if(item_cmd != NULL){
			p_data = cJSON_Print(item_cmd);//p_data�����������
			printf("pdata = %s\r\n",p_data);

			if(!strcmp(p_data,"\"set_cmd\"")){//ƥ����������������
				free(p_data);p_data=NULL;
				item_cmd = cJSON_GetObjectItem(root, "max_route");//��ȡָ������
				p_data = cJSON_Print(item_cmd);//p_data�����������
				printf("ָ�� = %s\r\n",p_data);
				free(p_data);p_data=NULL;
			}
			if(!strcmp(p_data,"\"exe_cmd\"")){

			}
		}
	}
}












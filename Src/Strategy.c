/*
 * Strategy.c
 *
 *  Created on: 2018��11��28��
 *      Author: YuanWP
 */
#include "Strategy.h"
#include "LCD12864_Display_Menu_Set_Strategy.h"
#include "relay.h"
#include "LCD12864.h"
#define	INIT_HOURS		23	//Ĭ�ϵ�ʱ��
#define	INIT_MINUTES	59
#define	INIT_SECONDS	59
#define INIT_TEMP_H		25	//���¶�
#define INIT_TEMP_L		20	//���¶�
#define INIT_ROUTE		0	//Ĭ���г�
#define	ROUTE_NUM		10

#define NO_CHECK_HOURS	4		//������ʱ���
#define NO_CHECK_MINUTES	00
#define NO_CHECK_SECONDS	00
extern float temp;
extern float g_rh;

/*
 * ��ʼ���ڲ�������Ϣ
 */
void Init_Strategy(uint8_t num)
{
	uint8_t i;
	for(i = num;i < 8;i++)
	{
		S_Strategy[i].Strategy_Num = (i+1);		// ��ʼ��������������
		S_Strategy[i].Time.Hours = INIT_HOURS;
		S_Strategy[i].Time.Minutes = INIT_MINUTES;
		S_Strategy[i].Time.Seconds = INIT_SECONDS;
		S_Strategy[i].Strategy_State = Strategy_Close;
		S_Strategy[i].Temp_h = INIT_TEMP_H;
		S_Strategy[i].Temp_l = INIT_TEMP_L;
		S_Strategy[i].Route = INIT_ROUTE;
		S_Strategy[i].Strategy_Type = Strategy_Type_Route;//Ĭ�Ϸ��ģʽ���
		if(i == 0)
		{
			S_Strategy[i].Strategy_State = Strategy_Open;
			S_Strategy[i].Time.Hours = NO_CHECK_HOURS;	//�������õ�ʱ����ص�
			S_Strategy[i].Time.Minutes = NO_CHECK_MINUTES;
			S_Strategy[i].Time.Seconds = NO_CHECK_SECONDS;
		}
		AT24CXX_Write(i*STRATEGY_ADDR,((uint8_t*)S_Strategy)+(sizeof(S_Strategy[i])*i),sizeof(S_Strategy[i]));	//д�����
	}
	for(i = 0;i < 8;i++)
	{
		AT24CXX_Read(i*STRATEGY_ADDR,((uint8_t*)G_Strategy)+(sizeof(G_Strategy[i])*i),sizeof(G_Strategy[i]));	//��ȡ����
		//printf("r %d,Hours %d,Minutes %d\r\n",i,G_Strategy[i].Time.Hours,G_Strategy[i].Time.Minutes);
	}
	printf("write all Strategy success%d!!\r\n",sizeof(G_Strategy[i]));
	return;
}
/*
 * ��ʼ����������
 */
void Start_Strategy_Task(void const * argument)
{
	RTC_TimeTypeDef Time;
	uint8_t i,Old_Strategy = 0,Mark = 0,Strategy_mode = 0;
	uint16_t Min_Time,NEXT_Min_Time,NEW_Route;
	uint32_t Delay_Time;
	osDelay(6000);
	while(1)
	{
		if(G_New_Mode != MANUAL_MODE)//0���ֶ�ģʽ ��0��ʾ���Ժ�
		{
			HAL_RTC_GetTime(&hrtc,&Time,RTC_FORMAT_BIN);//��ȡ��ǰʱ��
			for(i = 0 ;i < 8;i++)
			{
				if(G_Strategy[i].Strategy_State == Strategy_Open)//�鿴��ǰ�����Ƿ���
				{
					Min_Time = G_Strategy[i].Time.Hours * 60 + G_Strategy[i].Time.Minutes;//Сʱ����ɷ��Ӽ��Ϸ���
					NEXT_Min_Time = G_Strategy[i+1].Time.Hours * 60 + G_Strategy[i+1].Time.Minutes;//��һ����Сʱ����ɷ��Ӽ��Ϸ���
					if(((Time.Hours*60+Time.Minutes) >= Min_Time)&&((Time.Hours*60+Time.Minutes) < NEXT_Min_Time))//���Ե�ʱ����ڵ��ڵ�ǰʱ��
					{	//��ǰʱ��Ҫ���ڵ���            ����ʱ��       ��ǰʱ��             С�ڵ���  ��һ����ʱ��
						//printf("G_New_Mode = %d\r\n",G_New_Mode);
						G_New_Mode = i+1;//�жϲ��Կ���������ʱ���Ѿ��ж���ͬʱ����  �趨ִ�в���
						if(Old_Strategy != G_New_Mode)
						{
							if(Now_page == Humi_Page)
							{
								LCD12864_Put_Str_At_Present_Mode(G_New_Mode);	//lcd��ʾ���Ժ�
							}
							Old_Strategy = G_New_Mode;
						}
					}
				}
			}
			/*����Now_Strategy_num��ִ�в���*/
			if(G_Strategy[G_New_Mode-1].Strategy_Type == Strategy_Type_Route)	//��ǰ������ �г�����
			{
				Exe_Strategy_Type_Route();	//ִ���г�����
			}else if(G_Strategy[G_New_Mode-1].Strategy_Type == Strategy_Type_Temp){//��ǰ�������¶Ⱥ�������
				Exe_Strategy_Type_Temp(&Delay_Time,&Mark,&Time,&Strategy_mode,&NEW_Route);	//ִ�к�������
			}
			osDelay(200);
		}else if(G_New_Mode == MANUAL_MODE){
			/*
			 * �ֶ����(����)
			 * */
			Old_Strategy = MANUAL_MODE;
			osDelay(1000);
		}
	}
}

void Exe_Strategy_Type_Route(void)
{
	uint8_t relay1,relay2;
	relay1 = RELAY1_STATE;
	relay2 = RELAY2_STATE;
	if((G_Now_Route > G_Strategy[G_New_Mode-1].Route)&&((relay1 == GPIO_PIN_SET)&&(relay2 == GPIO_PIN_SET)))		//��ǰ�г̴����趨�г�
	{
		Motor_Reverses();
	}else if((G_Now_Route < G_Strategy[G_New_Mode-1].Route)&&((relay1 == GPIO_PIN_SET)&&(relay2 == GPIO_PIN_SET)))	//��ǰ�г�С���趨�г�
	{
		Motor_Forward();
	}else if((G_Now_Route == G_Strategy[G_New_Mode-1].Route)&&((relay1 == GPIO_PIN_RESET)||(relay2 == GPIO_PIN_RESET)))	//�г����
	{
		Motor_Stop();
	}
}
void Exe_Strategy_Type_Temp(uint32_t* p_Delay_Time,uint8_t* p_Mark,RTC_TimeTypeDef* p_Time,uint8_t* p_Strategy_mode,uint16_t* p_NEW_Route)
{
	uint8_t relay1,relay2;
	if((*p_Mark) == 0)
	{
		*p_Delay_Time = (p_Time->Hours*60+p_Time->Minutes*60+p_Time->Seconds);			//��¼��ǰʱ��
		(*p_Mark) = 1;
	}
	if((p_Time->Hours*60+p_Time->Minutes*60+p_Time->Seconds) >= ((*p_Delay_Time)+G_Delay_Inspection))//��ʱ����ִ�в���
	{
		relay1 = RELAY1_STATE;
		relay2 = RELAY2_STATE;
		if((relay1 == 1)&&(relay2 == 1))//���û��ת����ʱ��
		{
			//��ǰ�г�С������г̣�����0
			if(((temp/10) > G_Strategy[G_New_Mode-1].Temp_h)&&(G_Now_Route<G_Max_Route))//��⵱ǰ�¶ȴ�������¶�
			{
				(*p_NEW_Route) = G_Now_Route;
				*p_Strategy_mode = 1;	//1Ϊ��תģʽ10����  Ҫ���ɺ궨��
				Motor_Forward();	//��ת�����10����
			}else if(((temp/10) < G_Strategy[G_New_Mode-1].Temp_l)&&(G_Now_Route>0)){
				*p_NEW_Route = G_Now_Route;
				*p_Strategy_mode = 2;	//2Ϊ��תģʽ10����
				Motor_Reverses();	//��ת�ط��10����
			}
		}
		*p_Delay_Time = (p_Time->Hours*60+p_Time->Minutes*60+p_Time->Seconds);//��Ϊ��ʱ�����ϼ�
	}
	switch(*p_Strategy_mode){
	case 1://1��ʾ��ת�����10Ȧ
		*p_Delay_Time = (p_Time->Hours*60+p_Time->Minutes*60+p_Time->Seconds);	//��Ϊת��ʱ�򲻼���ʱ�����Խ�ʱ�����ϼ�
		if(G_Now_Route >= ((*p_NEW_Route)+ROUTE_NUM)){
			(*p_Mark) = 0;
			*p_Strategy_mode = 0;
			Motor_Stop();		//ͣת
		}
		break;
	case 2:
		*p_Delay_Time = (p_Time->Hours*60+p_Time->Minutes*60+p_Time->Seconds);	//��Ϊת��ʱ�򲻼���ʱ�����Խ�ʱ�����ϼ�
		if(G_Now_Route <= ((*p_NEW_Route)-ROUTE_NUM)){
			(*p_Mark) = 0;
			*p_Strategy_mode = 0;
			Motor_Stop();		//ͣת
		}
		break;
	}
}


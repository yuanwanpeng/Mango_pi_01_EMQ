/*
 * Strategy.h
 *
 *  Created on: 2018��11��28��
 *      Author: YuanWP
 */

#ifndef INC_STRATEGY_H_
#define INC_STRATEGY_H_
#include "LCD12864_Display_Menu_Delay_Inspection.h"
extern uint16_t G_Delay_Inspection;
extern Strategy_struct S_Strategy[8];	//8������

void Init_Strategy(uint8_t num);		//��ʼ������
void Start_Strategy_Task(void const * argument);//ִ�в�������
void Exe_Strategy_Type_Route(void);
void Exe_Strategy_Type_Temp(uint32_t* p_Delay_Time,uint8_t* p_Mark,RTC_TimeTypeDef* p_Time,uint8_t* p_Strategy_mode,uint16_t* p_NEW_Route);

#endif /* INC_STRATEGY_H_ */

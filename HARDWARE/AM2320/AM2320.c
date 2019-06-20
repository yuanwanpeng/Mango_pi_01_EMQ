/*
 * AM2320.c
 *
 *  Created on: 2019��3��17��
 *      Author: YuanWP
 */
#include "AM2320.h"
 
#define IIC_Add 0xB8    //������ַ
#define IIC_RX_Length 15
// ��������
uint8_t Sensor_Data[5]={0x00,0x00,0x00,0x00,0x00};
uint8_t IIC_TX_Buffer[]={0x03,0x00,0x04}; //����ʪ�������CRCУ�飩
unsigned char IIC_RX_Buffer[IIC_RX_Length] = {0x00};//���ص���ʪ��

uint8_t WR_Flag;

uint8_t Test_Ack(void)
{
	uint8_t ACK_Flag=0;
	SDA_IN(AM2320);
	IIC_AM2320_SCL_L;
	IIC_AM2320_SDA_H;
	delay_us(10);
	IIC_AM2320_SCL_H;
	delay_us(10);
	if(IIC_AM2320_SDA_STATE==0)
		ACK_Flag = 1;
	else 
		ACK_Flag = 0;
	IIC_AM2320_SCL_L;
	return ACK_Flag;
}

uint8_t WriteNByte(uint8_t sla,uint8_t *s,uint8_t n)
{
	uint8_t i;

	IIC_Start(AM2320); //����I2C
	IIC_Send_Byte(AM2320, sla);//����������ַ
	if(!Test_Ack())
	{	
		WR_Flag = 1;//����������**************
		printf("w wr_flag1\r\n");
		return(0);
	}
	for(i=0;i<n;i++)//д��8�ֽ�����
	{
		IIC_Send_Byte(AM2320,*(s+i));
		if(!Test_Ack())
		{
			WR_Flag = 1;//����Ҳ������
			printf("w wr_flag2\r\n");
			return(0);
		}
	}
	IIC_Stop(AM2320);
	return(1);
}

uint8_t ReadNByte(uint8_t Sal, uint8_t *p,uint8_t n)
{
  uint8_t i;
  IIC_Start(AM2320); 			//����I2C
  IIC_Send_Byte(AM2320,(Sal) | 0x01); 	//����������ַ
  if(!Test_Ack())
  {
  	WR_Flag = 1;
		printf("r wr_flag3\r\n");
		return(0);
  }
  osDelay(1);
  for(i=0;i<n-1;i++)  //��ȡ�ֽ�����
  {
     *(p+i)=IIC_Read_Byte(AM2320,1); //��ȡ������Ҫack
  }
  *(p+n-1)=IIC_Read_Byte(AM2320,0);  //����Ҫack
	IIC_Stop(AM2320);
  return(1);	 
}
void Waken(void)
{
	IIC_Start(AM2320);						//��ʼ�ź�
	IIC_Send_Byte(AM2320,0xB8);	  //����������ַ							
	Test_Ack();										//У��λ
	osDelay(2);										//��ʱ���3ms���Ѳ���
	IIC_Stop(AM2320);							//����һ��ֹͣ����
}

void Clear_Data (void)
{
	int i;
	for(i=0;i<IIC_RX_Length;i++)
	{
		IIC_RX_Buffer[i] = 0x00;
	}//������������
}
///����CRCУ����
unsigned int CRC16(unsigned char *ptr, unsigned char len)
{
   unsigned int crc=0xffff;
   unsigned char i;
   while(len--)
   {
       crc ^=*ptr++;
       for(i=0;i<8;i++)
	   {
	       if(crc & 0x1)
		   {
		      crc>>=1;
			  crc^=0xa001;
		   }
		   else
		   {
		      crc>>=1;
		   }
	   }
   }
   return crc;
}
///���CRCУ�����Ƿ���ȷ
unsigned char CheckCRC(unsigned char *ptr,unsigned char len)
{
  unsigned int crc;
	crc=(unsigned int)CRC16(ptr,len-2);
	if(ptr[len-1]==(crc>>8) && ptr[len-2]==(crc & 0x00ff))
	{
	    return 0xff;
	}
	else
	{
	   return 0x0;
	}
}
float AM2320_Get_TempResistor(float* p_temp,float* rh)
{
//	HUMI_struct humi_struct;
	IIC_AM2320_SDA_H;
	IIC_AM2320_SCL_H;
	Clear_Data(); // ����յ�����
	WR_Flag = 0;
	Waken();	  // ���Ѵ�����

	//���Ͷ�ָ��
	WriteNByte(IIC_Add,IIC_TX_Buffer,3);
	//���Ͷ�ȡ��д������������ٵȴ�2MS����̽ͷ����������ʱ��׼����
	osDelay(3); 
	
	//����������
	ReadNByte(IIC_Add,IIC_RX_Buffer,8);
	
	IIC_AM2320_SDA_H;
	IIC_AM2320_SCL_H;
	if(CheckCRC(IIC_RX_Buffer,8))
	{
		*p_temp = (IIC_RX_Buffer[4]*256+IIC_RX_Buffer[5]);
		*rh = (IIC_RX_Buffer[2]*256+IIC_RX_Buffer[3])/10;
	}else{
		*p_temp = 0;
		*rh = 0;
	}

	return 1;
}		






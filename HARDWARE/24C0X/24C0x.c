/*
 * 24C0x.c
 *
 *  Created on: 2018��10��16��
 *      Author: YuanWP
 */
#include "24c0x.h"
#include "IIC.h"

uint16_t G_Delay_Inspection;

//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	IIC_Init(AT24);
}
//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ
//����ֵ  :����������
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{
	uint8_t temp=0;
    IIC_Start(AT24);
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(AT24,0XA0);	   //����д����
		IIC_Wait_Ack(AT24);
		IIC_Send_Byte(AT24,ReadAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack(AT24);
	}else IIC_Send_Byte(AT24,0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д����

	IIC_Wait_Ack(AT24);
    IIC_Send_Byte(AT24,ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(AT24);
	IIC_Start(AT24);
	IIC_Send_Byte(AT24,0XA1);           //�������ģʽ
	IIC_Wait_Ack(AT24);
    temp=IIC_Read_Byte(AT24,0);
    IIC_Stop(AT24);//����һ��ֹͣ����
	return temp;
}

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{
    IIC_Start(AT24);
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(AT24,0XA0);	    //����д����
		IIC_Wait_Ack(AT24);
		IIC_Send_Byte(AT24,WriteAddr>>8);//���͸ߵ�ַ
 	}else
	{
		IIC_Send_Byte(AT24,0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д����
	}
	IIC_Wait_Ack(AT24);
    IIC_Send_Byte(AT24,WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(AT24);
	IIC_Send_Byte(AT24,DataToWrite);     //�����ֽ�
	IIC_Wait_Ack(AT24);
    IIC_Stop(AT24);//����һ��ֹͣ����
    osDelay(10);
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len)
{
	uint8_t t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
uint32_t AT24CXX_ReadLenByte(uint16_t ReadAddr,uint8_t Len)
{
	uint8_t t;
	uint32_t temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1);
	}
	return temp;
}
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
uint8_t AT24CXX_Check(uint8_t hardware)
{
	uint8_t temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX
	if(temp==0X55)return 0;
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);
		if(temp==0X55)return 0;
	}
	return 1;
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);
		NumToRead--;
	}
}
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
	
}

void Check_Delay_Inspection(void)
{
	uint8_t Delay_Inspection_H = 0,Delay_Inspection_L = 0;
	uint16_t Delay_Inspection;
	AT24CXX_Read(DELAY_INSPECTION,&Delay_Inspection_H,sizeof(Delay_Inspection_H));
	AT24CXX_Read(DELAY_INSPECTION+1,&Delay_Inspection_L,sizeof(Delay_Inspection_L));
	Delay_Inspection = (Delay_Inspection_H<<8)|Delay_Inspection_L;
	if(Delay_Inspection>600)
	{
		Init_Delay_Inspection(0);
	}else{
		G_Delay_Inspection = Delay_Inspection;
	}
}
void Init_Delay_Inspection(uint16_t Delay_Inspection)
{
	uint8_t Delay_Inspection_H,Delay_Inspection_L;
	Delay_Inspection_H = ((Delay_Inspection>>8)&0x00FF);
	Delay_Inspection_L = (Delay_Inspection&0x00FF);
	AT24CXX_Write(DELAY_INSPECTION,&Delay_Inspection_H,sizeof(Delay_Inspection_H));
	AT24CXX_Write(DELAY_INSPECTION+1,&Delay_Inspection_L,sizeof(Delay_Inspection_L));

	G_Delay_Inspection = Delay_Inspection;
}

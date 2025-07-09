/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <string.h>
#include "math.h"
#include "stm32f4xx_it.h"
#define HEADER1 0xAA
#define HEADER2 0xBB
uint16_t distance;
uint8_t Uart3_Receive_buf[1];
float best_angle=0;
uint16_t s=0;
OdomData Odom;
static const uint8_t CrcTable[256] =
{
 0x00, 0x4d, 0x9a, 0xd7, 0x79, 0x34, 0xe3,
 0xae, 0xf2, 0xbf, 0x68, 0x25, 0x8b, 0xc6, 0x11, 0x5c, 0xa9, 0xe4, 0x33,
 0x7e, 0xd0, 0x9d, 0x4a, 0x07, 0x5b, 0x16, 0xc1, 0x8c, 0x22, 0x6f, 0xb8,
 0xf5, 0x1f, 0x52, 0x85, 0xc8, 0x66, 0x2b, 0xfc, 0xb1, 0xed, 0xa0, 0x77,
 0x3a, 0x94, 0xd9, 0x0e, 0x43, 0xb6, 0xfb, 0x2c, 0x61, 0xcf, 0x82, 0x55,
 0x18, 0x44, 0x09, 0xde, 0x93, 0x3d, 0x70, 0xa7, 0xea, 0x3e, 0x73, 0xa4,
 0xe9, 0x47, 0x0a, 0xdd, 0x90, 0xcc, 0x81, 0x56, 0x1b, 0xb5, 0xf8, 0x2f,
 0x62, 0x97, 0xda, 0x0d, 0x40, 0xee, 0xa3, 0x74, 0x39, 0x65, 0x28, 0xff,
 0xb2, 0x1c, 0x51, 0x86, 0xcb, 0x21, 0x6c, 0xbb, 0xf6, 0x58, 0x15, 0xc2,
 0x8f, 0xd3, 0x9e, 0x49, 0x04, 0xaa, 0xe7, 0x30, 0x7d, 0x88, 0xc5, 0x12,
 0x5f, 0xf1, 0xbc, 0x6b, 0x26, 0x7a, 0x37, 0xe0, 0xad, 0x03, 0x4e, 0x99,
 0xd4, 0x7c, 0x31, 0xe6, 0xab, 0x05, 0x48, 0x9f, 0xd2, 0x8e, 0xc3, 0x14,
 0x59, 0xf7, 0xba, 0x6d, 0x20, 0xd5, 0x98, 0x4f, 0x02, 0xac, 0xe1, 0x36,
 0x7b, 0x27, 0x6a, 0xbd, 0xf0, 0x5e, 0x13, 0xc4, 0x89, 0x63, 0x2e, 0xf9,
 0xb4, 0x1a, 0x57, 0x80, 0xcd, 0x91, 0xdc, 0x0b, 0x46, 0xe8, 0xa5, 0x72,
 0x3f, 0xca, 0x87, 0x50, 0x1d, 0xb3, 0xfe, 0x29, 0x64, 0x38, 0x75, 0xa2,
 0xef, 0x41, 0x0c, 0xdb, 0x96, 0x42, 0x0f, 0xd8, 0x95, 0x3b, 0x76, 0xa1,
 0xec, 0xb0, 0xfd, 0x2a, 0x67, 0xc9, 0x84, 0x53, 0x1e, 0xeb, 0xa6, 0x71,
 0x3c, 0x92, 0xdf, 0x08, 0x45, 0x19, 0x54, 0x83, 0xce, 0x60, 0x2d, 0xfa,
 0xb7, 0x5d, 0x10, 0xc7, 0x8a, 0x24, 0x69, 0xbe, 0xf3, 0xaf, 0xe2, 0x35,
 0x78, 0xd6, 0x9b, 0x4c, 0x01, 0xf4, 0xb9, 0x6e, 0x23, 0x8d, 0xc0, 0x17,
 0x5a, 0x06, 0x4b, 0x9c, 0xd1, 0x7f, 0x32, 0xe5, 0xa8
};//用于crc校验的数组
char USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
uint16_t point1 ;
LiDARFrameTypeDef Pack_Data;//雷达接收的数据储存在这个变量之中
LidarPointStructDef Dataprocess[800];//雷达转动一圈数据储存
LidarPointStructDef PointDataProcess[800];
u16 receive_cnt;
u8 data_process_flag,data_flag = 0;
u16 data_cnt = 0;
extern uint8_t RX_buffer[1];
uint16_t safe_distance[200];
uint16_t forward_distance[150][2];
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */
     

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void send_odometry(OdomData *odom) {
     uint8_t buffer[15];  // 2(帧头) + 12(3个float) + 1(校验) = 15字节
    uint8_t checksum = 0;
    int pos = 0;
    
    // 1. 添加帧头
    buffer[pos++] = HEADER1;
    buffer[pos++] = HEADER2;
    
    // 2. 添加数据 (显式小端序处理)
    float temp;
    
    // 处理 x
    temp = odom->x;
    buffer[pos++] = *((uint8_t*)&temp + 0);
    buffer[pos++] = *((uint8_t*)&temp + 1);
    buffer[pos++] = *((uint8_t*)&temp + 2);
    buffer[pos++] = *((uint8_t*)&temp + 3);
    
    // 处理 y
    temp = odom->y;
    buffer[pos++] = *((uint8_t*)&temp + 0);
    buffer[pos++] = *((uint8_t*)&temp + 1);
    buffer[pos++] = *((uint8_t*)&temp + 2);
    buffer[pos++] = *((uint8_t*)&temp + 3);
    
    // 处理 yaw
    temp = odom->yaw;
    buffer[pos++] = *((uint8_t*)&temp + 0);
    buffer[pos++] = *((uint8_t*)&temp + 1);
    buffer[pos++] = *((uint8_t*)&temp + 2);
    buffer[pos++] = *((uint8_t*)&temp + 3);
    
    // 3. 计算校验和 (帧头+数据)
    for(int i = 0; i < 14; i++) {
       checksum += buffer[i];  
    }
    buffer[pos++] = checksum;
    
    // 4. 发送数据
    HAL_UART_Transmit(&huart3, buffer, sizeof(buffer), HAL_MAX_DELAY);
}
void data_process(void)//数据处理函数，完成一帧之后可进行数据处理
{
	int i,m,n;
	u32 distance_sum[12]={0};//2个点的距离和的数组
	float start_angle = Pack_Data.start_angle/100.0;//计算12个点的开始角度
	float end_angle = Pack_Data.end_angle/100.0;//计算12个点的结束角度
	float area_angle[12]={0};
	static uint16_t a=0;
	if(start_angle>end_angle)//结束角度和开始角度被0度分割的情况
	end_angle +=360;
	static  uint16_t cnt=0;
	static uint16_t mm=0;  
	static uint16_t aa=0;  
	float angletemp=0;
  data_process_flag=0; //标志位清零
	for(m=0;m<12;m++)
	{
		area_angle[m]=start_angle+(end_angle-start_angle)/12*m;
		if(area_angle[m]>360)  area_angle[m] -=360;
	}

	for(n=0;n<12;n++)//将数据传输到Dataprocess中，防止被覆写
	{
		Dataprocess[data_cnt+n].angle = area_angle[n];
		
		if(Pack_Data.point[n].distance==0)
		{
			Dataprocess[data_cnt+n].distance= Dataprocess[data_cnt+n-1].distance;
		}
		else if(Pack_Data.point[n].distance>=8500)
		{
			Dataprocess[data_cnt+n].distance= Dataprocess[data_cnt+n-1].distance;
		}
		else
		{
	  Dataprocess[data_cnt+n].distance = Pack_Data.point[n].distance;  //一帧数据为12个点
		}
		if(Dataprocess[data_cnt+n].angle>=180)
		{
			 angletemp=Dataprocess[data_cnt+n].angle-180;
		}
		if(Dataprocess[data_cnt+n].angle<180)
		{
			angletemp=Dataprocess[data_cnt+n].angle+180;
		}
		if(angletemp>=345&&mm==0)
		{
			forward_distance[0][0]=Dataprocess[data_cnt+n].distance;
			forward_distance[0][1]=angletemp;
			mm=1;
			aa=1;
		}
		else if(angletemp>345.8&&mm==1)
		{
			forward_distance[aa][0]=Dataprocess[data_cnt+n].distance;
			forward_distance[aa][1]=angletemp;
			mm=1;
			aa+=1;
		}
		else if(angletemp<=15)
		{
			forward_distance[aa][0]=Dataprocess[data_cnt+n].distance;
			forward_distance[aa][1]=angletemp;
			mm=2;
			aa+=1;
		}
		else if(angletemp>15&&mm==2)
		{
			forward_distance[aa][0]=Dataprocess[data_cnt+n].distance;
			forward_distance[aa][1]=angletemp;
			mm=0;
			aa=0;
		}
		
		if( Dataprocess[data_cnt+n].distance>=750)
		{
			safe_distance[a]=Dataprocess[data_cnt+n].angle;
			a+=1;
			if(angle_diff(safe_distance[a-1],safe_distance[0])>32)
			{
				best_angle=(safe_distance[a-1]+safe_distance[0])/2;
				if(best_angle>=180)
				{
					best_angle=best_angle-180;
				}
				if(best_angle<180)
				{
					best_angle=best_angle+180;
				}
				a=0;
				s+=1;
				if(s>=100)
				{
					s=0;
				}
				
			}
			cnt=0;
		}
		else
		{
			cnt+=1;
			
			if(cnt>=3)
			{
				a=0;
				cnt=0;
			}
		}
	}
	data_cnt +=12;
	if(data_cnt>=720) //雷达转一圈大概有720个点（大概数值，每圈的点数都不固定，一帧大约为6度，一圈大约是60帧数据=12*60=720）
	{
		data_cnt = 0;
		data_flag = 1;
	}
}


float float_abs(float input)
{
	if(input<0)
		return -input;
	else
		return input;

}
float angle_diff(float angle1,float angle2)
{
	float diff=0;
	diff=angle1-angle2;
	if(angle1>200&&angle2<100)
	{
		diff=angle2+360-angle1;
	}
	return diff;

}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) //���ջص�����
//{
//	   static u8 state = 0;//״̬λ	
//	static u8 crc = 0;//У���
//	static u8 cnt = 0;//����һ֡12����ļ���
//	u8 temp_data;
//	if(huart -> Instance == USART3)
// {
//	{
//			temp_data=Uart3_Receive_buf[0];	;
//		if (state > 5)
//		{
//			if(state < 42)
//			{
//				if(state%3 == 0)//一帧数据中的序号为6,9.....39的数据，距离值低8位
//				{
//					Pack_Data.point[cnt].distance = (u16)temp_data;
//					state++;
//					crc = CrcTable[(crc^temp_data) & 0xff];
//				}
//				else if(state%3 == 1)//一帧数据中的序号为7,10.....40的数据，距离值高8位
//				{
//					Pack_Data.point[cnt].distance = ((u16)temp_data<<8)+Pack_Data.point[cnt].distance;
//					state++;
//					crc = CrcTable[(crc^temp_data) & 0xff];
//				}
//				else//一帧数据中的序号为8,11.....41的数据，置信度
//				{
//					Pack_Data.point[cnt].confidence = temp_data;
//					cnt++;	
//					state++;
//					crc = CrcTable[(crc^temp_data) & 0xff];
//				}
//			}
//			else 
//			{
//				switch(state)
//				{
//					case 42:
//						Pack_Data.end_angle = (u16)temp_data;//结束角度低8位
//						state++;
//						crc = CrcTable[(crc^temp_data) & 0xff];
//						break;
//					case 43:
//						Pack_Data.end_angle = ((u16)temp_data<<8)+Pack_Data.end_angle;//结束角度高8位
//						state++;
//						crc = CrcTable[(crc^temp_data) & 0xff];
//						break;
//					case 44:
//						Pack_Data.timestamp = (u16)temp_data;//时间戳低8位
//						state++;
//						crc = CrcTable[(crc^temp_data) & 0xff];
//						break;
//					case 45:
//						Pack_Data.timestamp = ((u16)temp_data<<8)+Pack_Data.timestamp;//时间戳高8位
//						state++;
//						crc = CrcTable[(crc^temp_data) & 0xff];
//						break;
//					case 46:
//						Pack_Data.crc8 = temp_data;//雷达传来的校验和
//						if(Pack_Data.crc8 == crc)//校验正确
//						{
//							data_process();//接收到一帧且校验正确可以进行数据处理
//							receive_cnt++;//输出接收到正确数据的次数
//							data_process_flag=1; //标志位
//						}
//						else
//							memset(&Pack_Data,0,sizeof(Pack_Data));//清零
//						crc = 0;
//						state = 0;
//						cnt = 0;//复位
//					default: break;
//				}
//			}
//		}
//		else 
//		{
//			switch(state)
//			{
//				case 0:
//					if(temp_data == HEADER)//头固定
//					{
//						Pack_Data.header = temp_data;
//						state++;
//						crc = CrcTable[(crc^temp_data) & 0xff];//开始进行校验
//					} else state = 0,crc = 0;
//					break;
//				case 1:
//					if(temp_data == LENGTH)//测量的点数，目前固定
//					{
//						Pack_Data.ver_len = temp_data;
//						state++;
//						crc = CrcTable[(crc^temp_data) & 0xff];
//					} else state = 0,crc = 0;
//					break;
//				case 2:
//					Pack_Data.speed = (u16)temp_data;//雷达的转速低8位，单位度每秒
//					state++;
//					crc = CrcTable[(crc^temp_data) & 0xff];
//					break;
//				case 3:
//					Pack_Data.speed = ((u16)temp_data<<8)+Pack_Data.speed;//雷达的转速高8位
//					state++;
//					crc = CrcTable[(crc^temp_data) & 0xff];
//					break;
//				case 4:
//					Pack_Data.start_angle = (u16)temp_data;//开始角度低8位，放大了100倍
//					state++;
//					crc = CrcTable[(crc^temp_data) & 0xff];
//					break;
//				case 5:
//					Pack_Data.start_angle = ((u16)temp_data<<8)+Pack_Data.start_angle;
//					state++;
//					crc = CrcTable[(crc^temp_data) & 0xff];
//					break;
//				default: break;

//			}
//		}
//	}		
//		}
//		HAL_UART_Receive_IT(&huart3,Uart3_Receive_buf,sizeof(Uart3_Receive_buf));
//		
//	}
	
     
//		 uint8_t rx_byte2;
//	HAL_UART_Receive_IT(&huart3,&rx_byte2,1);
	
uint16_t  forward_detect()
{
	uint16_t a=0;
	uint16_t cnt=0;
	uint16_t cnt2=0;
	uint16_t flag=0;
	while(forward_distance[a][0]!=0)
	{
		a+=1;
		if(forward_distance[a][0]<290)
		{
			cnt+=1;
			cnt2+=1;
		}
//		else if((forward_distance[a][0]*sinf(forward_distance[a][1]*180/M_PI)<180)&&(angle_diff(forward_distance[a][1],0)>12.5))
//		{
//			cnt+=1;
//			cnt2+=1;
//		}
		else
		{
			cnt=0;
		}
		if(cnt>7)
		{
			flag=1;
			break;
		}
		if(cnt2>23)
		{
			flag=1;
			break;
		}
	}
	return flag;
}
 

/* USER CODE END 1 */

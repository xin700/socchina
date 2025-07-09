/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define USART_REC_LEN  			200  	//�����������ֽ��� 200


/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
#define ANGLE_PER_FRAME 12
#define HEADER 0x54
#define POINT_PER_PACK 12
#define LENGTH  0x2C 	//低五位是一帧数据接收到的点数，目前固定是12，高三位预留

typedef struct __attribute__((packed)) Point_Data
{
	u16 distance;//距离
	u8 confidence;//置信度
	float angle; //角度
}LidarPointStructDef;

typedef struct __attribute__((packed)) Pack_Data
{
	uint8_t header;
	uint8_t ver_len;
	uint16_t speed;
	uint16_t start_angle;
	LidarPointStructDef point[POINT_PER_PACK];
	uint16_t end_angle;
	uint16_t timestamp;
	uint8_t crc8;
}LiDARFrameTypeDef;
void data_process(void);
float float_abs(float input);
float angle_diff(float angle1,float angle2);
uint16_t  forward_detect();
typedef struct {
    float x;
    float y;
    float yaw;
} OdomData;
void send_odometry(OdomData *odom) ;
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */


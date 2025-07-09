/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define imu_buflength 256
#define imu_frame 132
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
typedef struct PIDSPEED{
    float KP;
	float KI;
	float KD;
	float LimitIntergal;
	float LimitOutput;
	float Integral;
	float Output;
	float limitoutput_fanzhuan;

}PIDdata;
typedef struct location{
    float Vtx;
	float Vty;
	float VX;
	float VY;
	float W;
	float locationX;
	float locationY;
	float relativeY;
    float relativeX;	
	
}locationdata;
typedef enum ObstacleCase {
  CLEAR_PATH,          // ���з���û���ϰ���
  OBSTACLE_LEFT,       // ��������ϰ���
  OBSTACLE_CENTER,     // ����ǰ�����ϰ���
  OBSTACLE_RIGHT,      // ���Ҳ����ϰ���
  OBSTACLE_LEFT_CENTER,// ������ǰ��ͬʱ���ϰ���
  OBSTACLE_CENTER_RIGHT,// ��ǰ�����Ҳ�ͬʱ���ϰ���
  OBSTACLE_LEFT_RIGHT, // �����������ϰ����ǰ���ޣ�
  OBSTACLE_ALL_SIDES   // ���з������ϰ���
}Obstaclecase;
uint16_t get_encoder();
uint16_t get_encoder1();
uint16_t get_encoder2();
uint16_t get_encoder3();
uint32_t DWT_Delay_Init(void);
void delay_us(uint32_t us) ;
void PID_init();
void PID_reinit();
int16_t speedPID( uint16_t target,float *vbefore,float V,PIDdata *speed);
int16_t direct_PID(int16_t target,int16_t *anglebefore,int16_t *intergram,int16_t angle ,PIDdata *tar);

float  MYabs(float a,float b);
float V_caculate(uint16_t *temp1,uint16_t *temp11,uint16_t *counter_temp1,uint16_t *counter_tempbefore1,uint16_t a);
void turn_fixed_angle(float angle_begin ,float angle_target,uint16_t time);
void locateenter(float *v1,float *v2,float *v3,float *v4,float dalayt);
float length_control(float lengthnow,float target,PIDdata *lengthpid);
void length_goY(float len);
void length_goX(float len);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

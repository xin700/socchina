/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.h
  * @brief   This file contains the headers of the interrupt handlers.
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
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);
void TIM1_TRG_COM_TIM11_IRQHandler(void);
void TIM1_CC_IRQHandler(void);
void USART1_IRQHandler(void);
void USART3_IRQHandler(void);
void TIM8_BRK_TIM12_IRQHandler(void);
void TIM8_UP_TIM13_IRQHandler(void);
void TIM6_DAC_IRQHandler(void);
void TIM7_IRQHandler(void);
/* USER CODE BEGIN EFP */

 // ???IMU
 #define RX_BUF_SIZE 64
typedef struct {
    float acc[3];    
    float gyro[3];  
    float angle[3]; 
    uint8_t updated; 
} JY901_Data;
typedef struct {
    float alpha_hpf;    // ��ͨ�˲�ϵ�������ƽ�ֹƵ�ʣ�
    float alpha_lpf;    // ��ͨ�˲�ϵ�������ƽ�ֹƵ�ʣ�
    float hpf_prev_in;  // ��ͨ�˲�����һ�ε�����ֵ�����ڲ�ּ��㣩
    float hpf_prev_out; // ��ͨ�˲�����һ�ε����ֵ�����ڵݹ���㣩
    float lpf_prev_out; // ��ͨ�˲�����һ�ε����ֵ�����ڵݹ���㣩
} BiquadFilter;

void init_bpf(BiquadFilter* filter, float f_low, float f_high, float deltaT) ;
float apply_bpf(BiquadFilter* filter, float input) ;
#define M_PI 3.14159265358979323846
void delay_us1(uint32_t us);
void parse_jy901_data(uint8_t type, uint8_t* buf);

uint16_t  detect_Obstacle();

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */

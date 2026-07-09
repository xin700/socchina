/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include <stdbool.h>

typedef struct {
  CAN_RxHeaderTypeDef rx_header;
  uint8_t rx_data[8];
  volatile bool rx_pending;
  volatile uint32_t tx_frame_cnt;
  volatile uint32_t rx_frame_cnt;
  volatile uint32_t tx_fail_cnt;
  volatile uint32_t irq_rx0_cnt;
  volatile uint32_t poll_rx_cnt;
} CAN_AppContext_t;

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan;

/* USER CODE BEGIN Private defines */

extern CAN_AppContext_t g_can_app;

/* USER CODE END Private defines */

void MX_CAN_Init(void);

/* USER CODE BEGIN Prototypes */

void CAN_App_FilterInit(void);
void CAN_App_Start(void);
bool CAN_App_SendCmd(const volatile uint8_t *cmd, uint8_t len);
void CAN_App_PollRx(void);
uint32_t CAN_App_GetRxFifoLevel(void);
void CAN_App_SetRxIrqEnabled(bool enable);
void CAN_App_DrainRxFifo(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */


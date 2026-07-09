/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "pickbot.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId chaoshengduquHandle;
osThreadId ZTDcontrolHandle;
osThreadId drv8874Handle;
osThreadId JYtaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void chaosheng(void const * argument);
void ZTDCONTROL(void const * argument);
void drv8874control(void const * argument);
void JY901read(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   (void)xTask;
   (void)pcTaskName;
}
/* USER CODE END 4 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of chaoshengduqu */
  osThreadDef(chaoshengduqu, chaosheng, osPriorityNormal, 0, 128);
  chaoshengduquHandle = osThreadCreate(osThread(chaoshengduqu), NULL);

  /* definition and creation of ZTDcontrol */
  osThreadDef(ZTDcontrol, ZTDCONTROL, osPriorityNormal, 0, 256);
  ZTDcontrolHandle = osThreadCreate(osThread(ZTDcontrol), NULL);

  /* definition and creation of drv8874 */
  osThreadDef(drv8874, drv8874control, osPriorityNormal, 0, 256);
  drv8874Handle = osThreadCreate(osThread(drv8874), NULL);

  /* definition and creation of JYtask */
  osThreadDef(JYtask, JY901read, osPriorityNormal, 0, 128);
  JYtaskHandle = osThreadCreate(osThread(JYtask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  默认任务：预留 RDK X5 上位机通信/系统监控
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  for (;;)
  {
    osDelay(20);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_chaosheng */
/**
  * @brief  US-100 超声测距任务
  */
/* USER CODE END Header_chaosheng */
__weak void chaosheng(void const * argument)
{
  /* USER CODE BEGIN chaosheng */
  for (;;)
  {
    Pickbot_Task_US100();
    osDelay(100);
  }
  /* USER CODE END chaosheng */
}

/* USER CODE BEGIN Header_ZTDCONTROL */
/**
  * @brief  张大头 ZDT 步进电机 CAN 控制任务
  */
/* USER CODE END Header_ZTDCONTROL */
__weak void ZTDCONTROL(void const * argument)
{
  /* USER CODE BEGIN ZTDCONTROL */
  for (;;)
  {
    Pickbot_Task_ZDT();
    osDelay(5);
  }
  /* USER CODE END ZTDCONTROL */
}

/* USER CODE BEGIN Header_drv8874control */
/**
  * @brief  DRV8874 捡球电机 + 编码器 + 电流采样任务
  */
/* USER CODE END Header_drv8874control */
__weak void drv8874control(void const * argument)
{
  /* USER CODE BEGIN drv8874control */
  for (;;)
  {
    Pickbot_Task_Drv8874();
    osDelay(20);
  }
  /* USER CODE END drv8874control */
}

/* USER CODE BEGIN Header_JY901read */
/**
* @brief Function implementing the JYtask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_JY901read */
__weak void JY901read(void const * argument)
{
  /* USER CODE BEGIN JY901read */
  /* Infinite loop */
  for(;;)
  {
	Pickbot_Task_JY901S();
    osDelay(20);
  }
  /* USER CODE END JY901read */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */


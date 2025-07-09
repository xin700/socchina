/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "freertos.h"
#include "semphr.h"
#include "task.h"  
 #include "cmsis_os2.h"
 #include "usart.h"
 #include <math.h>   
 #include "motor.h"
  #include "DWT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim11;
extern TIM_HandleTypeDef htim12;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern TIM_HandleTypeDef htim13;

/* USER CODE BEGIN EV */
extern TIM_HandleTypeDef htim2;
#define DELAY_US_TIM_CLK_HZ  SystemCoreClock  
uint16_t length1=0;
uint16_t length2=0;
uint16_t length3=0;
uint8_t rx_buf[RX_BUF_SIZE];
uint8_t rx_index = 0;
uint8_t data_length = 0;
uint8_t data_type = 0;
uint8_t checksum=0;
uint16_t aa=0;
uint16_t bb=0;
extern float best_angle;
float m;
enum {
    STATE_WAIT_HEADER,
    STATE_GET_TYPE,
    STATE_GET_DATA,
    STATE_CHECK_SUM
} rx_state = STATE_WAIT_HEADER;
JY901_Data imu_data;
JY901_Data* myimu=&imu_data;
float imu_angle_base[3];
float imu_acc_base[3];
float imu_angle[3];
extern osMessageQueueId_t pid_queueHandleHandle;
extern osMessageQueueId_t imudataHandle;
extern Obstaclecase judge_ob;
extern uint16_t flag_angle;
extern uint16_t runstate;
float V1=0;
float V2=0;
float V3=0;
float V4=0;
uint16_t imu_temp[200];
uint16_t Vtemp1=0;
uint16_t Vtemp2=0;
uint16_t Vtemp3=0;
uint16_t Vtemp4=0;
uint16_t Vtemp11=0;
int16_t Vtemp21=0;
uint16_t Vtemp31=0;
uint16_t Vtemp41=0;
uint16_t counter_before1=0;
uint16_t counter_before2=0;
uint16_t counter_before3=0;
uint16_t counter_before4=0;
uint16_t counter_temp1=0;
uint16_t counter_temp2=0;
uint16_t counter_temp3=0;
uint16_t counter_temp4=0;
uint16_t timenow1=0;
uint16_t timebefore1=0;
uint16_t send_time=0;
float deltat;
uint16_t changet;
extern uint16_t s;
extern OdomData Odom;
extern uint16_t fangxiang;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM1 trigger and commutation interrupts and TIM11 global interrupt.
  */
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_TRG_COM_TIM11_IRQn 0 */
	if (__HAL_TIM_GET_FLAG(&htim11, TIM_FLAG_UPDATE))
  {
	  
    __HAL_TIM_CLEAR_FLAG(&htim11, TIM_FLAG_UPDATE); // 清除标志
	  if(fangxiang==4&&aa==200)
	  {
	 static uint16_t i=0;
     i+=1;
	 if(i==2)
	{
	   Vtemp1=get_encoder();
	   Vtemp2=get_encoder1();
	   Vtemp3=get_encoder2();
	   Vtemp4=get_encoder3();
	   V1=V_caculate(&Vtemp2,&Vtemp21,&counter_temp2,&counter_before2,2);
	   V2=V_caculate(&Vtemp1,&Vtemp11,&counter_temp1,&counter_before1,1);
	   V3=V_caculate(&Vtemp3,&Vtemp31,&counter_temp3,&counter_before3,3);
	   V4=V_caculate(&Vtemp4,&Vtemp41,&counter_temp4,&counter_before4,4);
	   mode_change(&V1,&V2,&V3,&V4,runstate);
	   locateenter(&V1,&V2,&V3,&V4,0.02);
	   
	i=0;
	
	}
}
}
 
  /* USER CODE END TIM1_TRG_COM_TIM11_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  HAL_TIM_IRQHandler(&htim11);
  /* USER CODE BEGIN TIM1_TRG_COM_TIM11_IRQn 1 */

  /* USER CODE END TIM1_TRG_COM_TIM11_IRQn 1 */
}

/**
  * @brief This function handles TIM1 capture compare interrupt.
  */
void TIM1_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_CC_IRQn 0 */
	static uint16_t upedge=0;
	static uint16_t downedge=0;
	static uint8_t is_rising_captured1 = 0;
	static uint8_t is_rising_captured2 = 0;
	static uint8_t is_rising_captured3 = 0;
	static uint8_t i1=0;
	static uint8_t i2=0;
	static uint8_t i3=0;
	static uint16_t length1_temp[4];
	static uint16_t length2_temp[4];
	static uint16_t length3_temp[4];
	if (__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_CC1) && __HAL_TIM_GET_IT_SOURCE(&htim1, TIM_IT_CC1)) 
	 {
    if (!is_rising_captured1) {
		upedge=HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_1);
		 __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		is_rising_captured1=1;
	}
	else
	{
		downedge= HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_1);
		 __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
        is_rising_captured1 = 0;
		if(i1<=3)
		{
		length1_temp[i1]=(downedge-upedge)*340000/100000/2;
		i1+=1;
		if(i1==4)
		{
			for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 3 - j; k++) {
                if (length1_temp[k] > length1_temp[k+1]) {
                 
                    float temp = length1_temp[k];
                    length1_temp[k] = length1_temp[k+1];
                    length1_temp[k+1] = temp;
                }
            }
        }
			length1=(length1_temp[1]+length1_temp[2])/2;
		
			i1=0;
		}
		
		}
		HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_1);
	}
	__HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC1); 
}
	 if (__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_CC2) && __HAL_TIM_GET_IT_SOURCE(&htim1, TIM_IT_CC2)) 
	 {
    if (!is_rising_captured2) {
		upedge=HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_2);
		 __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
		is_rising_captured2=1;
	}
	else
	{
		downedge= HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_2);
		 __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
        is_rising_captured2 = 0;
		if(i2<=3)
		{
		length2_temp[i2]=(downedge-upedge)*340000/100000/2;
		i2+=1;
		if(i2==4)
		{
				for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 3 - j; k++) {
                if (length2_temp[k] > length2_temp[k+1]) {
                    // 交换元素
                    float temp = length2_temp[k];
                    length2_temp[k] = length2_temp[k+1];
                    length2_temp[k+1] = temp;
                }
            }
		}
			length2=(length2_temp[1]+length2_temp[2])/2;
			i2=0;
		}
		
		}
		
		//length2=(downedge-upedge)*340000/100000/2;
		HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_2);
	}
	 __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC2);
}
	if (__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_CC3) && __HAL_TIM_GET_IT_SOURCE(&htim1, TIM_IT_CC3))
	 {
    if (!is_rising_captured3) {
		upedge=HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_3);
		 __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
		is_rising_captured3=1;
	}
	else
	{
		downedge= HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_3);
		 __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
        is_rising_captured3 = 0;
		if(i3<=3)
		{
		length3_temp[i3]=(downedge-upedge)*340000/100000/2;
		i3+=1;
		if(i3==4)
		{
				for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 3 - j; k++) {
                if (length3_temp[k] > length3_temp[k+1]) {
                    // 交换元素
                    float temp = length3_temp[k];
                    length3_temp[k] = length3_temp[k+1];
                    length3_temp[k+1] = temp;
                }
            }
		}
			length3=(length3_temp[1]+length3_temp[2])/2;
			i3=0;
		}
		
		}
		//length3=(downedge-upedge)*340000/100000/2;
		
		HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_3);
	}

	__HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC3);
}
	 
	 
  /* USER CODE END TIM1_CC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_CC_IRQn 1 */

  /* USER CODE END TIM1_CC_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	static uint16_t a=0;
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE))//?????????
    {
		//if(fangxiang==4)
		{
		uint8_t byte = (uint8_t)(huart1.Instance->DR & 0xFF);
       imu_temp[a]=byte;
		a+=1;
		if(a>=200)
		{
			a=0;
		}
		switch(rx_state) {
        // 状态1：等待帧头0x55
        case STATE_WAIT_HEADER:
            if(byte == 0x55) {
                rx_index = 0;
                rx_state = STATE_GET_TYPE;
            }
            break;

        // 状态2：获取数据类型
        case STATE_GET_TYPE:
            data_type = byte;
            rx_state = STATE_GET_DATA;
            data_length = 8; // JY901数据部分固定8字节
            break;

        // 状态3：获取数据内容
        case STATE_GET_DATA:
            rx_buf[rx_index++] = byte;
            if(rx_index >= data_length) {
                rx_state = STATE_CHECK_SUM;
            }
            break;

        // 状态4：校验和验证
        case STATE_CHECK_SUM:
             checksum = 0x55 + data_type;
            for(int i=0; i<data_length; i++)
                checksum += rx_buf[i];
            
            if(checksum == byte) {
                parse_jy901_data(data_type, rx_buf);
                imu_data.updated = 1; // 设置更新标志
				
			osMessageQueuePut(imudataHandle,&imu_data.updated,0,0);
            }
            rx_state = STATE_WAIT_HEADER;
            break;
        }
	}
		 __HAL_UART_CLEAR_PEFLAG(&huart1);
    
}

// if(huart1.Instance->SR & UART_FLAG_RXNE)//���յ�����
//    {
//        __HAL_UART_CLEAR_PEFLAG(&huart1);
//    }
//    else if(USART1->SR & UART_FLAG_IDLE)
//    {
//        static uint16_t this_time_rx_len = 0;

//        __HAL_UART_CLEAR_PEFLAG(&huart1);

//        if ((hdma_usart1_rx.Instance->CR & DMA_SxCR_CT) == RESET)
//        {
//            /* Current memory buffer used is Memory 0 */

//            //disable DMA
//            //ʧЧDMA
//            __HAL_DMA_DISABLE(&hdma_usart1_rx);

//            //get receive data length, length = set_data_length - remain_length
//            
//            this_time_rx_len =imu_buflength - hdma_usart1_rx.Instance->NDTR;

//            //reset set_data_lenght
//           
//            hdma_usart1_rx.Instance->NDTR = imu_buflength;

//            //set memory buffer 1
//           
//            hdma_usart1_rx.Instance->CR |= DMA_SxCR_CT;
//            
//            //enable DMA
//           
//            __HAL_DMA_ENABLE(&hdma_usart1_rx);
//            
//            if(this_time_rx_len == imu_frame)
//            {
////                sbus_to_remote_ch(sbus_rx_buf[0], remote_ch);
////                //��¼���ݽ���ʱ��
////                detect_hook(DBUS_TOE);
//////                sbus_to_usart1(sbus_rx_buf[0]);
//            }
//        }
//        else
//        {
//            /* Current memory buffer used is Memory 1 */
//            //disable DMA
//            //ʧЧDMA
//            __HAL_DMA_DISABLE(&hdma_usart1_rx);

//        
//            this_time_rx_len = imu_buflength - hdma_usart1_rx.Instance->NDTR;

//            hdma_usart1_rx.Instance->NDTR = imu_buflength;


//            DMA1_Stream1->CR &= ~(DMA_SxCR_CT);
//            
//            //enable DMA
//        
//            __HAL_DMA_ENABLE(&hdma_usart1_rx);

//            if(this_time_rx_len == imu_frame)
//            {
//                
////                sbus_to_remote_ch(sbus_rx_buf[1], remote_ch);
////                
////                detect_hook(DBUS_TOE);
////                sbus_to_usart1(sbus_rx_buf[1]);
//            }
//        }
//    }

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE))
{
	static uint16_t state=0;
	uint8_t byte = (uint8_t)(huart3.Instance->DR & 0xFF);
    switch (state){
		case 0:
			if(byte==0xAA)
			{
				state=1;
		}
			break;
		case 1:
			if(byte==0xBB)
			{
				state=2;
			}
			break;
		case 2:
			fangxiang=byte;
		    state=3;
		    break;
		case 3:
			if(byte==0xCC)
			{
				state=0;
			}
			break;	
	}
	__HAL_UART_CLEAR_PEFLAG(&huart3);
	
	
	
}
  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles TIM8 break interrupt and TIM12 global interrupt.
  */
void TIM8_BRK_TIM12_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_BRK_TIM12_IRQn 0 */
	
    // if(HAL_UART_GetState(&huart3) == HAL_UART_STATE_READY)
	{
	send_odometry(&Odom);
	send_time+=1;
	
	}

  /* USER CODE END TIM8_BRK_TIM12_IRQn 0 */
  HAL_TIM_IRQHandler(&htim8);
  HAL_TIM_IRQHandler(&htim12);
  /* USER CODE BEGIN TIM8_BRK_TIM12_IRQn 1 */

  /* USER CODE END TIM8_BRK_TIM12_IRQn 1 */
}

/**
  * @brief This function handles TIM8 update interrupt and TIM13 global interrupt.
  */
void TIM8_UP_TIM13_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_UP_TIM13_IRQn 0 */

  /* USER CODE END TIM8_UP_TIM13_IRQn 0 */
  HAL_TIM_IRQHandler(&htim8);
  HAL_TIM_IRQHandler(&htim13);
  /* USER CODE BEGIN TIM8_UP_TIM13_IRQn 1 */

  /* USER CODE END TIM8_UP_TIM13_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */
	static uint16_t i=0;
    static uint16_t mm=0;
	static uint16_t a=0;
	if(i==0)
	{
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_SET);
	DWT_DelayUS(20);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_RESET);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
		
	}
	if(i==1)
	{
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_12,GPIO_PIN_SET);
	DWT_DelayUS(20);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_12,GPIO_PIN_RESET);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
		
	}
	if(i==2)
	{
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14,GPIO_PIN_SET);
	DWT_DelayUS(20);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14,GPIO_PIN_RESET);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);
		
	}
	i+=1;
	if(i==3)
	{
		i=0;
	}
	if(a==100)
	{
		a=0;
		if(mm==s)
		{
			best_angle=400;
		}
		else
		{
		mm=s;
		}
	}
	else
	{
		a+=1;
	}
	
	
   

	//HAL_TIM_Base_Start(&htim1);
  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
    static   uint16_t a=0;
    a+=1;
	static uint16_t b=0;
	static float angle=0;
	angle=imu_data.angle[2];
	static float angle_before=0;
	if(a==20)
	{
		float error=MYabs(angle,angle_before);
		if(angle<-90&&angle_before>90)
		{
			error=360-angle_before+angle;
		}
		if(angle>90&&angle_before<-90)
		{
			error=360-angle+angle_before;
		}
		if(error<=2)
		{
			b+=1;
		}
		else
		{
			b=0;
		}
		angle_before=angle;
		a=0;
	}
	osMessageQueuePut(pid_queueHandleHandle,&b,0,0);
	

	
  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void delay_us1(uint32_t us)
{
    uint32_t ticks_start = SysTick->VAL;                        
    uint32_t ticks_per_us = DELAY_US_TIM_CLK_HZ / 1000000;     
    uint32_t ticks_delay = us * ticks_per_us;                   
    uint32_t ticks_target = (ticks_start - ticks_delay) & 0xFFFFFF; 
    
   
    while(1) 
    {
        uint32_t ticks_current = SysTick->VAL;
        if(ticks_current <= ticks_target) 
        {
           
            if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) 
            {
                break;
            }
        }
        
        if((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) && 
           (ticks_current > ticks_target)) 
        {
            break;
        }
    }
}
void parse_jy901_data(uint8_t type, uint8_t* buf) {
    int16_t raw[4]; // 原始16位数据缓存
	
     const float g = 9.80665;
    switch(type) {
    case 0x51: // 加速度
        for(int i=0; i<3; i++) {
			if(bb<20)
			{
            raw[i] = (int16_t)((buf[2*i+1]<<8) | buf[2*i]);
			
			if(i==0)
			{
				imu_acc_base[i]+= raw[i] / 32768.0f * 16*g+sinf(imu_angle[1]/360*M_PI*2)*g;
				if(bb==19)
				{
					imu_acc_base[i]=imu_acc_base[i]/20;
				}
				
			}
			if(i==1)
			{
				imu_acc_base[i]+= raw[i] / 32768.0f *  16*g-sinf(imu_angle[0]/360*M_PI*2)*g*cosf(imu_angle[1]/360*M_PI*2);
				if(bb==19)
				{
					imu_acc_base[i]=imu_acc_base[i]/20;
				}
			}
			if(i==2)
			{
				m=cosf(imu_angle[0]/360*M_PI*2)*g*cosf(imu_angle[1]/360*M_PI*2);
				imu_acc_base[i]+= raw[i] / 32768.0f *  16*g-cosf(imu_angle[0])*g*cosf(imu_angle[1]);
				if(bb==19)
				{
					imu_acc_base[i]=imu_acc_base[i]/20;
				}
				
			}
			}
			else
			{
				bb=200;
				raw[i] = (int16_t)((buf[2*i+1]<<8) | buf[2*i]);
				if(i==0)
			{
				//imu_data.acc[i]= raw[i] / 32768.0f *  16*g+sinf(imu_angle[1]/360*M_PI*2)*g-imu_acc_base[i];
				imu_data.acc[i]= raw[i] / 32768.0f *  16*g+sinf(imu_angle[1]/360*M_PI*2)*g;
				
			}
			if(i==1)
			{
				//imu_data.acc[i]= raw[i] / 32768.0f *  16*g-sinf(imu_angle[0]/360*M_PI*2)*g*cosf(imu_angle[1]/360*M_PI*2)-imu_acc_base[i];
				imu_data.acc[i]= raw[i] / 32768.0f *  16*g-sinf(imu_angle[0]/360*M_PI*2)*g*cosf(imu_angle[1]/360*M_PI*2);
			}
			if(i==2)
			{
				m=cosf(imu_angle[0]/360*M_PI*2)*g*cosf(imu_angle[1]/360*M_PI*2);
				//imu_data.acc[i]= raw[i] / 32768.0f *  16*g-m-imu_acc_base[i];
				imu_data.acc[i]= raw[i] / 32768.0f *  16*g-m;
				
			}
				
			}
			
			
		}
	
		if(bb<20)
		{
			bb+=1;
		}

        
        break;

    case 0x52: // 角速度
        for(int i=0; i<3; i++) {
            raw[i] = (int16_t)((buf[2*i+1]<<8) | buf[2*i]);
            imu_data.gyro[i] = raw[i] / 32768.0f * 2000.0f * (M_PI/180.0f); // 量程±2000°/s转rad/s
        }
        break;

    case 0x53: // 欧拉角
        for(int i=0; i<3; i++) {
            raw[i] = (int16_t)((buf[2*i+1]<<8) | buf[2*i]);
			imu_angle[i]=raw[i] / 32768.0f * 180.0f;
            //imu_data.angle[i] = raw[i] / 32768.0f * 180.0f; // 量程±180°
			if(aa<20)
			{
				
			imu_angle_base[i]+=raw[i] / 32768.0f * 180.0f;  
				
				if(aa==19)
				{
					imu_angle_base[i]=imu_angle_base[i]/20;
				}
			}
			else
			{
				aa=200;
				imu_data.angle[i] = raw[i] / 32768.0f * 180.0f-imu_angle_base[i];
				//imu_angle[i]=raw[i] / 32768.0f * 180.0f;
				if(imu_data.angle[i]>180)
				{
					imu_data.angle[i]=imu_data.angle[i]-360;
				}
				if(imu_data.angle[i]<-180)
				{
					imu_data.angle[i]+=360;
				}
			}
        }
		if(aa<20)
		{
		aa+=1;
		}
//		
        break;

    // 其他数据类型的处理...
    }
}
uint16_t  detect_Obstacle()
{
	uint16_t judge=0;
	if(length1>420&&length2>420&&length3>420)
	//if(length1>250&&length2>250)
	{
		judge=CLEAR_PATH;
	}
	else if(length1<420&&length2>420&&length3>420)
	//else if(length1<250&&length2>250)
	{
		judge=OBSTACLE_CENTER;
	}
	else if(length1>420&&length2<420&&length3>420)
	{
		judge=OBSTACLE_LEFT;
	}
	else if(length1>420&&length2>420&&length3<420)
	{
		judge=OBSTACLE_RIGHT;
	}
	else if(length1<420&&length2<420&&length3>420)
	{
		judge=  OBSTACLE_LEFT_CENTER;
	}
	else if(length1<420&&length2>420&&length3<420)
	{
		judge=OBSTACLE_CENTER_RIGHT;
	}
	else if(length1>420&&length2<420&&length3<420)
	{
		judge=OBSTACLE_LEFT_RIGHT;
	}
	else if(length1<420&&length2<420&&length3<420)
	{
		judge= OBSTACLE_ALL_SIDES;
	}
	return judge;
}
void init_bpf(BiquadFilter* filter, float f_low, float f_high, float deltaT) 
{
	static uint16_t a=0;
    
    float RC_hpf = 1.0f / (2 * M_PI * f_low);       // 高通时间常数
    filter->alpha_hpf = RC_hpf / (RC_hpf + deltaT); // 高通滤波系数

    
    float RC_lpf = 1.0f / (2 * M_PI * f_high);      // 低通时间常数
    filter->alpha_lpf =1/(1+RC_lpf/deltaT) ;    // 低通滤波系数

    
	if(a==0)
	{
    filter->hpf_prev_in = 0.0f;
    filter->hpf_prev_out = 0.0f;
    filter->lpf_prev_out = 0.0f;
    a=1;
	}
}
float apply_bpf(BiquadFilter* filter, float input) 
{
    
    float hpf_out = filter->alpha_hpf * filter->hpf_prev_out + 
                    filter->alpha_hpf * (input - filter->hpf_prev_in);
    
   
    filter->hpf_prev_in = input;
    filter->hpf_prev_out = hpf_out;

   
    float lpf_out = filter->alpha_lpf * filter->lpf_prev_out + 
                    (1 - filter->alpha_lpf) * hpf_out;
    
    
    filter->lpf_prev_out = lpf_out;

    return lpf_out;
}


	
/* USER CODE END 1 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "cmsis_os.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "stm32f4xx_it.h"
#include "math.h"
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

/* USER CODE BEGIN PV */
uint8_t imu_buffer[2][10];
static PIDdata SP={0} ;
static PIDdata SP1={0} ;
static PIDdata SP2={0} ;
static PIDdata SP3={0} ;
static PIDdata target0={0} ;
static PIDdata target1={0} ;
static PIDdata turnangle={0} ;
static locationdata location={0};
static PIDdata lengthPID1={0};
static PIDdata lengthPID2={0};
locationdata *locate=&location;
PIDdata *speed=&SP; 
PIDdata *speed2=&SP1;
PIDdata *speed3=&SP2;
PIDdata *speed4=&SP3;
PIDdata *ta0=&target0;
PIDdata *ta1=&target1;
PIDdata *angle=&turnangle;
PIDdata *lengthpidX=&lengthPID1;
PIDdata *lengthpidY=&lengthPID2;
Obstaclecase judge_ob;
extern uint16_t runstate;
extern JY901_Data imu_data;
uint16_t sudu=0;
 uint8_t RX_buffer[1];
extern uint8_t Uart3_Receive_buf[1];
extern uint16_t len_control;
extern OdomData Odom;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_USART1_UART_Init();
  MX_TIM9_Init();
  MX_TIM7_Init();
  MX_TIM11_Init();
  MX_USART3_UART_Init();
  MX_TIM12_Init();
  /* USER CODE BEGIN 2 */
  DWT_Init();
 PID_init();
 // RC_Init(imu_buffer[0],imu_buffer[1],imu_buflength);
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start(&htim2);
     HAL_TIM_Base_Start_IT(&htim6);
    HAL_TIM_Base_Start(&htim9);
	 HAL_TIM_Base_Start_IT(&htim12);

  //  HAL_TIM_Base_Start(&htim10);;
   uint8_t rx_byte;
  
HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
	// motor_run(500,500,500,500);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
//  // motor_turn_Left(1200,1200,1200,1200);
//  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
//		
//	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
//		
//	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);
//		
  HAL_Delay(1000);	
  HAL_UART_Receive_IT(&huart3,Uart3_Receive_buf,1);
  HAL_TIM_Base_Start_IT(&htim11);
 


   
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
uint16_t get_encoder()
{
	return __HAL_TIM_GET_COUNTER(&htim3); 
}
uint16_t get_encoder1()
{
	return __HAL_TIM_GET_COUNTER(&htim4); 
}
uint16_t get_encoder2()
{
	return __HAL_TIM_GET_COUNTER(&htim5); 
}
uint16_t get_encoder3()
{
	return __HAL_TIM_GET_COUNTER(&htim8); 
}

// ΢�뼶��ʱ����
void delay_us(uint32_t us) {
	uint16_t cnt=0;
   HAL_TIM_Base_Stop(&htim9);
	 __HAL_TIM_SET_COUNTER(&htim9, 0);
	HAL_TIM_Base_Start(&htim9);         // 计数器归零
    while (__HAL_TIM_GET_COUNTER(&htim9) < us)
	{
		cnt+=1;
	}
		// 等待计数器达到目标值
}
int16_t speedPID( uint16_t target,float *vbefore,float V,PIDdata *speed)
{
	
	if(V<0)
	{
		V=V*-1;
	}
	 int16_t error=target-V;
	 speed->Integral+=speed->KI*error;
	 int16_t weifen=V-*vbefore;
	*vbefore=V;
	//speed->LimitIntergal=target*2.5;
	if(speed->Integral>=speed->LimitIntergal)
	{
		speed->Integral=speed->LimitIntergal;
	}
	if(speed->Integral<=(speed->LimitIntergal)*(-1))
	{
		speed->Integral=(speed->LimitIntergal)*(-1);
	}
	int16_t output=speed->KP*error+speed->Integral+speed->KD*weifen;
	//speed->LimitOutput=target*3;
	if(output>=speed->LimitOutput)
	{
		output=speed->LimitOutput;
	}
	if(output<=0)
	{
		output=10;
	}
	
	
	return output; 
	
}
int16_t direct_PID(int16_t target,int16_t *anglebefore,int16_t *intergram,int16_t angle ,PIDdata *tar)
{
	int16_t error=target-angle;
	*intergram+=tar->KI*error;
	int16_t wei=angle-*anglebefore;
	*anglebefore=angle;
	if(*intergram>=tar->LimitIntergal)
	{
		*intergram=tar->LimitIntergal;
	}
	if(*intergram<=((tar->LimitIntergal)*(-1)))
	{
		*intergram=(tar->LimitIntergal)*(-1);
	}
	int16_t output=tar->KP*error+*intergram+tar->KD*wei;
	
	if(output>=tar->LimitOutput)
	{
		output=tar->LimitOutput;
	}
	if(output<=(tar->LimitOutput)*(-1))
	{
		output=(-tar->LimitOutput*(-1));
	}
	return output;
	
}
void PID_init()
{
	speed->Integral=200;
	speed->KD=-1.5;
	speed->KI=0;
	speed->KP=25; 	
	speed->LimitIntergal=1400;
	speed->LimitOutput=1600;
	speed->limitoutput_fanzhuan=-200;  
   
	speed2->Integral=200;
	speed2->KD=-1.5;
	speed2->KI=0;
	speed2->KP=25; 	
	speed2->LimitIntergal=1400;
	speed2->LimitOutput=1600;
	speed2->limitoutput_fanzhuan=-200;  

	speed3->Integral=200;
	speed3->KD=-1.5;
	speed3->KI=0;
	speed3->KP=25; 	
	speed3->LimitIntergal=1400;
	speed3->LimitOutput=1600;
	speed3->limitoutput_fanzhuan=-200;  
	

	speed4->Integral=200;
	speed4->KD=-1.5;
	speed4->KI=0;
	speed4->KP=25; 	
	speed4->LimitIntergal=1400;
	speed4->LimitOutput=1600;
	speed4->limitoutput_fanzhuan=-200;  
	
	ta0->Integral=200;
	ta0->KD=10;
	ta0->KI=0.5;
	ta0->KP=0.05; 	
	ta0->LimitIntergal=200;
	ta0->LimitOutput=200;
	
	ta1->Integral=200;
	ta1->KD=10;
	ta1->KI=0.5;
	ta1->KP=0.05; 	
	ta1->LimitIntergal=200;
	ta1->LimitOutput=200;
	
	angle->KP=25;
	angle->KI=10;
	angle->KD=-0.9 ; 
//    angle->KP=4;
//	angle->KI=0.5;
//	angle->KD=-0.5 ;

	angle->LimitIntergal=1600;
	angle->LimitOutput=1900;
	angle->Integral=50;
	
    lengthpidX->KP=20;
	lengthpidX->KI=0.8;
	lengthpidX->KD=-0.3;
	lengthpidX->LimitIntergal=500;
	lengthpidX->Integral=0;
	lengthpidX->LimitOutput=700;
	
	lengthpidY->KP=20;
	lengthpidY->KI=0.8;
	lengthpidY->KD=-0.3;
	lengthpidY->LimitIntergal=500;
	lengthpidY->Integral=0;
	lengthpidY->LimitOutput=700;
	
	
	

	

	
}
void PID_reinit()
{
	
	speed->KI=2.4;
	speed->KP=22;
	speed->Integral=200;
	speed2->KI=2.4;
	speed2->KP=22;
	speed2->Integral=200;
	speed3->KI=2.4;
	speed3->KP=22;
	speed3->Integral=200;
	speed4->KI=2.4;
	speed4->KP=22;
	speed4->Integral=200;
	angle->Integral=150;
	
	
}

float  MYabs(float a,float b)
{
	if((a-b)>0)
	{
		return a-b;
	}
	else
	{
		return b-a;
	}
}
float V_caculate(uint16_t *temp1,uint16_t *temp11,uint16_t *counter_temp1,uint16_t *counter_tempbefore1,uint16_t a)
{
	     if(*temp1>20000)
		 {
			 *temp1=*temp11;
		 }
			*counter_temp1=MYabs(*temp1,*temp11);
		 static uint16_t m1=0; 
		 static uint16_t m2=0;
		  static uint16_t m3=0;
		  static uint16_t m4=0;
		 switch(a)
		 {
		case 1:
		{
		 
		if(runstate==2|runstate==4)
		{
		if(*temp1>17000&&*temp11<3000)
		{
		
		
			*counter_temp1=*temp11+20000-*temp1;
		
	}
}
		if(runstate==1|runstate==3)
		{
		if(*temp11>17000&&*temp1<3000)
		{
		
		
			*counter_temp1=*temp1+20000-*temp11;
		
	}
}
		*counter_temp1=MYabs(*counter_temp1,0);
        if(runstate!=5)
		{
		if(*counter_temp1==0)
		{
			*counter_temp1=*counter_tempbefore1;
			m1+=1;
			if(m1>4)
			{
				*counter_temp1=0;
				m1=5;
			}
		
		}
		else{
		*counter_tempbefore1=*counter_temp1;
			m1=0;
			
		}
	}
	*temp11=*temp1;	
	
	float V=*counter_temp1/0.02/13/4/30*M_PI*7.5;
	if(V>1000)
	{
		V=0;
	}
	
		return V;
	}
	case 2:
		{
		if(runstate==2|runstate==3)
		{
		if(*temp1>17000&&*temp11<3000)
		{
		
		
			*counter_temp1=*temp11+20000-*temp1;
		
	}
}
		if(runstate==1|runstate==4)
		{
		if(*temp11>17000&&*temp1<3000)
		{
		
		
			*counter_temp1=*temp1+20000-*temp11;
		
	}
}
		*counter_temp1=MYabs(*counter_temp1,0);
         if(runstate!=5)
		{
		if(*counter_temp1==0)
		{
			*counter_temp1=*counter_tempbefore1;
			m2+=1;
			if(m2>4)
			{
				*counter_temp1=0;
				m2=5;
			}
		}
		else{
		*counter_tempbefore1=*counter_temp1;
			m2=0;
		}
	}
	*temp11=*temp1;	
	float V=*counter_temp1/0.02/13/4/30*M_PI*7.5;
    	if(V>1000)
	{
		V=0;
	}
	
		return V;
	}
	case 3:
		{
		if(runstate==1|runstate==4)
		{
		if(*temp1>17000&&*temp11<3000)
		{
		
		
			*counter_temp1=*temp11+20000-*temp1;
		
	}
}
		if(runstate==2|runstate==3)
		{
		if(*temp11>17000&&*temp1<3000)
		{
		
		
			*counter_temp1=*temp1+20000-*temp11;
		
	}
}
		*counter_temp1=MYabs(*counter_temp1,0);
        if(runstate!=5)
		{
		
		if(*counter_temp1==0)
		{
			*counter_temp1=*counter_tempbefore1;	
			m3+=1;
			if(m3>4)
			{

				*counter_temp1=0;
				m3=5;
			}
			
		}
		else{
		*counter_tempbefore1=*counter_temp1;
			m3=0;
		}
	}
	*temp11=*temp1;	
	float V=*counter_temp1/0.02/13/4/30*M_PI*7.5;
    	if(V>1000)
	{
		V=0;
	}
	
		return V;
	}
	case 4:
		{
		if(runstate==1|runstate==3)
		{
		if(*temp1>17000&&*temp11<3000)
		{
		
		
			*counter_temp1=*temp11+20000-*temp1;
		
	}
}
		if(runstate==2|runstate==4)
		{
		if(*temp11>17000&&*temp1<3000)
		{
		
		
			*counter_temp1=*temp1+20000-*temp11;
		
	}
}
		*counter_temp1=MYabs(*counter_temp1,0);
        if(runstate!=5)
		{
		if(*counter_temp1==0)
		{
			*counter_temp1=*counter_tempbefore1;
			m4+=1;
			if(m4>4)
			{
				*counter_temp1=0;
				m4=5;
			}
		}
		else{
		*counter_tempbefore1=*counter_temp1;
			m4=0;
		}
	*temp11=*temp1;	
	float V=*counter_temp1/0.02/13/4/30*M_PI*7.5;
		if(V>1000)
	{
		V=0;
	}
	
		return V;
	}
}
	}
		 
}
void turn_fixed_angle(float angle_begin ,float angle_target,uint16_t time)
{
	static float error=0;
	static float error_before=0;
	
	static float weifen=0;
	float change_angle;
	change_angle=MYabs(angle_begin,angle_target);
	if(angle_target>=90&&angle_begin<=-90)
	{
		change_angle=360-angle_target+angle_begin;
	}
	if(angle_begin>=90&&angle_target<=-90)
	{
		change_angle=360-angle_begin+angle_target;
	}
	if(angle_begin<-90&&angle_target>90)
	{
		if(imu_data.angle[2]<-90)
		{
			error=180-angle_target+imu_data.angle[2]+180;
		}
		if(imu_data.angle[2]>0)
		{
			error=imu_data.angle[2]-angle_target;
		}
	    angle->Integral+=angle->KI*error;
		weifen=change_angle-error;
		error_before = error;
		if(MYabs(error,0)<25&&MYabs(error,0)>15)
		{
		angle->LimitIntergal=MYabs(error,0)*(30-MYabs(error,0))*30;
		}
		else if(MYabs(error,0)<15&&MYabs(error,0)>5)
		{
			angle->LimitIntergal=MYabs(error,0)*(19-MYabs(error,0))*28;
			if(change_angle>50)
			{
				angle->LimitIntergal+=50;
			}
			else
			{
				angle->LimitIntergal+=change_angle;
			}
		}
		else if(MYabs(error,0)<5&&MYabs(error,0)>1.5)
		{
			angle->LimitIntergal=1200;
		}
		else if(MYabs(error,0)<=1.5)
		{
			angle->LimitIntergal=0;
			weifen=0;
		}
//		if(angle->LimitIntergal>=change_angle*4)
//		{
//			angle->LimitIntergal=change_angle*4;
//		}
		if(angle->Integral>angle->LimitIntergal)
		{
			angle->Integral=angle->LimitIntergal;
		}
		if(angle->Integral<=(angle->LimitIntergal*-1))
		{
			angle->Integral=angle->LimitIntergal*-1;
		}
		int16_t output=angle->KP*error+ angle->Integral+angle->KD*weifen;
		if(output>angle->LimitOutput)
		{
			output=angle->LimitOutput;
		}
		if(output<angle->LimitOutput*(-1))
		{
			output==angle->LimitOutput*(-1);
		}
		if(output>0)
		{
			motor_turn_Right(output+12*time,output+12*time,output+12*time,output+12*time);
		}
		else
		{
			motor_turn_Left(-1*output+12*time,-1*output+12*time,-1*output+12*time,-1*output+12*time);
		}
	}
	else if(angle_begin>=90&&angle_target<=-90)
	{
		if(imu_data.angle[2]>90)
		{
			error=180+angle_target-imu_data.angle[2]+180;
		}
		if(imu_data.angle[2]<0)
		{
			error=angle_target-imu_data.angle[2];
		}
	    angle->Integral+=angle->KI*error;
		weifen=change_angle-error;
		error_before = error;
		if(MYabs(error,0)<25&&MYabs(error,0)>15)
		{
		angle->LimitIntergal=MYabs(error,0)*(30-MYabs(error,0))*30;
		}
        else if(MYabs(error,0)<15&&MYabs(error,0)>5)
		{
			angle->LimitIntergal=MYabs(error,0)*(19-MYabs(error,0))*28;
			if(change_angle>50)
			{
				angle->LimitIntergal+=100;
			}
			else
			{
				angle->LimitIntergal+=change_angle;
			}
		}
		else if(MYabs(error,0)<5&&MYabs(error,0)>1.5)
		{
			angle->LimitIntergal=1200;
		}
		else if(MYabs(error,0)<=1.5)
		{
			angle->LimitIntergal=0;
			weifen=0;
		}
//		if(angle->LimitIntergal>=change_angle*5)
//		{
//			angle->LimitIntergal=change_angle*5;
//		}
		if(angle->Integral>angle->LimitIntergal)
		{
			angle->Integral=angle->LimitIntergal;
		}
		if(angle->Integral<=(angle->LimitIntergal*-1))
		{
			angle->Integral=angle->LimitIntergal*-1;
		}
		int16_t output=angle->KP*error+ angle->Integral+angle->KD*weifen;
		if(output>angle->LimitOutput)
		{
			output=angle->LimitOutput;
		}
		if(output<angle->LimitOutput*-1)
		{
			output=angle->LimitOutput*-1;
		}
		if(output>0)
		{
			motor_turn_Left(output+12*time,output+12*time,output+12*time,output+12*time);
		}
		else if(output<=0)
		{
		motor_turn_Right(output*-1+12*time,output*-1+12*time,output*-1+12*time,output*-1+12*time);
		}
	}
	else
	{   
		if(imu_data.angle[2]>90&&angle_target<-90)
		{
			error=180-imu_data.angle[2]+angle_target+180;
		}
		else if(imu_data.angle[2]<-90&&angle_target>90)
		{
			error=-1*(imu_data.angle[2]+180+180-angle_target);
		}
		else 
		{
			error=angle_target-imu_data.angle[2];
		}
		angle->Integral+=angle->KI*error;
		//angle->LimitIntergal=MYabs(error,0)*10;
		if(MYabs(error,0)<25&&MYabs(error,0)>15)
		{
		angle->LimitIntergal=MYabs(error,0)*(30-MYabs(error,0))*30+20;
		}
        else if(MYabs(error,0)<15&&MYabs(error,0)>5)
		{
			angle->LimitIntergal=MYabs(error,0)*(19-MYabs(error,0))*28;
			if(change_angle>30)
			{
				angle->LimitIntergal+=100;
			}
//			else
//			{
//				angle->LimitIntergal+=change_angle;
//			}
		}
		else if(MYabs(error,0)<5&&MYabs(error,0)>1.5)
		{
			angle->LimitIntergal=1200;

		}
		else if(MYabs(error,0)<=1.5)
		{
			angle->LimitIntergal=0;
			weifen=0;
		}
//		if(angle->LimitIntergal>=change_angle*5)
//		{
//			angle->LimitIntergal=change_angle*5;
//		}
		weifen=change_angle-error;
		error_before = error;
		if(angle->Integral>angle->LimitIntergal)
		{
			angle->Integral=angle->LimitIntergal;
		}
		if(angle->Integral<=(angle->LimitIntergal*-1))
		{
			angle->Integral=angle->LimitIntergal*-1;
		}
		int16_t output=angle->KP*error+angle->Integral+angle->KD*weifen;
		if(output>angle->LimitOutput)
		{
			output=angle->LimitOutput;
		}
		if(-1*output>angle->LimitOutput)
		{
			output=angle->LimitOutput*-1;
		}
		
		if(output>=0)
		{
			motor_turn_Left(output+12*time,output+12*time,output+12*time,output+12*time);
		}
		else
		{
			motor_turn_Right(-1*output+12*time,-1*output+12*time,-1*output+12*time,-1*output+12*time);
		}
	}
	
}	

//void turn_fixed_angle(float angle_begin ,float angle_target,uint16_t time)
//{
//	static float error=0;
//	static float error_before=0;
//	
//	static float weifen=0;
//	float change_angle;
//	change_angle=MYabs(angle_begin,angle_target);
//	if(angle_target>=90&&angle_begin<=-90)
//	{
//		change_angle=360-angle_target+angle_begin;
//	}
//	if(angle_begin>=90&&angle_target<=-90)
//	{
//		change_angle=360-angle_begin+angle_target;
//	}
//	if(angle_begin<-90&&angle_target>90)
//	{
//		if(imu_data.angle[2]<-90)
//		{
//			error=180-angle_target+imu_data.angle[2]+180;
//		}
//		if(imu_data.angle[2]>0)
//		{
//			error=imu_data.angle[2]-angle_target;
//		}
//	    angle->Integral+=angle->KI*error;
//		weifen=change_angle-error;
//		error_before = error;
//		if(MYabs(error,0)<25&&MYabs(error,0)>15)
//		{
//		angle->LimitIntergal=MYabs(error,0)*(27-MYabs(error,0));
//		}
//		else if(MYabs(error,0)<15&&MYabs(error,0)>5)
//		{
//			angle->LimitIntergal=MYabs(error,0)*(18-MYabs(error,0));
//			if(change_angle>50)
//			{
//				angle->LimitIntergal+=10;
//			}
////			else
////			{
////				angle->LimitIntergal+=change_angle;
////			}
//		}
//		else if(MYabs(error,0)<5&&MYabs(error,0)>1.5)
//		{
//			angle->LimitIntergal=50;
//		}
//		else if(MYabs(error,0)<=1.5)
//		{
//			angle->LimitIntergal=0;
//			weifen=0;
//		}
//		if(angle->LimitIntergal>=change_angle*4)
//		{
//			angle->LimitIntergal=change_angle*4;
//		}
//		if(angle->Integral>angle->LimitIntergal)
//		{
//			angle->Integral=angle->LimitIntergal;
//		}
//		if(angle->Integral<=(angle->LimitIntergal*-1))
//		{
//			angle->Integral=angle->LimitIntergal*-1;
//		}
//		int16_t output=angle->KP*error+ angle->Integral+angle->KD*weifen;
//		if(output>angle->LimitOutput)
//		{
//			output=angle->LimitOutput;
//		}
//		if(output<angle->LimitOutput*(-1))
//		{
//			output==angle->LimitOutput*(-1);
//		}
//		if(output>0)
//		{
//			motor_turn_Right(output+12*time,output+12*time,output+12*time,output+12*time);
//		}
//		else
//		{
//			motor_turn_Left(-1*output+12*time,-1*output+12*time,-1*output+12*time,-1*output+12*time);
//		}
//	}
//	else if(angle_begin>=90&&angle_target<=-90)
//	{
//		if(imu_data.angle[2]>90)
//		{
//			error=180+angle_target-imu_data.angle[2]+180;
//		}
//		if(imu_data.angle[2]<0)
//		{
//			error=angle_target-imu_data.angle[2];
//		}
//	    angle->Integral+=angle->KI*error;
//		weifen=change_angle-error;
//		error_before = error;
//		if(MYabs(error,0)<25&&MYabs(error,0)>15)
//		{
//		angle->LimitIntergal=MYabs(error,0)*(27-MYabs(error,0));
//		}
//        else if(MYabs(error,0)<15&&MYabs(error,0)>5)
//		{
//			angle->LimitIntergal=MYabs(error,0)*(18-MYabs(error,0));
//			if(change_angle>50)
//			{
//				angle->LimitIntergal+=10;
//			}
////			else
////			{
////				angle->LimitIntergal+=change_angle;
////			}
//		}
//		else if(MYabs(error,0)<5&&MYabs(error,0)>1.5)
//		{
//			angle->LimitIntergal=50;
//		}
//		else if(MYabs(error,0)<=1.5)
//		{
//			angle->LimitIntergal=0;
//			weifen=0;
//		}
//		if(angle->LimitIntergal>=change_angle*5)
//		{
//			angle->LimitIntergal=change_angle*5;
//		}
//		if(angle->Integral>angle->LimitIntergal)
//		{
//			angle->Integral=angle->LimitIntergal;
//		}
//		if(angle->Integral<=(angle->LimitIntergal*-1))
//		{
//			angle->Integral=angle->LimitIntergal*-1;
//		}
//		int16_t output=angle->KP*error+ angle->Integral+angle->KD*weifen;
//		if(output>angle->LimitOutput)
//		{
//			output=angle->LimitOutput;
//		}
//		if(output<angle->LimitOutput*-1)
//		{
//			output=angle->LimitOutput*-1;
//		}
//		if(output>0)
//		{
//			motor_turn_Left(output+12*time,output+12*time,output+12*time,output+12*time);
//		}
//		else if(output<=0)
//		{
//		motor_turn_Right(output*-1+12*time,output*-1+12*time,output*-1+12*time,output*-1+12*time);
//		}
//	}
//	else
//	{   
//		if(imu_data.angle[2]>90&&angle_target<-90)
//		{
//			error=180-imu_data.angle[2]+angle_target+180;
//		}
//		else if(imu_data.angle[2]<-90&&angle_target>90)
//		{
//			error=-1*(imu_data.angle[2]+180+180-angle_target);
//		}
//		else 
//		{
//			error=angle_target-imu_data.angle[2];
//		}
//		angle->Integral+=angle->KI*error;
//		angle->LimitIntergal=MYabs(error,0)*10;
//		if(MYabs(error,0)<25&&MYabs(error,0)>15)
//		{
//		angle->LimitIntergal=MYabs(error,0)*(27-MYabs(error,0))+20;
//		}
//        else if(MYabs(error,0)<15&&MYabs(error,0)>5)
//		{
//			angle->LimitIntergal=MYabs(error,0)*(18-MYabs(error,0));
//			if(change_angle>30)
//			{
//				angle->LimitIntergal+=10;
//			}
////			else
////			{
////				angle->LimitIntergal+=change_angle;
////			}
//		}
//		else if(MYabs(error,0)<5&&MYabs(error,0)>1.5)
//		{
//			angle->LimitIntergal=50;

//		}
//		else if(MYabs(error,0)<=1.5)
//		{
//			angle->LimitIntergal=0;
//			weifen=0;
//		}
//		if(angle->LimitIntergal>=change_angle*5)
//		{
//			angle->LimitIntergal=change_angle*5;
//		}
//		weifen=change_angle-error;
//		error_before = error;
//		if(angle->Integral>angle->LimitIntergal)
//		{
//			angle->Integral=angle->LimitIntergal;
//		}
//		if(angle->Integral<=(angle->LimitIntergal*-1))
//		{
//			angle->Integral=angle->LimitIntergal*-1;
//		}
//		int16_t output=angle->KP*error+angle->Integral+angle->KD*weifen;
//		if(output>angle->LimitOutput)
//		{
//			output=angle->LimitOutput;
//		}
//		if(-1*output>angle->LimitOutput)
//		{
//			output=angle->LimitOutput*-1;
//		}
//		
//		if(output>=0)
//		{
//			motor_turn_Left(output+12*time,output+12*time,output+12*time,output+12*time);
//		}
//		else
//		{
//			motor_turn_Right(-1*output+12*time,-1*output+12*time,-1*output+12*time,-1*output+12*time);
//		}
//	}
//	
//	
//}
void locateenter(float *v1,float *v2,float *v3,float *v4,float dalayt)
{
	locate->Vtx=(*v1+*v2+*v3+*v4)/4;
	locate->Vty=(*v3-*v1+*v2-*v4)/4;
	locate->relativeX+=locate->Vtx*dalayt;
	locate->relativeY+=locate->Vty*dalayt;
	float angle=imu_data.angle[2]*2*M_PI/360;
	locate->VX=locate->Vtx*cosf(angle)-locate->Vty*sinf(angle);
	locate->VY=locate->Vtx*sinf(angle)+locate->Vty*cosf(angle);
	locate->locationX+=locate->VX*dalayt;
	locate->locationY+=locate->VY*dalayt;
	locate->W=(*v4-*v3)/(100-50);
	Odom.x=locate->locationX/100;
	Odom.y=locate->locationY/100;
	Odom.yaw=imu_data.angle[2];
	
}
float length_control(float lengthnow,float target,PIDdata *lengthpid)
{
	static float error_before;
	float error=target-lengthnow;
	float weifen=error-error_before;
	error_before=error;
	if(error<15)
	{	
	lengthpid->Integral+=error*lengthpid->KI;
	}
	if(lengthpid->Integral>lengthpid->LimitIntergal)
	{
		lengthpid->Integral=lengthpid->LimitIntergal;
	}
		if(lengthpid->Integral<lengthpid->LimitIntergal*(-1))
	{
		lengthpid->Integral=lengthpid->LimitIntergal*(-1);
	}
	float output=lengthpid->KP*error+lengthpid->KD*weifen+lengthpid->Integral;
	if(output>lengthpid->LimitOutput)
	{
		output=lengthpid->LimitOutput;
    }
		if(output<lengthpid->LimitOutput*-1)
	{
		output=lengthpid->LimitOutput*-1;
    }
	return output;
}
void length_goX(float len)
{
	if(len_control==1)
	{
	  static uint16_t cnt=0;
	  static float target1=0;
	  static uint16_t cnt1=0;
	  if(cnt==0)
	  {
		  target1=locate->relativeX+len;
		  cnt=1;
	  }
	  float output=length_control(locate->relativeX,target1,lengthpidX);
	  if(len>=0)
	  {
	  motor_left(output,output,output,output);
	  }
	  if(len<0)
	  {
	  motor_right(output,output,output,output);
	  }
	  if(MYabs(target1,locate->relativeX)<1)
	  {
		  cnt1+=1;
		  if(cnt1>=5)
		  {
		  lengthpidX->Integral=0;
		  motor_stop();
		   len_control=2;
		  }
	  }
	  else
	  {
		  cnt1=0;
	  }
	 
  }
}
void length_goY(float len)
{
	if(len_control==0)
	{
	  static uint16_t cnt=0;
	  static float target1=0;
	 static uint16_t cnt1=0;
	  if(cnt==0)
	  {
		  target1=locate->relativeY+len;
		  cnt=1;
	  }
	  float output=length_control(locate->relativeY,target1,lengthpidY);
	  if(len>=0)
	  {
	  motor_run(output,output,output,output);
	  }
	  if(len<0)
	  {
	  motor_back(output,output,output,output);
	  }
	  if(MYabs(target1,locate->relativeY)<1)
	  {
		  cnt1+=1;
		  if(cnt1>=5)
		  {
		  lengthpidY->Integral=0;
		  motor_stop();
		  len_control=1;
		  }
	  }
	  else
	  {
		  cnt1=0;
	  }
  }
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM13 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM13)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

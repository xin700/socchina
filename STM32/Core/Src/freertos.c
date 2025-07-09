/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f4xx_it.h"
#include "motor.h"

#include "freertos.h"
#include "semphr.h"
#include "task.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint16_t cnt=0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim10;
extern JY901_Data imu_data;
extern uint16_t aa;
extern uint16_t runstate;
extern PIDdata *speed;
extern PIDdata *speed2;
extern PIDdata *speed3;
extern PIDdata *speed4;
extern PIDdata *ta0;
extern PIDdata *ta1;
extern uint16_t length1;
extern uint16_t length2;
extern uint16_t length3;
extern Obstaclecase judge_ob;

extern float V1;
extern float V2;
extern float V3;
extern float V4;
uint16_t temp1=0;
uint16_t temp2=0;
uint16_t temp3=0;
uint16_t temp4=0;
int16_t  sudu1=0;
int16_t  sudu2=0;
int16_t  sudu3=0;
int16_t  sudu4=0;
float imuprocessdata[9];
extern uint16_t bb;
uint16_t flag_angle=0;
int16_t angle_now;
float angle_begin;
float target;
float angle_true=0;
uint16_t timenow=0;
uint16_t timebefore=0;

 float acc_x_filtered=0;
 float acc_y_filtered=0;
float acc_z_filtered=0;
extern locationdata *locate;
uint16_t fangxiang=10;
extern PIDdata *lengthpid;
uint16_t len_control=1;
extern float best_angle;
uint16_t flag=0;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for imu_task */
osThreadId_t imu_taskHandle;
const osThreadAttr_t imu_task_attributes = {
  .name = "imu_task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for motor_task */
osThreadId_t motor_taskHandle;
const osThreadAttr_t motor_task_attributes = {
  .name = "motor_task",
  .stack_size = 384 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for exchange_task */
osThreadId_t exchange_taskHandle;
const osThreadAttr_t exchange_task_attributes = {
  .name = "exchange_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for imutomotor */
osMessageQueueId_t imutomotorHandle;
const osMessageQueueAttr_t imutomotor_attributes = {
  .name = "imutomotor"
};
/* Definitions for imudata */
osMessageQueueId_t imudataHandle;
const osMessageQueueAttr_t imudata_attributes = {
  .name = "imudata"
};
/* Definitions for pid_queueHandle */
osMessageQueueId_t pid_queueHandleHandle;
const osMessageQueueAttr_t pid_queueHandle_attributes = {
  .name = "pid_queueHandle"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void IMU_task(void *argument);
void MOTOR_TASK(void *argument);
void EXchangetask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of imutomotor */
  imutomotorHandle = osMessageQueueNew (32, sizeof(uint16_t), &imutomotor_attributes);

  /* creation of imudata */
  imudataHandle = osMessageQueueNew (32, sizeof(uint16_t), &imudata_attributes);

  /* creation of pid_queueHandle */
  pid_queueHandleHandle = osMessageQueueNew (20, sizeof(uint16_t), &pid_queueHandle_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of imu_task */
  imu_taskHandle = osThreadNew(IMU_task, NULL, &imu_task_attributes);

  /* creation of motor_task */
  motor_taskHandle = osThreadNew(MOTOR_TASK, NULL, &motor_task_attributes);

  /* creation of exchange_task */
  exchange_taskHandle = osThreadNew(EXchangetask, NULL, &exchange_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  
 
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
   while(1)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_IMU_task */
/**
* @brief Function implementing the imu_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_IMU_task */
void IMU_task(void *argument)
{
  /* USER CODE BEGIN IMU_task */
  /* Infinite loop */
	osStatus_t status;
	
	uint8_t data;
	
    int16_t target1=0;
	int16_t target2=0;
    int16_t target3=0;
    int16_t target4=0;
    int16_t angle_be=0;
	int16_t angle2_be=0;
	int16_t intergram=0;
	int16_t intergram2=0;
	float v1;
	float v2;
	float v3;
	BiquadFilter bpf_x, bpf_y, bpf_z;
	BiquadFilter bpfv_x, bpfv_y, bpfv_z;
    init_bpf(&bpf_x, 0.1f, 50.0f, 0.001f);
    init_bpf(&bpf_y, 0.1f, 50.0f, 0.001f);
    init_bpf(&bpf_z, 0.1f, 50.0f, 0.001f);
	   init_bpf(&bpfv_x, 0.1f, 50.0f, 0.001f);
    init_bpf(&bpfv_y, 0.1f, 50.0f, 0.001f);
    init_bpf(&bpfv_z, 0.1f, 50.0f, 0.001f);
  while(1)
  {
	  if(osMessageQueueGet(imudataHandle,&data,0,0)==osOK)
	  {
		  timenow= __HAL_TIM_GET_COUNTER(&htim2);
		  uint16_t changet=timenow-timebefore;
		  timebefore=timenow;
		  float deltat=changet*0.000001f;
	if(imu_data.updated==1&&aa==200&&bb==200)  
	{
		     init_bpf(&bpf_x, 0.01f, 200.0f, deltat);
             init_bpf(&bpf_y, 0.01f, 200.0f, deltat);
             init_bpf(&bpf_z, 0.01f, 200.0f, deltat);
//		    acc_x_filtered = apply_bpf(&bpf_x, imu_data.acc[0]);
//            acc_y_filtered = apply_bpf(&bpf_y, imu_data.acc[1]);
//            acc_z_filtered = apply_bpf(&bpf_z, imu_data.acc[2]);
		acc_x_filtered=imu_data.acc[0];
		acc_y_filtered=imu_data.acc[1];
		acc_z_filtered=imu_data.acc[2];
		target1=direct_PID(angle_true,&angle_be,&intergram,imu_data.angle[2],ta0);
		target2=direct_PID(angle_true,&angle2_be,&intergram2,imu_data.angle[2],ta1);
		imuprocessdata[0]=imu_data.angle[2];
		imuprocessdata[1]=target1;
		imuprocessdata[2]=target2;
        init_bpf(&bpfv_x, 0.05f, 1000.0f, deltat);
        init_bpf(&bpfv_y, 0.05f, 1000.0f, deltat);
        init_bpf(&bpfv_z, 0.05f, 1000.0f, deltat);
		if(acc_x_filtered<-0.025||acc_x_filtered>0.025)
		{
		v1+=acc_x_filtered*deltat;
		}
		if(acc_y_filtered<-0.025||acc_y_filtered>0.025)
		{
		v2+= acc_y_filtered*deltat;
		}
		if(acc_z_filtered<-0.025||acc_z_filtered>0.025)
		{
		v3+=acc_z_filtered*deltat;
		}
         imuprocessdata[3]=apply_bpf(&bpfv_x,v1);
		imuprocessdata[4]=apply_bpf(&bpfv_y, v2);
		imuprocessdata[5]=apply_bpf(&bpfv_z, v3);
		if(imuprocessdata[3]<-0.04||imuprocessdata[3]>0.04)
		{
		imuprocessdata[6]+=imuprocessdata[3]*deltat;
		}
		if(imuprocessdata[4]<-0.04||imuprocessdata[4]>0.04)
		{
		imuprocessdata[7]+=imuprocessdata[4]*deltat;
		}
		if(imuprocessdata[5]<-0.04||imuprocessdata[5]>0.04)
		{
		imuprocessdata[8]+=imuprocessdata[5]*deltat;
		}
		
		
		imu_data.updated=0;
	//	motor_run(300,300,300,300);
	} 
}
	if(imu_data.updated==0)
	{
	osDelay(5);
	}
  }
  /* USER CODE END IMU_task */
}

/* USER CODE BEGIN Header_MOTOR_TASK */
/**
* @brief Function implementing the motor_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MOTOR_TASK */
void MOTOR_TASK(void *argument)
{
  /* USER CODE BEGIN MOTOR_TASK */
  /* Infinite loop */
	HAL_TIM_Base_Start_IT(&htim7);

	uint8_t received_array[6];
	osStatus_t status;	
	 int16_t  sudu1_be=0;
	
	 int16_t  sudu2_be=0;
	
	 int16_t  sudu3_be=0;
	
	 int16_t  sudu4_be=0;
	 int i=0;
	 float V1_be=0;
	 float V2_be=0;
	 float V3_be=0;
	 float V4_be=0;
     uint16_t first_time=0;
	 uint16_t j=0;
	  uint16_t time=0;
	  static uint16_t last_judge=0;
	   uint16_t V_cnt=0;
	   uint16_t turn_cnt=0;
	   uint16_t last_turn=0;
	   uint16_t angle_break=0;
	   int16_t turncircle=0;
	   uint16_t mode=1;
	   uint16_t step=1;

  while(1)
  {
	  if(osMessageQueueGet(pid_queueHandleHandle,&time,0,0)==osOK)
	   
	  {
	   if(aa==200&&first_time<5){
		   PID_init();
		 //  PID_reinit();
//		  speed->Integral=200; 
//		  speed2->Integral=200; 
//		  speed3->Integral=200; 
//		  speed4->Integral=200; 
		   first_time+=1;
	   }
	   else if(first_time>=5)
	   {

		  {


			  //sudu1=speedPID(500+imuprocessdata[1],&V1_be,v1,speed);
			  sudu1=speedPID(30,&V2_be,V2,speed);
			  if (V1>10&&j==0)
			  {
				  PID_reinit();
				  j+=1;
			  }
			  if(sudu1==0)
			  {
				  sudu1=sudu1_be;
			  }
			  else
			  {
				  sudu1_be=sudu1;
			  }
			 // sudu2=speedPID(500-imuprocessdata[1],&V2_be,v2,speed2);
			  sudu2=speedPID(30,&V1_be,V1,speed2);
			  if(sudu2==0)
			  {
				  sudu2=sudu2_be;
			  }
			  else
			  {
				  sudu2_be=sudu2;
			  }
			  //sudu3=speedPID(500-imuprocessdata[2],&V3_be,v3,speed3);
			  sudu3=speedPID(30,&V3_be,V3,speed3);
			  if(sudu3==0)
			  {
				  sudu3=sudu3_be;
			  }
			  else
			  {
				  sudu3_be=sudu3;
			  }
			  //sudu4=speedPID(500+imuprocessdata[2],&V4_be,v4,speed4);
			  sudu4=speedPID(30,&V4_be,V4,speed4);
			  if(sudu4==0)
			  {
				  sudu4=sudu4_be;
			  }
			  else
			  {
				  sudu4_be=sudu4;
			  }
			  if(mode==0)
			  {
			  if(fangxiang==0)
			  {
			  motor_run(sudu1,sudu2,sudu3,sudu4);
			  }
			  if(fangxiang==1)
			  {
			   motor_back(sudu1,sudu2,sudu3,sudu4);
			  }
			  if(fangxiang==2)
			  {
				motor_turn_Left(sudu1,sudu2,sudu3,sudu4);
			  }
			  if(fangxiang ==3)
			  {
				  motor_turn_Right(sudu1,sudu2,sudu3,sudu4);
			  }
			    if(fangxiang ==4)
			  {
				  motor_stop();
			  }
			 // length_goX(50);
			  //length_goY(180);
			  
				   

		  }

//			    static uint16_t cnt=0;
//					   static uint16_t cnt1=0;
//					    
//					   if(cnt==0)
//					   {
//					    angle_begin=imu_data.angle[2];
//					    target=angle_begin+25;
//						cnt=1;
//					   }
//					   if(target>180)
//					   {
//						   target=target-360;
//					   }
//					   if(target<-180)
//					   {   
//					       target+=360;
//					   }
//					   turn_fixed_angle(angle_begin,target,time);
//					   if(MYabs(imu_data.angle[2],target)>1.5)
//					   {
//					   flag_angle==1;
//					   cnt=1;
//					   cnt1=0;
//					   }
//					   if(MYabs(imu_data.angle[2],target)<=1.5)
//					   {
//						   cnt1+=1;
//						   if(cnt1>=3)
//						   {
//							   //cnt1=0;
//							   flag_angle=0;
//							   cnt=0;
//							   motor_stop();
//							  PID_reinit();
//							   osDelay(2000);
//							   
//						   }
//					   }
                 if(mode==1)
				 {					 
					if(flag_angle==0)
					{
					judge_ob= detect_Obstacle();
					}
					if(judge_ob!=last_judge)
					{
						sudu1=650;
						sudu2=650;
						sudu3=650;
						sudu4=650;
						V_cnt=0;
					
					}
					if(V_cnt<5)
					{
						V_cnt+=1;
						
					}
			  
						
			  switch(judge_ob)
			  {
			       case 0:
				   {
			       motor_run(sudu1,sudu2,sudu3,sudu4);

				   break;
				   }
				   case 1:
				   {
					   static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    
					   if(cnt==0)
					   {
						   flag_angle=1;
						  motor_back(70,70,70,70);
						  osDelay(250);
						   motor_stop();
						  //  osDelay(300);
					    angle_begin=imu_data.angle[2];
					    target=angle_begin-25-angle_break;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>5)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<=5)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
							   cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							  PID_reinit();
							   osDelay(400);
							   turn_cnt =2;
							   angle_true=imu_data.angle[2];
							   if(last_turn!=turn_cnt&&turn_cnt !=0)
							   {
								   turncircle+=1;
								   
							   }
							   if(last_turn==turn_cnt)
							   {
								   turncircle=0;
								   angle_break=0;

								   
							   }
							   if(turncircle>=3)
							   {
								 uint16_t a=turncircle/3;
								   angle_break+=a*15;
								   
							   }
							   last_turn=turn_cnt;
							    PID_reinit();
\
							   

							   
							  
						   }
					   }
				   
					   break;
				   }
				   case 2:
				   {
					static  uint16_t m=0;
					if(length2>length3||m==1)
					{
					   static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    m=1;
					   if(cnt==0)
					   { 
						   flag_angle=1;
						  motor_back(70,70,70,70);
						  osDelay(250);
						     motor_stop();
						   
					    angle_begin=imu_data.angle[2];
					    target=angle_begin+25+angle_break;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>5)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<=5)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
							   cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							
							   turn_cnt =1;
							    angle_true=imu_data.angle[2];
							   if(last_turn!=turn_cnt&&turn_cnt !=0)
							   {
								   turncircle+=1;
								   
							   }
							   if(last_turn==turn_cnt)
							   {
								   turncircle=0;
								   angle_break=0;

								   
							   }
							   if(turncircle>=3)
							   {
								 uint16_t a=turncircle/3;
								   angle_break+=a*15;
								  
								  
								   
							   }
							   last_turn=turn_cnt;
							   osDelay(400);
							    PID_reinit();
							   m=0;
							   
						   }
					   }
				   
					   break;
					}
					else
					{
										   static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    m=0;
					   if(cnt==0)
					   {
						   flag_angle=1;
						   motor_back(70,70,70,70);
						  osDelay(250);
						     motor_stop();
					    angle_begin=imu_data.angle[2];
					    target=angle_begin-25-angle_break;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>5)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<=5)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
							   cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							  
							   turn_cnt =2;
							    angle_true=imu_data.angle[2];
							   if(last_turn!=turn_cnt&&turn_cnt !=0)
							   {
								   turncircle+=1;
								   
							   }
							   if(last_turn==turn_cnt)
							   {
								   turncircle=0;
								   angle_break=0;

								   
							   }
							   if(turncircle>=3)
							   {
								 uint16_t a=turncircle/3;
								   angle_break+=a*15;
								   
							   }
							   last_turn=turn_cnt;
							   osDelay(400);
							    PID_reinit();
							   
						   }
					   }
				   
					   break;
					}
				   }
				   case 3:
				   {
					 static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    
					   if(cnt==0)
					   {
						   flag_angle=1;
						  motor_back(70,70,70,70);
						  osDelay(250);
						     motor_stop();
					    angle_begin=imu_data.angle[2];
					    target=angle_begin+25+angle_break;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>5)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<=5)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
							   cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							   turn_cnt =1;
							    angle_true=imu_data.angle[2];
							   if(last_turn!=turn_cnt&&turn_cnt !=0)
							   {
								   turncircle+=1;
								   
							   }
							   if(last_turn==turn_cnt)
							   {
								   turncircle=0;
								   angle_break=0;

								   
							   }
							   if(turncircle>=3)
							   {
								 uint16_t a=turncircle/3;
								   angle_break+=a*15;
								   
							   }
							   last_turn=turn_cnt;
							
							   osDelay(400);
							    PID_reinit();
							   
						   }
					   }
				   
					   break;
					
				   }
				   case 4:
				   {
					   static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    
					   if(cnt==0)
					   {
						   flag_angle=1;
						  motor_back(70,70,70,70);
						  osDelay(250);
						     motor_stop();
					    angle_begin=imu_data.angle[2];
					    target=angle_begin-30-angle_break;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>5)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<5)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
							   cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							   turn_cnt =2;
							    angle_true=imu_data.angle[2];
							   if(last_turn!=turn_cnt&&turn_cnt !=0)
							   {
								   turncircle+=1;
								   
							   }
							   if(last_turn==turn_cnt)
							   {
								   turncircle=0;
								   angle_break=0;

								   
							   }
							   if(turncircle>=3)
							   {
								 uint16_t a=turncircle/3;
								   angle_break+=a*15;
								   
							   }
							   last_turn=turn_cnt;
							
							   osDelay(400);
							    PID_reinit();
							   
						   }
					   }
					   break;
					   
					   
				   }
				   case 5:
				   {
					    static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    
					   if(cnt==0)
					   {
						   flag_angle=1;
						  motor_back(70,70,70,70);
						  osDelay(250);
						     motor_stop();
					    angle_begin=imu_data.angle[2];
					    target=angle_begin+30+angle_break;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>5)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<=5)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
							   cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							   turn_cnt =1;
							    angle_true=imu_data.angle[2];
							   if(last_turn!=turn_cnt&&turn_cnt !=0)
							   {
								   turncircle+=1;
								   
							   }
							   if(last_turn==turn_cnt)
							   {
								   turncircle=0;
								   angle_break=0;

								   
							   }
							   if(turncircle>=3)
							   {
								 uint16_t a=turncircle/3;
								   angle_break+=a*15;
								   
							   }
							   last_turn=turn_cnt;
							 
							   osDelay(400);
							    PID_reinit();
							   
						   }
					   }
					   break;
					   
					   
				   }
				   case 6:
				   {
					   static  uint16_t m=0;
					   if(length2>length3||m==1)
					{
					   static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    m=1;
					   if(cnt==0)
					   { 
						   flag_angle=1;
						 motor_back(70,70,70,70);
						  osDelay(250);
						     motor_stop();
					    angle_begin=imu_data.angle[2];
					    target=angle_begin+40+angle_break;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>5)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<=5)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
							   cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							   turn_cnt =1;
							    angle_true=imu_data.angle[2];
							   if(last_turn!=turn_cnt&&turn_cnt !=0)
							   {
								   turncircle+=1;
								   
							   }
							   if(last_turn==turn_cnt)
							   {
								   turncircle=0;
								   angle_break=0;

								   
							   }
							   if(turncircle>=3)
							   {
								 uint16_t a=turncircle/3;
								   angle_break+=a*15;
								   
							   }
							   last_turn=turn_cnt;
							
							   osDelay(400);
							    PID_reinit();
							   m=0;
							   
						   }
					   }
				   
					   break;
					}
					else
					{
					   static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    m=0;
					   if(cnt==0)
					   {
						   flag_angle=1;
						   motor_back(70,70,70,70);
						  osDelay(250);
						     motor_stop();
						    
					    angle_begin=imu_data.angle[2];
					    target=angle_begin-40-angle_break;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>5)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<=5)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
							   cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							   turn_cnt =2;
							    angle_true=imu_data.angle[2];
							   if(last_turn!=turn_cnt&&turn_cnt !=0)
							   {
								   turncircle+=1;
								   
							   }
							   if(last_turn==turn_cnt)
							   {
								   turncircle=0;
								   angle_break=0;

								   
							   }
							   if(turncircle>=3)
							   {
								 uint16_t a=turncircle/3;
								   angle_break+=a*15;
								   
							   }
							   last_turn=turn_cnt;
							  
							   osDelay(400);
							    PID_reinit();
							   
						   }
					   }
				   }
					break;
				   }
				   case 7:
				   {
					   static  uint16_t m=0;
					   if(length2>length3||m==1)
					{
					   static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    m=1;
					   if(cnt==0)
					   { 
						   flag_angle=1;
						   motor_back(70,70,70,70);
						  osDelay(250);
						     motor_stop();
					    angle_begin=imu_data.angle[2];
					    target=angle_begin+60+angle_break;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>5)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<=5)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
                                cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							   turn_cnt =1;
							    angle_true=imu_data.angle[2];
							   if(last_turn!=turn_cnt&&turn_cnt !=0)
							   {
								   turncircle+=1;
								   
							   }
							   if(last_turn==turn_cnt)
							   {
								   turncircle=0;
								   angle_break=0;

								   
							   }
							   if(turncircle>=3)
							   {
								 uint16_t a=turncircle/3;
								   angle_break+=a*15;
								   
							   }
							   last_turn=turn_cnt;
							 
							   osDelay(400);
							    PID_reinit();
							   m=0;
							   
						   }
					   }
				   
					   break;
					}
					else
					{
					   static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    m=0;
					   if(cnt==0)
					   {
						   flag_angle=1;
						   motor_back(70,70,70,70);
						  osDelay(250);
						     motor_stop();
					    angle_begin=imu_data.angle[2];
					    target=angle_begin-60-angle_break;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>5)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<=5)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
                               cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							   turn_cnt =2;
							    angle_true=imu_data.angle[2];
							   if(last_turn!=turn_cnt&&turn_cnt !=0)
							   {
								   turncircle+=1;
								   
							   }
							   if(last_turn==turn_cnt)
							   {
								   turncircle=0;
								   angle_break=0;

								   
							   }
							   if(turncircle>=3)
							   {
								 uint16_t a=turncircle/3;
								   angle_break+=a*15;
								   
							   }
							   last_turn=turn_cnt;
							 
							   osDelay(400);
							    PID_reinit();
							   
						   }
					   }
				   }
					break;
					   
					   
					   
				   }
				   

			  }
		 // i=0;

		  }
		  if(mode==3)
		  {
			  if(flag_angle==0)
			  {
			  flag=forward_detect();
			  }
			  if(flag==0)
			  {
				    motor_run(sudu1,sudu2,sudu3,sudu4);
			  }
			  if(flag==1)
			  {
				  if(best_angle!=400)
				  {
				   static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    
					   if(cnt==0)
					   {
						   flag_angle=1;
						  motor_back(70,70,70,70);
						  osDelay(250);
						   motor_stop();
						  //  osDelay(300);
					    angle_begin=imu_data.angle[2];
					    target=angle_begin-best_angle;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>2)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<=2)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
							   cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							   j=0;
							PID_init();
							   osDelay(400);
							   flag=0;
							   angle_true=imu_data.angle[2];
							  
						   }
					   }
				   }
				  else
				  {
					  static uint16_t cnt=0;
					   static uint16_t cnt1=0;
					    
					   if(cnt==0)
					   {
						   flag_angle=1;
						  motor_back(70,70,70,70);
						  osDelay(250);
						   motor_stop();
						  //  osDelay(300);
					    angle_begin=imu_data.angle[2];
					    target=angle_begin-180;
						cnt=1;
					   }
					   if(target>180)
					   {
						   target=target-360;
					   }
					   if(target<-180)
					   {   
					       target+=360;
					   }
					   turn_fixed_angle(angle_begin,target,time);
					   if(MYabs(imu_data.angle[2],target)>2)
					   {
					   flag_angle=1;
					   cnt=1;
					   cnt1=0;
					   }
					   if(MYabs(imu_data.angle[2],target)<=2)
					   {
						   cnt1+=1;
						   if(cnt1>=3)
						   {
							   cnt1=0;
							   flag_angle=0;
							   cnt=0;
							   motor_stop();
							   j=0;
							PID_init();
							   osDelay(400);
							   flag=0;
							   angle_true=imu_data.angle[2];
							  
						   }
					   }
				  }
				   
			  }
		  }
		  if(mode==4)
		  {
			       switch(step)
				   {
					   case 1:
					   {
							  static uint16_t cnt=0;
							   static uint16_t cnt1=0;
								
							   if(cnt==0)
							   {
								angle_begin=imu_data.angle[2];
								target=angle_begin+25;
								cnt=1;
							   }
							   if(target>180)
							   {
								   target=target-360;
							   }
							   if(target<-180)
							   {   
								   target+=360;
							   }
							   turn_fixed_angle(angle_begin,target,time);
							   if(MYabs(imu_data.angle[2],target)>1.5)
							   {
							   flag_angle==1;
							   cnt=1;
							   cnt1=0;
							   }
							   if(MYabs(imu_data.angle[2],target)<=1.5)
							   {
								   cnt1+=1;
								   if(cnt1>=3)
								   {
									   cnt1=0;
									   flag_angle=0;
									   step=2;
									   cnt=0;
									   motor_stop();
									  PID_reinit();
									   osDelay(500);
									   
								   }
							   } 
						   }
					   case 2:
					   {
							  static uint16_t cnt=0;
							   static uint16_t cnt1=0;
								
							   if(cnt==0)
							   {
								angle_begin=imu_data.angle[2];
								target=angle_begin-25;
								cnt=1;
							   }
							   if(target>180)
							   {
								   target=target-360;
							   }
							   if(target<-180)
							   {   
								   target+=360;
							   }
							   turn_fixed_angle(angle_begin,target,time);
							   if(MYabs(imu_data.angle[2],target)>1.5)
							   {
							   flag_angle==1;
							   cnt=1;
							   cnt1=0;
							   }
							   if(MYabs(imu_data.angle[2],target)<=1.5)
							   {
								   cnt1+=1;
								   if(cnt1>=3)
								   {
									   cnt1=0;
									   flag_angle=0;
									   cnt=0;
									   motor_stop();
									  PID_reinit();
									   step=3;
									   osDelay(500);
									   
								   }
							   } 
						   }
				case 3:
					   {
							  static uint16_t cnt=0;
							   static uint16_t cnt1=0;
								
							   if(cnt==0)
							   {
								angle_begin=imu_data.angle[2];
								target=angle_begin-25;
								cnt=1;
							   }
							   if(target>180)
							   {
								   target=target-360;
							   }
							   if(target<-180)
							   {   
								   target+=360;
							   }
							   turn_fixed_angle(angle_begin,target,time);
							   if(MYabs(imu_data.angle[2],target)>1.5)
							   {
							   flag_angle==1;
							   cnt=1;
							   cnt1=0;
							   }
							   if(MYabs(imu_data.angle[2],target)<=1.5)
							   {
								   cnt1+=1;
								   if(cnt1>=3)
								   {
									   cnt1=0;
									   flag_angle=0;
									   cnt=0;
									   motor_stop();
									  PID_reinit();
									   step=4;
									   osDelay(500);
									   
								   }
							   } 
						   }
			case 4:
					   {
							  static uint16_t cnt=0;
							   static uint16_t cnt1=0;
								
							   if(cnt==0)
							   {
								angle_begin=imu_data.angle[2];
								target=angle_begin+25;
								cnt=1;
							   }
							   if(target>180)
							   {
								   target=target-360;
							   }
							   if(target<-180)
							   {   
								   target+=360;
							   }
							   turn_fixed_angle(angle_begin,target,time);
							   if(MYabs(imu_data.angle[2],target)>1.5)
							   {
							   flag_angle==1;
							   cnt=1;
							   cnt1=0;
							   }
							   if(MYabs(imu_data.angle[2],target)<=1.5)
							   {
								   cnt1+=1;
								   if(cnt1>=3)
								   {
									   cnt1=0;
									   flag_angle=0;
									   cnt=0;
									   motor_stop();
									   step=1;
									  PID_reinit();
									   osDelay(500);
									   
								   }
							   } 
						   }
		  }
	  }
		  osDelay(5);
	  }
  }
  }
		  cnt+=1;
		  if(cnt>100)
		  {
			  cnt=0;
		  }
	  
	 
	   
     
  }
  /* USER CODE END MOTOR_TASK */
}

/* USER CODE BEGIN Header_EXchangetask */
/**
* @brief Function implementing the exchange_task thread.;
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_EXchangetask */
void EXchangetask(void *argument)
{
  /* USER CODE BEGIN EXchangetask */
  /* Infinite loop */
  while(1)
  {
    osDelay(1);
  }
  /* USER CODE END EXchangetask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


/* USER CODE END Application */


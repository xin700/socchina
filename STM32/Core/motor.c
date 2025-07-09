#include "main.h"
#include "tim.h"
uint16_t runstate=1;
void motor_run(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4)
{
	if(sudu1<0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_SET);
	}
	if(sudu2<0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);	
	}
	if(sudu3>0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_SET);	
	}
		
	if(sudu4>0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET);	
	}

	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, sudu1);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, sudu2);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, sudu3);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, sudu4);
	
	runstate=1;
}
void motor_back(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4)
{
	if(sudu1<0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_RESET);
	}
	if(sudu2<0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);	
	}
	if(sudu3>0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_SET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);	
	}
		
	if(sudu4>0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);	
	}

	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, sudu1);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, sudu2);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, sudu3);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, sudu4);
	runstate=2;
}
void motor_turn_Left(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4)
{
	if(sudu1<0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_SET);
	}
	if(sudu2<0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);	
	}
	if(sudu3>0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_SET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);	
	}
		
	if(sudu4>0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET);	
	}

	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, sudu1);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, sudu2);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, sudu3);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, sudu4);
	runstate=3;
}
void motor_turn_Right(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4)
{	
	if(sudu1>0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_SET);
	}
	if(sudu2>0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);	
	}
	if(sudu3<0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_SET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);	
	}
		
	if(sudu4<0)
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET);	
	}

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, sudu1);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, sudu2);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, sudu3);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, sudu4);
	runstate=4;
}
void motor_right(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, sudu1);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, sudu2);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, sudu3);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, sudu4);
	runstate=6;
	
}
void motor_left(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, sudu1);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, sudu2);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, sudu3);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, sudu4);
	runstate=7;
	
}
void motor_stop()
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);	
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);	
	runstate=5;
}
void mode_change(float *v1,float *v2,float *v3,float *v4,uint16_t runstate)
{
	switch(runstate)
	{
		case 1:
		{
			 *v1=*v1;
		     *v2=*v2;
		     *v3=*v3;
		     *v4=*v4;
			break;
		}
		case 2:
		{
			*v1=*v1*-1;
			*v2=*v2*-1;
			*v3=*v3*-1;
			*v4=*v4*-1;
			break;
		}
		case 3:
		{
			*v1=*v1;
			*v2=*v2*-1;
			*v3=*v3*-1;
			*v4=*v4;
			break;
		}
		case 4:
		{
			*v1=*v1*-1;
			*v2=*v2;
			*v3=*v3 ;
			*v4=*v4*-1;
			break;
		}
		case 6:
		{
			*v1=*v1;
			*v2=*v2*-1;
			*v3=*v3;
			*v4=*v4*-1;
		}
		case 7:
		{
			*v1=*v1*-1;
			*v2=*v2;
			*v3=*v3*-1 ;
			*v4=*v4;
		}
		case 5:
		{
			
		}
		default:
		{
			break;
		}
		
	}
}
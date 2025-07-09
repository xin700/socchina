#ifndef __MOTOR_H
#define __MOTOR_H

void motor_run(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4);
void motor_back(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4);
void motor_left(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4);
void motor_right(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4);
void motor_turn_Right(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4);
void motor_turn_Left(int16_t sudu1,int16_t sudu2,int16_t sudu3,int16_t sudu4);
void motor_stop();
void mode_change(float *v1,float *v2,float *v3,float *v4,uint16_t runstate);
#endif
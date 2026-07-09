#ifndef DRV8874_H
#define DRV8874_H

#include <stdint.h>
#include <stdbool.h>

#define DRV8874_PWM_MAX           3600U
#define DRV8874_SPEED_MAX         1000
#define DRV8874_ENC_SAMPLE_HZ     20000U
/* 测速窗口：TIM2 GPIO 采样频率下，在此时间窗内累加脉冲再算 PPS */
#define DRV8874_SPEED_WINDOW_MS   50U

void DRV8874_Init(void);
void DRV8874_SetSpeed(int16_t speed);
void DRV8874_SetDuty(uint16_t duty);
void DRV8874_SetDirection(bool forward);
void DRV8874_Stop(void);
uint16_t DRV8874_ReadCurrentRaw(void);
uint16_t DRV8874_ReadCurrent_mA(void);

void DRV8874_OnEncoderTimerTick(void);
void DRV8874_ResetEncoder(void);
int32_t DRV8874_GetEncoderCount(void);
int32_t DRV8874_GetEncoderDelta(void);
float DRV8874_GetEncoderSpeed_PPS(void);
float DRV8874_GetEncoderSpeed_Instant_PPS(void);
float DRV8874_GetEncoderSpeed_RPM(void);

void DRV8874_SetTargetRpm(float target_rpm);
float DRV8874_GetTargetRpm(void);
void DRV8874_SpeedPid_Reset(void);
void DRV8874_SpeedPid_Update(void);
float DRV8874_GetPidOutput(void);

#endif

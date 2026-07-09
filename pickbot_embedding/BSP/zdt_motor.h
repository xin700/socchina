#ifndef ZDT_MOTOR_H
#define ZDT_MOTOR_H

#include <stdint.h>
#include <stdbool.h>
#include "can.h"

#define ZDT_MOTOR_DEFAULT_ADDR   1U

#define ZDT_MOTOR_MAX_RPM        5000U
#define ZDT_MOTOR_POWERUP_DELAY  500U

/* S_FLAG 堵转保护触发位（EMM 手册 bit2，读标志后自行核对） */
#define ZDT_MOTOR_FLAG_STALL_PROTECT  0x0004U

#define ZDT_STATUS_OK            0U

typedef struct
{
  bool valid;
  uint8_t addr;
  uint8_t func_code;
  uint8_t status;
  uint8_t dlc;
  uint8_t data[8];
} ZDT_Motor_Response_t;

void ZDT_Motor_Init(void);
bool ZDT_Motor_SendRawCmd(const volatile uint8_t *cmd, uint8_t len);
void ZDT_Motor_Enable(uint8_t addr, bool enable);
bool ZDT_Motor_SetSpeed(uint8_t addr, int16_t rpm, uint8_t acc);
void ZDT_Motor_Stop(uint8_t addr);
bool ZDT_Motor_WaitResponse(uint32_t timeout_ms);
bool ZDT_Motor_WaitResponseForAddr(uint8_t addr, uint32_t timeout_ms);
bool ZDT_Motor_WaitForFunc(uint8_t addr, uint8_t func_code, uint32_t timeout_ms);
float ZDT_Motor_RawToAngleDeg(int32_t raw);
uint8_t ZDT_Motor_ResponseAddr(void);
const ZDT_Motor_Response_t *ZDT_Motor_GetLastResponse(void);
bool ZDT_Motor_IsCommandOk(void);
void ZDT_Motor_OnCanRx(const CAN_RxHeaderTypeDef *header, const uint8_t *data);
int16_t ZDT_Motor_ParseVelocityRpm(void);
int32_t ZDT_Motor_ParsePosition(void);
bool ZDT_Motor_ReadVelocity(uint8_t addr, int16_t *rpm, uint32_t timeout_ms);
bool ZDT_Motor_ReadPosition(uint8_t addr, int32_t *pos, uint32_t timeout_ms);
bool ZDT_Motor_ResetClogProtection(uint8_t addr, uint32_t timeout_ms);
bool ZDT_Motor_RestoreMotor(uint8_t addr, uint32_t timeout_ms);
bool ZDT_Motor_ReadStatusFlag(uint8_t addr, uint16_t *flag, uint32_t timeout_ms);
bool ZDT_Motor_IsStallProtected(uint8_t addr, bool *stall_active, uint32_t timeout_ms);
uint16_t ZDT_Motor_ParseStatusFlag(void);

#include "emm_v5.h"

#endif

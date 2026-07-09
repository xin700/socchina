#include "zdt_motor.h"

#include "emm_v5.h"

#include "can.h"

#include "board.h"

#include "main.h"

#include <string.h>


uint16_t ZDT_MOTOR_DEFAULT_ACC=200U;
#define ZDT_TAIL       0x6BU
#define ZDT_ERR_FUNC   0x00U
#define ZDT_ERR_CODE   0xEEU
#define ZDT_FUNC_VEL   0x35U
#define ZDT_FUNC_CPOS  0x36U
#define ZDT_FUNC_CLOG_RESET  0x0EU   /* 解除堵转保护 0x0E 0x52 回包 func */
#define ZDT_FUNC_RESTORE     0x0FU   /* 恢复出厂 0x0F 0x5F 回包 func */
#define ZDT_FUNC_STATUS_FLAG 0x3AU   /* 读状态标志 S_FLAG */



static ZDT_Motor_Response_t s_last_response = {0};



bool ZDT_Motor_SendRawCmd(const volatile uint8_t *cmd, uint8_t len)

{

  g_can_app.rx_pending = false;

  s_last_response.valid = false;

  return CAN_App_SendCmd(cmd, len);

}



static void zdt_store_response(const CAN_RxHeaderTypeDef *header, const uint8_t *data)

{

  uint8_t copy_len;



  if ((header == NULL) || (data == NULL))

  {

    return;

  }



  copy_len = header->DLC;

  if (copy_len > sizeof(s_last_response.data))

  {

    copy_len = (uint8_t)sizeof(s_last_response.data);

  }



  s_last_response.valid = true;

  s_last_response.addr = (uint8_t)(header->ExtId >> 8);

  s_last_response.dlc = copy_len;

  memset(s_last_response.data, 0, sizeof(s_last_response.data));

  memcpy(s_last_response.data, data, copy_len);



  s_last_response.func_code = (copy_len >= 1U) ? data[0] : 0U;

  s_last_response.status = (copy_len >= 2U) ? data[1] : 0xFFU;

}



void ZDT_Motor_OnCanRx(const CAN_RxHeaderTypeDef *header, const uint8_t *data)

{

  zdt_store_response(header, data);

}



void ZDT_Motor_Init(void)
{
  memset(&s_last_response, 0, sizeof(s_last_response));
}



void ZDT_Motor_Enable(uint8_t addr, bool enable)

{

  Emm_V5_En_Control(addr, enable, false);

}



bool ZDT_Motor_SetSpeed(uint8_t addr, int16_t rpm, uint8_t acc)
{
  uint8_t dir = 0U;
  uint16_t vel;
  uint32_t tx_fail_before;

  if (rpm < 0)
  {
    dir = 1U;
    vel = (uint16_t)(-rpm);
  }
  else
  {
    dir = 0U;
    vel = (uint16_t)rpm;
  }

  if (vel > ZDT_MOTOR_MAX_RPM)
  {
    vel = ZDT_MOTOR_MAX_RPM;
  }

  tx_fail_before = g_can_app.tx_fail_cnt;
  Emm_V5_Vel_Control(addr, dir, vel, acc, false);
  return (g_can_app.tx_fail_cnt == tx_fail_before);
}



void ZDT_Motor_Stop(uint8_t addr)

{

  Emm_V5_Stop_Now(addr, false);

}



bool ZDT_Motor_WaitResponse(uint32_t timeout_ms)

{

  const uint32_t start = HAL_GetTick();



  while (!g_can_app.rx_pending)

  {

    CAN_App_PollRx();

    if ((HAL_GetTick() - start) >= timeout_ms)

    {

      return false;

    }

  }



  g_can_app.rx_pending = false;

  return s_last_response.valid;

}



bool ZDT_Motor_WaitResponseForAddr(uint8_t addr, uint32_t timeout_ms)

{

  const uint32_t start = HAL_GetTick();



  while ((HAL_GetTick() - start) < timeout_ms)

  {

    CAN_App_PollRx();

    if (g_can_app.rx_pending)

    {

      if (s_last_response.valid && (s_last_response.addr == addr))

      {

        g_can_app.rx_pending = false;

        return true;

      }

      g_can_app.rx_pending = false;

    }

  }



  return false;

}



static bool zdt_response_matches(uint8_t addr, uint8_t func_code)
{
  return (s_last_response.valid &&
          (s_last_response.addr == addr) &&
          (s_last_response.func_code == func_code));
}

bool ZDT_Motor_WaitForFunc(uint8_t addr, uint8_t func_code, uint32_t timeout_ms)
{
  const uint32_t start = HAL_GetTick();

  while ((HAL_GetTick() - start) < timeout_ms)
  {
    if (g_can_app.rx_pending)
    {
      g_can_app.rx_pending = false;
      if (zdt_response_matches(addr, func_code))
      {
        return true;
      }
    }

    while (CAN_App_GetRxFifoLevel() > 0U)
    {
      CAN_App_PollRx();
      if (zdt_response_matches(addr, func_code))
      {
        g_can_app.rx_pending = false;
        return true;
      }
    }
  }

  return false;
}



float ZDT_Motor_RawToAngleDeg(int32_t raw)

{

  return ((float)raw / ZDT_POS_FULL_SCALE) * 360.0f;

}



uint8_t ZDT_Motor_ResponseAddr(void)

{

  return s_last_response.valid ? s_last_response.addr : (uint8_t)(g_can_app.rx_header.ExtId >> 8);

}



const ZDT_Motor_Response_t *ZDT_Motor_GetLastResponse(void)

{

  return &s_last_response;

}



bool ZDT_Motor_IsCommandOk(void)

{

  if (!s_last_response.valid || (s_last_response.dlc < 3U))

  {

    return false;

  }



  if (s_last_response.data[s_last_response.dlc - 1U] != ZDT_TAIL)

  {

    return false;

  }



  return (s_last_response.status == ZDT_STATUS_OK);

}



static bool zdt_is_read_response_ok(uint8_t func_code, uint8_t min_dlc)

{

  if (!s_last_response.valid || (s_last_response.dlc < min_dlc))

  {

    return false;

  }



  if (s_last_response.data[s_last_response.dlc - 1U] != ZDT_TAIL)

  {

    return false;

  }



  if ((s_last_response.data[0] == ZDT_ERR_FUNC) &&

      (s_last_response.data[1] == ZDT_ERR_CODE))

  {

    return false;

  }



  return (s_last_response.data[0] == func_code);

}



int16_t ZDT_Motor_ParseVelocityRpm(void)

{

  const ZDT_Motor_Response_t *rsp = ZDT_Motor_GetLastResponse();

  int32_t raw;



  if ((rsp == NULL) || !rsp->valid || (rsp->dlc < 5U) || (rsp->data[0] != ZDT_FUNC_VEL))

  {

    return 0;

  }



  /* EMM CAN payload: [0x35][符号][vel_h][vel_l][0x6B] */

  raw = (int32_t)((uint16_t)(((uint16_t)rsp->data[2] << 8) | rsp->data[3]));

  if (rsp->data[1] != 0U)

  {

    raw = -raw;

  }



  return (int16_t)(raw );

}



int32_t ZDT_Motor_ParsePosition(void)

{

  const ZDT_Motor_Response_t *rsp = ZDT_Motor_GetLastResponse();

  int32_t raw;



  if ((rsp == NULL) || !rsp->valid || (rsp->dlc < 7U) || (rsp->data[0] != ZDT_FUNC_CPOS))

  {

    return 0;

  }



  /* EMM CAN payload: [0x36][符号][pos24..pos0][0x6B]，换算见 ZDT_Motor_RawToAngleDeg */

  raw = (int32_t)((((uint32_t)rsp->data[2] << 24) |

                   ((uint32_t)rsp->data[3] << 16) |

                   ((uint32_t)rsp->data[4] << 8) |

                   (uint32_t)rsp->data[5]));

  if (rsp->data[1] != 0U)

  {

    raw = -raw;

  }



  return raw;

}



bool ZDT_Motor_ReadVelocity(uint8_t addr, int16_t *rpm, uint32_t timeout_ms)

{

  Emm_V5_Read_Sys_Params(addr, S_VEL);



  if (!ZDT_Motor_WaitForFunc(addr, ZDT_FUNC_VEL, timeout_ms))

  {

    return false;

  }



  if (rpm != NULL)

  {

    *rpm = ZDT_Motor_ParseVelocityRpm();

  }



  return zdt_is_read_response_ok(ZDT_FUNC_VEL, 5U);

}



bool ZDT_Motor_ReadPosition(uint8_t addr, int32_t *pos, uint32_t timeout_ms)

{

  Emm_V5_Read_Sys_Params(addr, S_CPOS);



  if (!ZDT_Motor_WaitForFunc(addr, ZDT_FUNC_CPOS, timeout_ms))

  {

    return false;

  }



  if (pos != NULL)

  {

    *pos = ZDT_Motor_ParsePosition();

  }



  return zdt_is_read_response_ok(ZDT_FUNC_CPOS, 7U);

}

static void zdt_motor_can_rx_begin(void)
{
  CAN_App_SetRxIrqEnabled(false);
  CAN_App_DrainRxFifo();
  g_can_app.rx_pending = false;
}

static void zdt_motor_can_rx_end(void)
{
  CAN_App_DrainRxFifo();
  g_can_app.rx_pending = false;
  CAN_App_SetRxIrqEnabled(true);
}

static bool zdt_motor_wait_cmd_ok(uint8_t addr, uint8_t func_code, uint32_t timeout_ms)
{
  if (!ZDT_Motor_WaitForFunc(addr, func_code, timeout_ms))
  {
    return false;
  }

  return ZDT_Motor_IsCommandOk();
}

bool ZDT_Motor_ResetClogProtection(uint8_t addr, uint32_t timeout_ms)
{
  bool ok;

  zdt_motor_can_rx_begin();
  Emm_V5_Reset_Clog_Pro(addr);
  ok = zdt_motor_wait_cmd_ok(addr, ZDT_FUNC_CLOG_RESET, timeout_ms);
  zdt_motor_can_rx_end();
  return ok;
}

bool ZDT_Motor_RestoreMotor(uint8_t addr, uint32_t timeout_ms)
{
  bool ok;

  zdt_motor_can_rx_begin();
  Emm_V5_Restore_Motor(addr);
  ok = zdt_motor_wait_cmd_ok(addr, ZDT_FUNC_RESTORE, timeout_ms);
  zdt_motor_can_rx_end();
  return ok;
}

uint16_t ZDT_Motor_ParseStatusFlag(void)
{
  const ZDT_Motor_Response_t *rsp = ZDT_Motor_GetLastResponse();

  if ((rsp == NULL) || !rsp->valid || (rsp->dlc < 5U) ||
      (rsp->data[0] != ZDT_FUNC_STATUS_FLAG))
  {
    return 0U;
  }

  /* EMM CAN: [0x3A][保留/符号][flag_h][flag_l][0x6B] */
  return (uint16_t)(((uint16_t)rsp->data[2] << 8) | rsp->data[3]);
}

bool ZDT_Motor_ReadStatusFlag(uint8_t addr, uint16_t *flag, uint32_t timeout_ms)
{
  bool ok;

  zdt_motor_can_rx_begin();
  Emm_V5_Read_Sys_Params(addr, S_FLAG);

  if (!ZDT_Motor_WaitForFunc(addr, ZDT_FUNC_STATUS_FLAG, timeout_ms))
  {
    zdt_motor_can_rx_end();
    return false;
  }

  ok = zdt_is_read_response_ok(ZDT_FUNC_STATUS_FLAG, 5U);
  if (ok && (flag != NULL))
  {
    *flag = ZDT_Motor_ParseStatusFlag();
  }

  zdt_motor_can_rx_end();
  return ok;
}

bool ZDT_Motor_IsStallProtected(uint8_t addr, bool *stall_active, uint32_t timeout_ms)
{
  uint16_t flag = 0U;

  if (stall_active == NULL)
  {
    return false;
  }

  if (!ZDT_Motor_ReadStatusFlag(addr, &flag, timeout_ms))
  {
    return false;
  }

  *stall_active = ((flag & ZDT_MOTOR_FLAG_STALL_PROTECT) != 0U);
  return true;
}


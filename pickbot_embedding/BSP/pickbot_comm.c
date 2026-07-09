#include "pickbot_comm.h"

#include "board.h"
#include "pickbot.h"
#include "drv8874.h"
#include "usart.h"
#include "tim.h"

#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

extern int16_t vel;
extern int16_t velright;

typedef enum
{
  RX_WAIT_SOF0 = 0,
  RX_WAIT_SOF1,
  RX_WAIT_TYPE,
  RX_WAIT_LEN,
  RX_WAIT_PAYLOAD,
  RX_WAIT_CRC_L,
  RX_WAIT_CRC_H
} PickbotComm_RxState_t;

static volatile uint8_t s_rx_byte;
static PickbotComm_RxState_t s_rx_state = RX_WAIT_SOF0;
static uint8_t s_rx_type;
static uint8_t s_rx_len;
static uint8_t s_rx_payload[PICKBOT_COMM_CMD_BYTES];
static uint8_t s_rx_index;
static uint16_t s_rx_crc_expect;

PickbotComm_Cmd_t s_cmd = {0};
static volatile uint32_t s_rx_frame_count;
static volatile uint32_t s_rx_crc_error_count;
static volatile uint32_t s_tx_frame_count;

static uint8_t s_tx_buf[2U + 1U + 1U + PICKBOT_COMM_TELEM_BYTES + 2U];
static volatile uint8_t s_tx_busy;

uint8_t s_nav_active;
float s_nav_target_x_m;
float s_nav_target_y_m;
float s_nav_drive_speed_mps;

static volatile uint32_t s_last_rx_tick;

static void PickbotComm_OnRxByte(uint8_t byte);

static uint16_t pickbot_comm_crc16(const uint8_t *data, uint16_t len)
{
  uint16_t crc = 0xFFFFU;
  uint16_t i;
  uint8_t b;

  for (i = 0U; i < len; i++)
  {
    crc ^= (uint16_t)data[i] << 8;
    for (b = 0U; b < 8U; b++)
    {
      if ((crc & 0x8000U) != 0U)
      {
        crc = (uint16_t)((crc << 1) ^ 0x1021U);
      }
      else
      {
        crc <<= 1;
      }
    }
  }

  return crc;
}

static void pickbot_comm_float_to_bytes(float value, uint8_t *out)
{
  union
  {
    float f;
    uint8_t b[4];
  } u;

  u.f = value;
  out[0] = u.b[0];
  out[1] = u.b[1];
  out[2] = u.b[2];
  out[3] = u.b[3];
}

static float pickbot_comm_bytes_to_float(const uint8_t *in)
{
  union
  {
    float f;
    uint8_t b[4];
  } u;

  u.b[0] = in[0];
  u.b[1] = in[1];
  u.b[2] = in[2];
  u.b[3] = in[3];
  return u.f;
}

static float pickbot_comm_sign_from_dir(float dir_flag)
{
  return (dir_flag >= 0.5f) ? 1.0f : -1.0f;
}

static int16_t pickbot_comm_mps_to_rpm(float v_mps)
{
  float wheel_circ;
  float rpm_f;

  wheel_circ = 2.0f * (float)M_PI * PICKBOT_WHEEL_RADIUS_M;
  if (wheel_circ <= 0.0f)
  {
    return 0;
  }

  rpm_f = (v_mps / wheel_circ) * 60.0f;
  if (rpm_f > 32767.0f)
  {
    return 32767;
  }
  if (rpm_f < -32768.0f)
  {
    return -32768;
  }

  return (int16_t)rpm_f;
}

static void pickbot_comm_set_wheel_speed_mps(float v_mps, float omega_rps)
{
  float v_left_mps;
  float v_right_mps;
  float half_track = PICKBOT_TRACK_WIDTH_M * 0.5f;

  v_left_mps = v_mps - omega_rps * half_track;
  v_right_mps = v_mps + omega_rps * half_track;

  vel = pickbot_comm_mps_to_rpm(v_left_mps);
  velright = pickbot_comm_mps_to_rpm(v_right_mps);
}

static float pickbot_comm_wrap_pi(float rad)
{
  while (rad > (float)M_PI)
  {
    rad -= 2.0f * (float)M_PI;
  }
  while (rad < -(float)M_PI)
  {
    rad += 2.0f * (float)M_PI;
  }
  return rad;
}

static void pickbot_comm_apply_velocity_cmd(const PickbotComm_Cmd_t *cmd)
{
  float v_mps;
  float omega_rps;

  v_mps = cmd->lin_speed_mps * pickbot_comm_sign_from_dir(cmd->lin_dir)*8;
  omega_rps = cmd->ang_speed_rps * pickbot_comm_sign_from_dir(cmd->ang_dir)*4;
  pickbot_comm_set_wheel_speed_mps(v_mps, omega_rps);
}

static void pickbot_comm_apply_goto_cmd(const PickbotComm_Cmd_t *cmd,
                                        float odom_x_m,
                                        float odom_y_m,
                                        float yaw_rad)
{
  float dx;
  float dy;
  float dist_m;
  float bearing_rad;
  float yaw_err_rad;
  float drive_speed_mps;
  float omega_rps;

  dx = cmd->target_x_m - odom_x_m;
  dy = cmd->target_y_m - odom_y_m;
  dist_m = sqrtf((dx * dx) + (dy * dy));

  if (dist_m < 0.05f)
  {
    s_nav_active = 0U;
    vel = 0;
    velright = 0;
    return;
  }

  bearing_rad = atan2f(dy, dx);
  yaw_err_rad = pickbot_comm_wrap_pi(bearing_rad - yaw_rad);

  drive_speed_mps = cmd->lin_speed_mps;
  if (drive_speed_mps <= 0.0f)
  {
    drive_speed_mps = 0.20f;
  }

  if (fabsf(yaw_err_rad) > 0.08f)
  {
    omega_rps = cmd->ang_speed_rps;
    if (omega_rps <= 0.0f)
    {
      omega_rps = 0.80f;
    }
    omega_rps *= (yaw_err_rad >= 0.0f) ? 1.0f : -1.0f;
    pickbot_comm_set_wheel_speed_mps(0.0f, omega_rps);
    return;
  }

  pickbot_comm_set_wheel_speed_mps(drive_speed_mps, 0.0f);
}

static void pickbot_comm_store_cmd(const uint8_t *payload)
{
  s_cmd.lin_speed_mps = pickbot_comm_bytes_to_float(&payload[0]);
  s_cmd.lin_dir = pickbot_comm_bytes_to_float(&payload[4]);
  s_cmd.ang_speed_rps = pickbot_comm_bytes_to_float(&payload[8]);
  s_cmd.ang_dir = pickbot_comm_bytes_to_float(&payload[12]);
  s_cmd.target_x_m = pickbot_comm_bytes_to_float(&payload[16]);
  s_cmd.target_y_m = pickbot_comm_bytes_to_float(&payload[20]);
  s_cmd.control_mode = pickbot_comm_bytes_to_float(&payload[24]);
  s_cmd.drv8874_speed = pickbot_comm_bytes_to_float(&payload[28]);
  s_cmd.fresh = true;
  s_last_rx_tick = HAL_GetTick();
  s_rx_frame_count++;
}

static void pickbot_comm_rx_reset(void)
{
  s_rx_state = RX_WAIT_SOF0;
  s_rx_index = 0U;
}

static void pickbot_comm_start_rx_it(void)
{
  if (HAL_UART_Receive_IT(&huart1, (uint8_t *)&s_rx_byte, 1U) != HAL_OK)
  {
    pickbot_comm_rx_reset();
  }
}

static void pickbot_comm_build_telemetry(const volatile Pickbot_Status_t *st,
                                         uint8_t *payload)
{
  float left_mm;
  float right_mm;

  pickbot_comm_float_to_bytes(st->odom_x_m, &payload[0]);
  pickbot_comm_float_to_bytes(st->odom_y_m, &payload[4]);
  pickbot_comm_float_to_bytes(st->jy901s_yaw_deg, &payload[8]);

  left_mm = st->us100_uart2_ok ? (float)st->us100_uart2_distance_mm : -1.0f;
  right_mm = st->us100_uart3_ok ? (float)st->us100_uart3_distance_mm : -1.0f;
  pickbot_comm_float_to_bytes(left_mm, &payload[12]);
  pickbot_comm_float_to_bytes(right_mm, &payload[16]);
}

void PickbotComm_Init(void)
{
  memset((void *)&s_cmd, 0, sizeof(s_cmd));
  pickbot_comm_rx_reset();
  s_tx_busy = 0U;
  s_nav_active = 0U;
  s_last_rx_tick = 0U;
  pickbot_comm_start_rx_it();
  (void)HAL_TIM_Base_Start_IT(&htim1);
}

void PickbotComm_OnUartRxCplt(void)
{
  PickbotComm_OnRxByte(s_rx_byte);
}

void PickbotComm_OnUartError(void)
{
  pickbot_comm_rx_reset();
  pickbot_comm_start_rx_it();
}

static void PickbotComm_OnRxByte(uint8_t byte)
{
  uint16_t crc_calc;
  uint8_t crc_buf[2U + PICKBOT_COMM_CMD_BYTES];

  switch (s_rx_state)
  {
    case RX_WAIT_SOF0:
      if (byte == PICKBOT_COMM_SOF0)
      {
        s_rx_state = RX_WAIT_SOF1;
      }
      break;

    case RX_WAIT_SOF1:
      if (byte == PICKBOT_COMM_SOF1)
      {
        s_rx_state = RX_WAIT_TYPE;
      }
      else if (byte != PICKBOT_COMM_SOF0)
      {
        s_rx_state = RX_WAIT_SOF0;
      }
      break;

    case RX_WAIT_TYPE:
      s_rx_type = byte;
      s_rx_state = RX_WAIT_LEN;
      break;

    case RX_WAIT_LEN:
      s_rx_len = byte;
      s_rx_index = 0U;
      if ((s_rx_type == PICKBOT_COMM_TYPE_CMD) &&
          (s_rx_len == PICKBOT_COMM_CMD_BYTES))
      {
        s_rx_state = RX_WAIT_PAYLOAD;
      }
      else
      {
        s_rx_crc_error_count++;
        pickbot_comm_rx_reset();
      }
      break;

    case RX_WAIT_PAYLOAD:
      s_rx_payload[s_rx_index++] = byte;
      if (s_rx_index >= s_rx_len)
      {
        s_rx_state = RX_WAIT_CRC_L;
      }
      break;

    case RX_WAIT_CRC_L:
      s_rx_crc_expect = (uint16_t)byte;
      s_rx_state = RX_WAIT_CRC_H;
      break;

    case RX_WAIT_CRC_H:
      s_rx_crc_expect |= (uint16_t)byte << 8;
      crc_buf[0] = s_rx_type;
      crc_buf[1] = s_rx_len;
      memcpy(&crc_buf[2], s_rx_payload, s_rx_len);
      crc_calc = pickbot_comm_crc16(crc_buf, (uint16_t)(2U + s_rx_len));
      if (crc_calc == s_rx_crc_expect)
      {
        pickbot_comm_store_cmd(s_rx_payload);
      }
      else
      {
        s_rx_crc_error_count++;
      }
      pickbot_comm_rx_reset();
      break;

    default:
      pickbot_comm_rx_reset();
      break;
  }

  pickbot_comm_start_rx_it();
}

void PickbotComm_OnTim1Tick(void)
{
  const volatile Pickbot_Status_t *st;
  uint8_t payload[PICKBOT_COMM_TELEM_BYTES];
  uint8_t crc_buf[2U + PICKBOT_COMM_TELEM_BYTES];
  uint16_t crc;
  uint16_t i;

  if (s_tx_busy != 0U)
  {
    return;
  }

  st = Pickbot_GetStatusPtr();
  pickbot_comm_build_telemetry(st, payload);

  s_tx_buf[0] = PICKBOT_COMM_SOF0;
  s_tx_buf[1] = PICKBOT_COMM_SOF1;
  s_tx_buf[2] = PICKBOT_COMM_TYPE_TELEM;
  s_tx_buf[3] = PICKBOT_COMM_TELEM_BYTES;
  memcpy(&s_tx_buf[4], payload, PICKBOT_COMM_TELEM_BYTES);

  crc_buf[0] = PICKBOT_COMM_TYPE_TELEM;
  crc_buf[1] = PICKBOT_COMM_TELEM_BYTES;
  memcpy(&crc_buf[2], payload, PICKBOT_COMM_TELEM_BYTES);
  crc = pickbot_comm_crc16(crc_buf, (uint16_t)(2U + PICKBOT_COMM_TELEM_BYTES));

  i = (uint16_t)(4U + PICKBOT_COMM_TELEM_BYTES);
  s_tx_buf[i++] = (uint8_t)(crc & 0xFFU);
  s_tx_buf[i++] = (uint8_t)((crc >> 8) & 0xFFU);

  s_tx_busy = 1U;
  if (HAL_UART_Transmit_IT(&huart1, s_tx_buf, i) != HAL_OK)
  {
    s_tx_busy = 0U;
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart1)
  {
    s_tx_busy = 0U;
    s_tx_frame_count++;
  }
}

static void pickbot_comm_clamp_drv8874(float speed_f)
{
  int16_t drv_speed = (int16_t)speed_f;

  if (drv_speed > DRV8874_SPEED_MAX)
  {
    drv_speed = DRV8874_SPEED_MAX;
  }
  else if (drv_speed < -DRV8874_SPEED_MAX)
  {
    drv_speed = -DRV8874_SPEED_MAX;
  }
  Pickbot_Drv8874_SetSpeed(drv_speed);
}

static void pickbot_comm_emergency_stop(void)
{
  vel = 0;
  velright = 0;
  s_nav_active = 0U;
  Pickbot_Drv8874_SetSpeed(0);
  s_cmd.fresh = false;
}

static bool pickbot_comm_link_ok(void)
{
  uint32_t now = HAL_GetTick();

  if (s_last_rx_tick == 0U)
  {
    return false;
  }

  return (now - s_last_rx_tick) <= PICKBOT_COMM_LINK_TIMEOUT_MS;
}

void PickbotComm_ApplyMotion(void)
{
  PickbotComm_Cmd_t cmd_local;
  const volatile Pickbot_Status_t *st;

  if (!pickbot_comm_link_ok())
  {
    pickbot_comm_emergency_stop();
    return;
  }

  if (s_cmd.fresh)
  {
    cmd_local = s_cmd;
    s_cmd.fresh = false;

    pickbot_comm_clamp_drv8874(cmd_local.drv8874_speed);

    if (cmd_local.control_mode >= 1.5f)
    {
      s_nav_active = 1U;
      s_nav_target_x_m = cmd_local.target_x_m;
      s_nav_target_y_m = cmd_local.target_y_m;
      s_nav_drive_speed_mps = cmd_local.lin_speed_mps;
      s_cmd.ang_speed_rps = cmd_local.ang_speed_rps;
    }
    else
    {
      s_nav_active = 0U;
      pickbot_comm_apply_velocity_cmd(&cmd_local);
    }
  }

  if (s_nav_active == 0U)
  {
    return;
  }

  cmd_local.lin_speed_mps = s_nav_drive_speed_mps;
  cmd_local.lin_dir = 1.0f;
  cmd_local.ang_speed_rps = s_cmd.ang_speed_rps;
  cmd_local.ang_dir = 1.0f;
  cmd_local.target_x_m = s_nav_target_x_m;
  cmd_local.target_y_m = s_nav_target_y_m;

  st = Pickbot_GetStatusPtr();
  pickbot_comm_apply_goto_cmd(&cmd_local,
                              st->odom_x_m,
                              st->odom_y_m,
                              st->odom_yaw_rad);
}

bool PickbotComm_IsLinkOk(void)
{
  return pickbot_comm_link_ok();
}

const PickbotComm_Cmd_t *PickbotComm_GetCmd(void)
{
  return &s_cmd;
}

uint32_t PickbotComm_GetRxFrameCount(void)
{
  return s_rx_frame_count;
}

uint32_t PickbotComm_GetRxCrcErrorCount(void)
{
  return s_rx_crc_error_count;
}

uint32_t PickbotComm_GetTxFrameCount(void)
{
  return s_tx_frame_count;
}

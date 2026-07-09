#include "pickbot.h"
#include "zdt_motor.h"
#include "zdt_odometry.h"
#include "drv8874.h"
#include "us100.h"
#include "jy901s.h"
#include "board.h"
#include "pickbot_comm.h"
#include "can.h"
#include "cmsis_os.h"
#include "stm32f1xx_it.h"
#include "tim.h"
extern int16_t vel;
extern int16_t velright;
extern uint16_t ZDT_MOTOR_DEFAULT_ACC;
Pickbot_Status_t s_status = {0};
uint8_t s_zdt_inited = 0U;
uint8_t s_drv_inited = 0U;
int16_t speed = 0;
int16_t speed1=0;
static ZDT_WheelStatus_t *pickbot_wheel_left(void)
{
  return (ZDT_WheelStatus_t *)&s_status.zdt_left;
}

static ZDT_WheelStatus_t *pickbot_wheel_right(void)
{
  return (ZDT_WheelStatus_t *)&s_status.zdt_right;
}

static void pickbot_zdt_init_wheel(ZDT_WheelStatus_t *wheel, uint8_t id)
{
  wheel->id = id;
  wheel->target_rpm = 0;
  wheel->actual_rpm = 0;
  wheel->position_raw = 0;
  wheel->angle_deg = 0.0f;
  wheel->online = false;
}

static void pickbot_zdt_init_wheels(void)
{
  pickbot_zdt_init_wheel(pickbot_wheel_left(), ZDT_WHEEL_LEFT_ID);
  pickbot_zdt_init_wheel(pickbot_wheel_right(), ZDT_WHEEL_RIGHT_ID);
}

static int16_t pickbot_zdt_motor_rpm(uint8_t motor_id, int16_t rpm)
{
  if (motor_id == ZDT_WHEEL_RIGHT_ID)
  {
    return (int16_t)(rpm * ZDT_WHEEL_RIGHT_RPM_SIGN);
  }

  return rpm;
}

static int16_t pickbot_zdt_body_rpm(uint8_t motor_id, int16_t motor_rpm)
{
  if (motor_id == ZDT_WHEEL_RIGHT_ID)
  {
    return (int16_t)(motor_rpm * ZDT_WHEEL_RIGHT_RPM_SIGN);
  }

  return motor_rpm;
}

static bool pickbot_zdt_seed_wheel(ZDT_WheelStatus_t *wheel)
{
  int32_t pos = 0;

  if (ZDT_Motor_ReadPosition(wheel->id, &pos, ZDT_CAN_RESP_TIMEOUT))
  {
    wheel->position_raw = pos;
    wheel->angle_deg = ZDT_Motor_RawToAngleDeg(pos);
    wheel->online = true;
    return true;
  }

  wheel->online = false;
  return false;
}

static void pickbot_zdt_enable_all(void)
{
  ZDT_WheelStatus_t *left = pickbot_wheel_left();
  ZDT_WheelStatus_t *right = pickbot_wheel_right();
  int32_t left_raw = 0;
  int32_t right_raw = 0;

  /* 上电初始化：先解除堵转保护，再使能 */
  (void)ZDT_Motor_ResetClogProtection(left->id, ZDT_CAN_RESP_TIMEOUT);
  (void)ZDT_Motor_ResetClogProtection(right->id, ZDT_CAN_RESP_TIMEOUT);

  ZDT_Motor_Enable(left->id, true);
  left->online = ZDT_Motor_WaitResponseForAddr(left->id, ZDT_CAN_RESP_TIMEOUT);

  ZDT_Motor_Enable(right->id, true);
  right->online = ZDT_Motor_WaitResponseForAddr(right->id, ZDT_CAN_RESP_TIMEOUT);

  if (pickbot_zdt_seed_wheel(left))
  {
    left_raw = left->position_raw;
  }

  if (pickbot_zdt_seed_wheel(right))
  {
    right_raw = right->position_raw;
  }

  ZdtOdometry_Reset();
  ZdtOdometry_Seed(left_raw, right_raw);
}

static void pickbot_zdt_send_speed(ZDT_WheelStatus_t *wheel)
{
  uint8_t retry;

  for (retry = 0U; retry < 2U; retry++)
  {
    if (ZDT_Motor_SetSpeed(wheel->id,
                           pickbot_zdt_motor_rpm(wheel->id, wheel->target_rpm),
                           ZDT_MOTOR_DEFAULT_ACC))
    {
      break;
    }
  }

  (void)ZDT_Motor_WaitForFunc(wheel->id, 0xF6U, ZDT_CAN_F6_ACK_TIMEOUT_MS);
}

static void pickbot_zdt_read_wheel(ZDT_WheelStatus_t *wheel, int32_t *pos_raw, bool read_rpm)
{
  int16_t rpm = 0;
  int32_t pos = 0;

  if (ZDT_Motor_ReadPosition(wheel->id, &pos, ZDT_CAN_RESP_TIMEOUT))
  {
    wheel->position_raw = pos;
    wheel->angle_deg = ZDT_Motor_RawToAngleDeg(pos);
    wheel->online = true;
    if (pos_raw != NULL)
    {
      *pos_raw = pos;
    }
  }
  else
  {
    wheel->online = false;
  }

  if (read_rpm && ZDT_Motor_ReadVelocity(wheel->id, &rpm, ZDT_CAN_RESP_TIMEOUT))
  {
    wheel->actual_rpm = pickbot_zdt_body_rpm(wheel->id, rpm);
  }
}

static void pickbot_zdt_poll_wheels(ZDT_WheelStatus_t *left,
                                    ZDT_WheelStatus_t *right,
                                    int32_t *left_raw,
                                    int32_t *right_raw,
                                    bool read_left_rpm,
                                    bool read_right_rpm)
{
  CAN_App_SetRxIrqEnabled(false);
  CAN_App_DrainRxFifo();
  g_can_app.rx_pending = false;

  pickbot_zdt_send_speed(left);
  pickbot_zdt_send_speed(right);

  pickbot_zdt_read_wheel(left, left_raw, read_left_rpm);
  pickbot_zdt_read_wheel(right, right_raw, read_right_rpm);

  CAN_App_DrainRxFifo();
  g_can_app.rx_pending = false;
  CAN_App_SetRxIrqEnabled(true);
}

Pickbot_Status_t Pickbot_GetStatus(void)
{
  return s_status;
}

const volatile Pickbot_Status_t *Pickbot_GetStatusPtr(void)
{
  return &s_status;
}

void Pickbot_Task_Drv8874(void)
{
  if (s_drv_inited == 0U)
  {
    DRV8874_Init();
    s_drv_inited = 1U;
  }

  s_status.drv8874_encoder_count = DRV8874_GetEncoderCount();
  s_status.drv8874_encoder_pps = DRV8874_GetEncoderSpeed_PPS();
  s_status.drv8874_encoder_rpm = DRV8874_GetEncoderSpeed_RPM();
  s_status.drv8874_current_ma = DRV8874_ReadCurrent_mA();
  DRV8874_SetSpeed(speed);
//  s_status.drv8874_target_rpm =
//      (float)speed * DRV8874_TARGET_RPM_MAX / (float)DRV8874_SPEED_MAX;
//  DRV8874_SetTargetRpm(s_status.drv8874_target_rpm);
//  DRV8874_SpeedPid_Update();
//  s_status.drv8874_pid_output = DRV8874_GetPidOutput();
}

void Pickbot_Task_US100(void)
{
  static uint8_t s_us100_inited = 0U;
  uint16_t distance1 = 0U;
  uint16_t distance2 = 0U;
  uint32_t echo1_us = 0U;
  uint32_t echo2_us = 0U;
  US100_Result_t res1;
  US100_Result_t res2;

  if (s_us100_inited == 0U)
  {
    US100_Init();
    s_us100_inited = 1U;
  }

  res1 = US100_MeasureSensor(0U, &distance1, &echo1_us);
  osDelay(US100_SENSOR_GAP_MS);
  res2 = US100_MeasureSensor(1U, &distance2, &echo2_us);

  s_status.us100_1_state = (uint8_t)res1;
  s_status.us100_2_state = (uint8_t)res2;
  s_status.us100_1_echo_us = (uint16_t)((echo1_us > 65535U) ? 65535U : echo1_us);
  s_status.us100_2_echo_us = (uint16_t)((echo2_us > 65535U) ? 65535U : echo2_us);

  s_status.us100_uart2_ok = (res1 == US100_OK);
  s_status.us100_uart3_ok = (res2 == US100_OK);
  s_status.us100_ok = s_status.us100_uart2_ok || s_status.us100_uart3_ok;

  if (res1 == US100_OK)
  {
    s_status.us100_uart2_distance_mm = distance1;
    s_status.distance_mm = distance1;
  }

  if (res2 == US100_OK)
  {
    s_status.us100_uart3_distance_mm = distance2;
  }
}

void Pickbot_Task_JY901S(void)
{
  static uint8_t s_first_try = 1U;
  static uint32_t s_last_init_try_tick = 0U;
  JY901S_Data_t data = {0};
  uint32_t now;

  if (!JY901S_IsCalibrated())
  {
    now = HAL_GetTick();

    if ((s_last_init_try_tick == 0U) ||
        ((now - s_last_init_try_tick) >= JY901S_INIT_RETRY_MS))
    {
      if (s_first_try != 0U)
      {
        s_first_try = 0U;
        osDelay(JY901S_POWERUP_DELAY_MS);
      }

      s_last_init_try_tick = now;
      s_status.jy901s_calibrated = JY901S_Init();
    }

    s_status.jy901s_ok = false;
    return;
  }

  s_status.jy901s_calibrated = true;
  s_status.jy901s_ok = JY901S_Read(&data);
  if (s_status.jy901s_ok)
  {
    s_status.jy901s_ax_g = data.ax_g;
    s_status.jy901s_ay_g = data.ay_g;
    s_status.jy901s_az_g = data.az_g;
    s_status.jy901s_gx_dps = data.gx_dps;
    s_status.jy901s_gy_dps = data.gy_dps;
    s_status.jy901s_gz_dps = data.gz_dps;
    s_status.jy901s_roll_raw_deg = data.roll_raw_deg;
    s_status.jy901s_pitch_raw_deg = data.pitch_raw_deg;
    s_status.jy901s_yaw_raw_deg = data.yaw_raw_deg;
    s_status.jy901s_roll_offset_deg = data.roll_offset_deg;
    s_status.jy901s_pitch_offset_deg = data.pitch_offset_deg;
    s_status.jy901s_yaw_offset_deg = data.yaw_offset_deg;
    s_status.jy901s_roll_deg = data.roll_deg;
    s_status.jy901s_pitch_deg = data.pitch_deg;
    s_status.jy901s_yaw_deg = data.yaw_deg;
    s_status.jy901s_yaw_fused_deg = data.yaw_fused_deg;
    s_status.jy901s_gz_bias_dps = data.gz_bias_dps;
    s_status.jy901s_yaw_gyro_deg = data.yaw_gyro_deg;
    s_status.jy901s_yaw_gyro_wrap_deg = data.yaw_gyro_wrap_deg;
    s_status.jy901s_yaw_diff_deg = data.yaw_diff_deg;
    s_status.jy901s_temp_c = data.temp_c;

    if (JY901S_IsCalibrated())
    {
      s_status.odom_yaw_rad = JY901S_GetYawRad();
    }
  }
}

void Pickbot_Task_ZDT(void)
{
  static uint32_t s_last_tick = 0U;
  static uint8_t s_rpm_phase = 0U;
  ZdtOdometryPose_t pose;
  ZDT_WheelStatus_t *left = pickbot_wheel_left();
  ZDT_WheelStatus_t *right = pickbot_wheel_right();
  int32_t left_raw = left->position_raw;
  int32_t right_raw = right->position_raw;
  uint32_t now = HAL_GetTick();
  uint32_t dt_ms = (s_last_tick == 0U) ? 20U : (now - s_last_tick);

  s_last_tick = now;

  if (s_zdt_inited == 0U)
  {
    osDelay(ZDT_MOTOR_POWERUP_DELAY);
    ZDT_Motor_Init();
    pickbot_zdt_init_wheels();
    ZdtOdometry_Init();
    pickbot_zdt_enable_all();
    s_status.odom_x_m = 0.0f;
    s_status.odom_y_m = 0.0f;
    s_status.odom_yaw_rad = 0.0f;
    s_zdt_inited = 1U;
    s_status.zdt_enabled = true;
  }

  PickbotComm_ApplyMotion();

  left->target_rpm = vel;
  right->target_rpm = velright;

  pickbot_zdt_poll_wheels(left,
                          right,
                          &left_raw,
                          &right_raw,
                          (s_rpm_phase == 0U),
                          (s_rpm_phase == 1U));
  s_rpm_phase ^= 1U;

  if (JY901S_IsCalibrated())
  {
    ZdtOdometry_UpdateWithYaw(left_raw,
                              right_raw,
                              JY901S_GetYawRad(),
                              true,
                              dt_ms);
  }
  else
  {
    ZdtOdometry_Update(left_raw, right_raw, dt_ms);
  }
  pose = ZdtOdometry_GetPose();
  s_status.odom_x_m = pose.x_m;
  s_status.odom_y_m = pose.y_m;
  if (!JY901S_IsCalibrated())
  {
    s_status.odom_yaw_rad = pose.yaw_rad;
  }
}

void Pickbot_ZDT_SetWheelSpeed(int16_t left_rpm, int16_t right_rpm)
{
  pickbot_wheel_left()->target_rpm = left_rpm;
  pickbot_wheel_right()->target_rpm = right_rpm;
  ZDT_Motor_SetSpeed(ZDT_WHEEL_LEFT_ID,
                     pickbot_zdt_motor_rpm(ZDT_WHEEL_LEFT_ID, left_rpm),
                     ZDT_MOTOR_DEFAULT_ACC);
  ZDT_Motor_SetSpeed(ZDT_WHEEL_RIGHT_ID,
                     pickbot_zdt_motor_rpm(ZDT_WHEEL_RIGHT_ID, right_rpm),
                     ZDT_MOTOR_DEFAULT_ACC);
}

void Pickbot_ZDT_SetTargetRpm(int16_t rpm)
{
  Pickbot_ZDT_SetWheelSpeed(rpm, rpm);
}

void Pickbot_Odom_Reset(void)
{
  if (JY901S_IsCalibrated())
  {
    float yaw_rad = JY901S_GetYawRad();

    ZdtOdometry_ResetWithYaw(yaw_rad);
    s_status.odom_yaw_rad = yaw_rad;
  }
  else
  {
    ZdtOdometry_Reset();
    s_status.odom_yaw_rad = 0.0f;
  }

  s_status.odom_x_m = 0.0f;
  s_status.odom_y_m = 0.0f;
}

static ZDT_WheelStatus_t *pickbot_zdt_wheel_by_id(uint8_t motor_id)
{
  if (motor_id == ZDT_WHEEL_LEFT_ID)
  {
    return pickbot_wheel_left();
  }

  if (motor_id == ZDT_WHEEL_RIGHT_ID)
  {
    return pickbot_wheel_right();
  }

  return NULL;
}

bool Pickbot_ZDT_ResetClogProtection(uint8_t motor_id)
{
  ZDT_WheelStatus_t *wheel = pickbot_zdt_wheel_by_id(motor_id);

  if (wheel == NULL)
  {
    return false;
  }

  if (ZDT_Motor_ResetClogProtection(motor_id, ZDT_CAN_RESP_TIMEOUT))
  {
    wheel->online = true;
    return true;
  }

  wheel->online = false;
  return false;
}

bool Pickbot_ZDT_ResetClogProtectionAll(void)
{
  bool ok_left = Pickbot_ZDT_ResetClogProtection(ZDT_WHEEL_LEFT_ID);
  bool ok_right = Pickbot_ZDT_ResetClogProtection(ZDT_WHEEL_RIGHT_ID);

  return (ok_left && ok_right);
}

bool Pickbot_ZDT_RestoreMotor(uint8_t motor_id)
{
  ZDT_WheelStatus_t *wheel = pickbot_zdt_wheel_by_id(motor_id);

  if (wheel == NULL)
  {
    return false;
  }

  if (ZDT_Motor_RestoreMotor(motor_id, ZDT_CAN_RESP_TIMEOUT))
  {
    wheel->online = true;
    return true;
  }

  wheel->online = false;
  return false;
}

bool Pickbot_ZDT_ReadWheelStatusFlag(uint8_t motor_id, uint16_t *flag)
{
  ZDT_WheelStatus_t *wheel = pickbot_zdt_wheel_by_id(motor_id);

  if (wheel == NULL)
  {
    return false;
  }

  if (ZDT_Motor_ReadStatusFlag(motor_id, flag, ZDT_CAN_RESP_TIMEOUT))
  {
    wheel->online = true;
    return true;
  }

  wheel->online = false;
  return false;
}

bool Pickbot_ZDT_IsWheelStallProtected(uint8_t motor_id, bool *stall_active)
{
  ZDT_WheelStatus_t *wheel = pickbot_zdt_wheel_by_id(motor_id);

  if (wheel == NULL)
  {
    return false;
  }

  if (ZDT_Motor_IsStallProtected(motor_id, stall_active, ZDT_CAN_RESP_TIMEOUT))
  {
    wheel->online = true;
    return true;
  }

  wheel->online = false;
  return false;
}

void Pickbot_Drv8874_SetSpeed(int16_t speed_cmd)
{
  speed = speed_cmd;
}

void Pickbot_Drv8874_SetTargetRpm(float target_rpm)
{
  DRV8874_SetTargetRpm(target_rpm);
}

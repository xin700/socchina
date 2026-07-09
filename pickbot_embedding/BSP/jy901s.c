#include "jy901s.h"

#include "board.h"
#include "i2c.h"
#include "cmsis_os.h"

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define JY901S_REG_SAVE     0x00U
#define JY901S_REG_ALG      0x24U   /* 0=9轴 1=6轴 */
#define JY901S_REG_AX       0x34U
#define JY901S_REG_ROLL     0x3DU
#define JY901S_REG_TEMP     0x40U
#define JY901S_READ_LEN     26U

static float s_offset_roll_deg = 0.0f;
static float s_offset_pitch_deg = 0.0f;
static float s_offset_yaw_deg = 0.0f;
static float s_yaw_rad = 0.0f;
static float s_yaw_gyro_pure_deg = 0.0f;
static float s_yaw_gyro_deg = 0.0f;
static float s_gz_bias_dps = 0.0f;
static uint32_t s_last_gyro_tick = 0U;
static uint8_t s_stationary_count = 0U;
static uint8_t s_calibrated = 0U;

static int16_t jy901s_i16_le(const uint8_t *buf)
{
  return (int16_t)((uint16_t)buf[0] | ((uint16_t)buf[1] << 8));
}

static float jy901s_raw_to_deg(int16_t raw)
{
  return (float)raw / 32768.0f * 180.0f;
}

static float jy901s_wrap_deg(float deg)
{
  while (deg > 180.0f)
  {
    deg -= 360.0f;
  }
  while (deg < -180.0f)
  {
    deg += 360.0f;
  }
  return deg;
}

static float jy901s_sub_deg(float angle_deg, float offset_deg)
{
  return jy901s_wrap_deg(angle_deg - offset_deg);
}

static float jy901s_gz_corrected(float gz_dps)
{
  return (JY901S_GZ_SIGN * JY901S_GZ_SCALE * gz_dps) - s_gz_bias_dps;
}

static void jy901s_reset_gyro_yaw(uint32_t tick_ms)
{
  s_yaw_gyro_pure_deg = 0.0f;
  s_yaw_gyro_deg = 0.0f;
  s_last_gyro_tick = tick_ms;
  s_stationary_count = 0U;
}

static float jy901s_fusion_trust_weight(float fused_yaw_deg)
{
  float abs_fused = fabsf(fused_yaw_deg);

  if (abs_fused >= JY901S_FUSION_TRUST_ANGLE_DEG)
  {
    return 0.0f;
  }

  return 1.0f - (abs_fused / JY901S_FUSION_TRUST_ANGLE_DEG);
}

/*
 * 主 Yaw：gz 积分为主；模块融合(raw-offset) 在 |融合角| 小处加权拉回，抑制长期漂移。
 * 大角度处融合不可信，trust→0，退化为纯积分。
 */
static void jy901s_apply_fusion_correction(float fused_yaw_deg)
{
  float gyro_wrap_deg;
  float err_deg;
  float trust;
  float kp;
  float correction_deg;

  gyro_wrap_deg = jy901s_wrap_deg(s_yaw_gyro_pure_deg);
  err_deg = jy901s_sub_deg(fused_yaw_deg, gyro_wrap_deg);
  trust = jy901s_fusion_trust_weight(fused_yaw_deg);

  if (trust <= 0.0f)
  {
    s_yaw_gyro_deg = s_yaw_gyro_pure_deg;
    return;
  }

  if (fabsf(err_deg) > JY901S_FUSION_MAX_ERR_DEG)
  {
    s_yaw_gyro_deg = s_yaw_gyro_pure_deg;
    return;
  }

  kp = (s_stationary_count >= JY901S_STATIONARY_SAMPLES)
           ? JY901S_FUSION_BLEND_KP_STATIC
           : JY901S_FUSION_BLEND_KP;
  correction_deg = trust * kp * err_deg;
  s_yaw_gyro_deg = s_yaw_gyro_pure_deg + correction_deg;
}

static bool jy901s_write_reg16(uint8_t reg, int16_t value)
{
  uint8_t buf[2];

  buf[0] = (uint8_t)(value & 0xFF);
  buf[1] = (uint8_t)((uint16_t)((uint16_t)value >> 8) & 0xFFU);
  return (HAL_I2C_Mem_Write(&hi2c1,
                            (uint16_t)(JY901S_I2C_ADDR_7BIT << 1),
                            reg,
                            I2C_MEMADD_SIZE_8BIT,
                            buf,
                            2U,
                            JY901S_I2C_TIMEOUT_MS) == HAL_OK);
}

#if JY901S_USE_6AXIS_FUSION
static bool jy901s_set_6axis_fusion(void)
{
  if (!jy901s_write_reg16(JY901S_REG_ALG, 1))
  {
    return false;
  }

  osDelay(50U);

  if (!jy901s_write_reg16(JY901S_REG_SAVE, 0))
  {
    return false;
  }

  osDelay(100U);
  return true;
}
#endif

static bool jy901s_i2c_read_buf(uint8_t *buf, uint16_t len)
{
  if ((buf == NULL) || (len == 0U))
  {
    return false;
  }

  return (HAL_I2C_Mem_Read(&hi2c1,
                           (uint16_t)(JY901S_I2C_ADDR_7BIT << 1),
                           JY901S_REG_AX,
                           I2C_MEMADD_SIZE_8BIT,
                           buf,
                           len,
                           JY901S_I2C_TIMEOUT_MS) == HAL_OK);
}

/* 纯 gz 积分（减零偏），结果写入 s_yaw_gyro_pure_deg */
static void jy901s_update_gyro_yaw(float gz_dps, uint32_t tick_ms)
{
  uint32_t dt_ms;
  float dt_s;
  float gz_meas;
  float gz_corr;

  if (s_last_gyro_tick == 0U)
  {
    s_last_gyro_tick = tick_ms;
    return;
  }

  dt_ms = tick_ms - s_last_gyro_tick;
  s_last_gyro_tick = tick_ms;

  if (dt_ms == 0U)
  {
    return;
  }

  if (dt_ms > 200U)
  {
    dt_ms = 200U;
  }

  dt_s = (float)dt_ms / 1000.0f;
  gz_meas = JY901S_GZ_SIGN * JY901S_GZ_SCALE * gz_dps;

  if (fabsf(gz_meas) < JY901S_GZ_STATIONARY_DPS)
  {
    if (s_stationary_count < 255U)
    {
      s_stationary_count++;
    }
  }
  else
  {
    s_stationary_count = 0U;
  }

  if (s_stationary_count >= JY901S_STATIONARY_SAMPLES)
  {
    s_gz_bias_dps += JY901S_GZ_BIAS_LPF * (gz_meas - s_gz_bias_dps);
  }

  gz_corr = gz_meas - s_gz_bias_dps;
  s_yaw_gyro_pure_deg += gz_corr * dt_s;
}

static void jy901s_fill_yaw_compare(JY901S_Data_t *data)
{
  float gyro_wrap_deg;

  if (data == NULL)
  {
    return;
  }

  data->yaw_gyro_deg = s_yaw_gyro_pure_deg;
  gyro_wrap_deg = jy901s_wrap_deg(s_yaw_gyro_pure_deg);
  data->yaw_gyro_wrap_deg = gyro_wrap_deg;
  data->yaw_diff_deg = jy901s_sub_deg(data->yaw_fused_deg, gyro_wrap_deg);
}

static bool jy901s_read_angles_gz(float *roll_deg,
                                  float *pitch_deg,
                                  float *yaw_deg,
                                  float *gz_dps)
{
  uint8_t buf[JY901S_READ_LEN] = {0};
  int16_t roll;
  int16_t pitch;
  int16_t yaw;
  int16_t gz;

  if ((roll_deg == NULL) || (pitch_deg == NULL) || (yaw_deg == NULL) || (gz_dps == NULL))
  {
    return false;
  }

  if (!jy901s_i2c_read_buf(buf, sizeof(buf)))
  {
    return false;
  }

  roll = jy901s_i16_le(&buf[(JY901S_REG_ROLL - JY901S_REG_AX) * 2U]);
  pitch = jy901s_i16_le(&buf[(JY901S_REG_ROLL - JY901S_REG_AX + 1U) * 2U]);
  yaw = jy901s_i16_le(&buf[(JY901S_REG_ROLL - JY901S_REG_AX + 2U) * 2U]);
  gz = jy901s_i16_le(&buf[10]);

  *roll_deg = jy901s_raw_to_deg(roll);
  *pitch_deg = jy901s_raw_to_deg(pitch);
  *yaw_deg = jy901s_raw_to_deg(yaw);
  *gz_dps = (float)gz / 32768.0f * 2000.0f;
  return true;
}

static bool jy901s_calibrate_offsets(void)
{
  float roll_sum = 0.0f;
  float pitch_sum = 0.0f;
  float yaw_sin_sum = 0.0f;
  float yaw_cos_sum = 0.0f;
  float gz_sum = 0.0f;
  uint32_t ok_count = 0U;
  uint32_t i;

  s_calibrated = 0U;
  s_offset_roll_deg = 0.0f;
  s_offset_pitch_deg = 0.0f;
  s_offset_yaw_deg = 0.0f;

  osDelay(JY901S_CALIB_SETTLE_MS);

  for (i = 0U; i < JY901S_CALIB_SAMPLES; i++)
  {
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float gz = 0.0f;

    if (jy901s_read_angles_gz(&roll, &pitch, &yaw, &gz))
    {
      roll_sum += roll;
      pitch_sum += pitch;
      yaw_sin_sum += sinf(yaw * (float)M_PI / 180.0f);
      yaw_cos_sum += cosf(yaw * (float)M_PI / 180.0f);
      gz_sum += JY901S_GZ_SIGN * JY901S_GZ_SCALE * gz;
      ok_count++;
    }

    if ((i + 1U) < JY901S_CALIB_SAMPLES)
    {
      osDelay(JY901S_CALIB_DELAY_MS);
    }
  }

  if (ok_count < JY901S_CALIB_MIN_OK)
  {
    return false;
  }

  s_offset_roll_deg = roll_sum / (float)ok_count;
  s_offset_pitch_deg = pitch_sum / (float)ok_count;
  s_offset_yaw_deg = atan2f(yaw_sin_sum, yaw_cos_sum) * 180.0f / (float)M_PI;
  s_gz_bias_dps = gz_sum / (float)ok_count;
  s_yaw_rad = 0.0f;
  jy901s_reset_gyro_yaw(HAL_GetTick());
  s_calibrated = 1U;
  return true;
}

static void jy901s_i2c_recover(void)
{
  (void)HAL_I2C_DeInit(&hi2c1);
  MX_I2C1_Init();
}

bool JY901S_Init(void)
{
  if (HAL_I2C_IsDeviceReady(&hi2c1,
                            (uint16_t)(JY901S_I2C_ADDR_7BIT << 1),
                            JY901S_DEVICE_READY_TRIES,
                            JY901S_I2C_TIMEOUT_MS) != HAL_OK)
  {
    jy901s_i2c_recover();

    if (HAL_I2C_IsDeviceReady(&hi2c1,
                              (uint16_t)(JY901S_I2C_ADDR_7BIT << 1),
                              JY901S_DEVICE_READY_TRIES,
                              JY901S_I2C_TIMEOUT_MS) != HAL_OK)
    {
      s_calibrated = 0U;
      return false;
    }
  }

#if JY901S_USE_6AXIS_FUSION
  (void)jy901s_set_6axis_fusion();
#endif

  return jy901s_calibrate_offsets();
}

bool JY901S_IsCalibrated(void)
{
  return (s_calibrated != 0U);
}

float JY901S_GetYawRad(void)
{
  return s_yaw_rad;
}

float JY901S_GetYawGyroDeg(void)
{
  return s_yaw_gyro_pure_deg;
}

void JY901S_ResetGyroYaw(void)
{
  jy901s_reset_gyro_yaw(HAL_GetTick());
}

bool JY901S_Read(JY901S_Data_t *data)
{
  uint8_t buf[JY901S_READ_LEN] = {0};
  int16_t ax;
  int16_t ay;
  int16_t az;
  int16_t gx;
  int16_t gy;
  int16_t gz;
  int16_t roll;
  int16_t pitch;
  int16_t yaw;
  int16_t temp;
  float roll_deg;
  float pitch_deg;
  float yaw_deg;
  float fused_yaw_deg;
  uint32_t now_tick;

  if (data == NULL)
  {
    return false;
  }

  if (!jy901s_i2c_read_buf(buf, sizeof(buf)))
  {
    return false;
  }

  ax = jy901s_i16_le(&buf[0]);
  ay = jy901s_i16_le(&buf[2]);
  az = jy901s_i16_le(&buf[4]);
  gx = jy901s_i16_le(&buf[6]);
  gy = jy901s_i16_le(&buf[8]);
  gz = jy901s_i16_le(&buf[10]);
  roll = jy901s_i16_le(&buf[(JY901S_REG_ROLL - JY901S_REG_AX) * 2U]);
  pitch = jy901s_i16_le(&buf[(JY901S_REG_ROLL - JY901S_REG_AX + 1U) * 2U]);
  yaw = jy901s_i16_le(&buf[(JY901S_REG_ROLL - JY901S_REG_AX + 2U) * 2U]);
  temp = jy901s_i16_le(&buf[(JY901S_REG_TEMP - JY901S_REG_AX) * 2U]);

  data->ax_g = (float)ax / 32768.0f * 16.0f;
  data->ay_g = (float)ay / 32768.0f * 16.0f;
  data->az_g = (float)az / 32768.0f * 16.0f;
  data->gx_dps = (float)gx / 32768.0f * 2000.0f;
  data->gy_dps = (float)gy / 32768.0f * 2000.0f;
  data->gz_dps = (float)gz / 32768.0f * 2000.0f;
  data->gz_bias_dps = s_gz_bias_dps;

  roll_deg = jy901s_raw_to_deg(roll);
  pitch_deg = jy901s_raw_to_deg(pitch);
  yaw_deg = jy901s_raw_to_deg(yaw);

  data->roll_raw_deg = roll_deg;
  data->pitch_raw_deg = pitch_deg;
  data->yaw_raw_deg = yaw_deg;
  data->roll_offset_deg = s_offset_roll_deg;
  data->pitch_offset_deg = s_offset_pitch_deg;
  data->yaw_offset_deg = s_offset_yaw_deg;

  if (s_calibrated != 0U)
  {
    data->roll_deg = jy901s_sub_deg(roll_deg, s_offset_roll_deg);
    data->pitch_deg = jy901s_sub_deg(pitch_deg, s_offset_pitch_deg);
    fused_yaw_deg = jy901s_sub_deg(yaw_deg, s_offset_yaw_deg);
  }
  else
  {
    data->roll_deg = roll_deg;
    data->pitch_deg = pitch_deg;
    fused_yaw_deg = yaw_deg;
  }

  data->yaw_fused_deg = fused_yaw_deg;

  now_tick = HAL_GetTick();
  if (s_calibrated != 0U)
  {
    jy901s_update_gyro_yaw(data->gz_dps, now_tick);
    jy901s_apply_fusion_correction(fused_yaw_deg);
    data->yaw_deg = jy901s_wrap_deg(s_yaw_gyro_deg);
    s_yaw_rad = data->yaw_deg * (float)M_PI / 180.0f;
  }
  else
  {
    data->yaw_deg = fused_yaw_deg;
    s_yaw_rad = fused_yaw_deg * (float)M_PI / 180.0f;
  }

  jy901s_fill_yaw_compare(data);

  data->temp_c = (float)temp / 100.0f;
  return true;
}

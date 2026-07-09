#include "drv8874.h"
#include "board.h"
#include "adc.h"
#include "tim.h"

#define DRV8874_SPEED_WINDOW_TICKS ((DRV8874_ENC_SAMPLE_HZ * DRV8874_SPEED_WINDOW_MS) / 1000U)

static int32_t s_enc_count = 0;
static int32_t s_enc_delta = 0;
static int32_t s_enc_last_delta = 0;
static uint8_t s_enc_last_ab = 0U;

static int32_t s_speed_window_sum = 0;
static uint16_t s_speed_window_ticks = 0;
static float s_speed_pps = 0.0f;

static float s_target_rpm = 0.0f;
static float s_pid_integral = 0.0f;
static float s_pid_prev_error = 0.0f;
static float s_pid_output = 0.0f;

float DRV8874_PID_KP=90.0f;
float DRV8874_PID_KI=5.0f;
float DRV8874_PID_KD=1.5f;
static float drv8874_fabsf(float x)
{
  return (x < 0.0f) ? -x : x;
}

static float drv8874_clampf(float x, float min_v, float max_v)
{
  if (x < min_v)
  {
    return min_v;
  }

  if (x > max_v)
  {
    return max_v;
  }

  return x;
}

static uint8_t drv8874_encoder_read_ab(void)
{
  uint8_t a = (HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin) == GPIO_PIN_SET) ? 1U : 0U;
  uint8_t b = (HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin) == GPIO_PIN_SET) ? 1U : 0U;
  return (uint8_t)((a << 1) | b);
}

static int8_t drv8874_encoder_quadrature_step(uint8_t prev, uint8_t curr)
{
  static const int8_t table[16] = {
      0, -1, 1, 0,
      1, 0, 0, -1,
      -1, 0, 0, 1,
      0, 1, -1, 0};

  return table[(prev << 2) | curr];
}

static void drv8874_speed_window_reset(void)
{
  s_speed_window_sum = 0;
  s_speed_window_ticks = 0;
  s_speed_pps = 0.0f;
}

static void drv8874_speed_pid_reset(void)
{
  s_pid_integral = 0.0f;
  s_pid_prev_error = 0.0f;
  s_pid_output = 0.0f;
}

static void drv8874_apply_signed_duty(float duty_signed)
{
  bool forward = (duty_signed >= 0.0f);
  float magnitude = drv8874_fabsf(duty_signed);

  if (magnitude > (float)DRV8874_PWM_MAX)
  {
    magnitude = (float)DRV8874_PWM_MAX;
  }

  DRV8874_SetDirection(forward);
  DRV8874_SetDuty((uint16_t)magnitude);
}

static void drv8874_encoder_init(void)
{
  s_enc_count = 0;
  s_enc_delta = 0;
  s_enc_last_delta = 0;
  s_enc_last_ab = drv8874_encoder_read_ab();
  drv8874_speed_window_reset();
  HAL_TIM_Base_Start_IT(&htim2);
}

void DRV8874_Init(void)
{
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0U);
  HAL_GPIO_WritePin(DRV_PH_GPIO_Port, DRV_PH_Pin, GPIO_PIN_RESET);
  ADC_App_Start();
  drv8874_encoder_init();
  drv8874_speed_pid_reset();
}

void DRV8874_SetDirection(bool forward)
{
  HAL_GPIO_WritePin(DRV_PH_GPIO_Port, DRV_PH_Pin, forward ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void DRV8874_SetDuty(uint16_t duty)
{
  if (duty > DRV8874_PWM_MAX)
  {
    duty = DRV8874_PWM_MAX;
  }

  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty);
}

void DRV8874_SetSpeed(int16_t speed)
{
  int16_t abs_speed = speed;
  bool forward = true;

  if (speed < 0)
  {
    forward = false;
    abs_speed = (int16_t)(-speed);
  }

  if (abs_speed > DRV8874_SPEED_MAX)
  {
    abs_speed = DRV8874_SPEED_MAX;
  }

  DRV8874_SetDirection(forward);
  DRV8874_SetDuty((uint16_t)((abs_speed * (int32_t)DRV8874_PWM_MAX) / DRV8874_SPEED_MAX));
}

void DRV8874_Stop(void)
{
  DRV8874_SetDuty(0U);
}

uint16_t DRV8874_ReadCurrentRaw(void)
{
  return ADC_App_GetRaw();
}

uint16_t DRV8874_ReadCurrent_mA(void)
{
  return ADC_App_GetMotorCurrent_mA();
}

void DRV8874_OnEncoderTimerTick(void)
{
  const uint8_t curr = drv8874_encoder_read_ab();
  const int8_t step = drv8874_encoder_quadrature_step(s_enc_last_ab, curr);

  if (step != 0)
  {
    s_enc_count += step;
    s_enc_delta += step;
    s_speed_window_sum += step;
  }

  s_enc_last_ab = curr;
  s_enc_last_delta = s_enc_delta;
  s_enc_delta = 0;

  s_speed_window_ticks++;
  if (s_speed_window_ticks >= DRV8874_SPEED_WINDOW_TICKS)
  {
    s_speed_pps = (float)s_speed_window_sum * 1000.0f / (float)DRV8874_SPEED_WINDOW_MS;
    s_speed_window_sum = 0;
    s_speed_window_ticks = 0;
  }
}

void DRV8874_ResetEncoder(void)
{
  s_enc_count = 0;
  s_enc_delta = 0;
  s_enc_last_delta = 0;
  drv8874_speed_window_reset();
}

int32_t DRV8874_GetEncoderCount(void)
{
  return s_enc_count;
}

int32_t DRV8874_GetEncoderDelta(void)
{
  return s_enc_last_delta;
}

float DRV8874_GetEncoderSpeed_PPS(void)
{
  return s_speed_pps;
}

float DRV8874_GetEncoderSpeed_Instant_PPS(void)
{
  return (float)s_enc_last_delta * (float)DRV8874_ENC_SAMPLE_HZ;
}

float DRV8874_GetEncoderSpeed_RPM(void)
{
  const float counts_per_rev = (float)DRV8874_ENC_LINES *
                               (float)DRV8874_GEAR_RATIO *
                               (float)DRV8874_ENC_QUAD;

  if (counts_per_rev <= 0.0f)
  {
    return 0.0f;
  }

  return s_speed_pps * 60.0f / counts_per_rev;
}

void DRV8874_SetTargetRpm(float target_rpm)
{
  s_target_rpm = target_rpm;
}

float DRV8874_GetTargetRpm(void)
{
  return s_target_rpm;
}

void DRV8874_SpeedPid_Reset(void)
{
  drv8874_speed_pid_reset();
}

void DRV8874_SpeedPid_Update(void)
{
  const float dt = (float)DRV8874_PID_DT_MS / 1000.0f;
  const float measured_rpm = DRV8874_GetEncoderSpeed_RPM();
  const float error = s_target_rpm - measured_rpm;
  float derivative;
  float output;

  if (drv8874_fabsf(s_target_rpm) < 0.5f)
  {
    drv8874_speed_pid_reset();
    DRV8874_Stop();
    return;
  }

  s_pid_integral += error * dt;
  s_pid_integral = drv8874_clampf(s_pid_integral,
                                  -DRV8874_PID_INTEGRAL_MAX,
                                  DRV8874_PID_INTEGRAL_MAX);

  derivative = (error - s_pid_prev_error) / dt;
  s_pid_prev_error = error;

  output = (DRV8874_PID_KP * error) +
           (DRV8874_PID_KI * s_pid_integral) +
           (DRV8874_PID_KD * derivative);

  output = drv8874_clampf(output, -(float)DRV8874_PWM_MAX, (float)DRV8874_PWM_MAX);
  s_pid_output = output;
  drv8874_apply_signed_duty(output);
}

float DRV8874_GetPidOutput(void)
{
  return s_pid_output;
}

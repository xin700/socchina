#ifndef PICKBOT_H
#define PICKBOT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint8_t id;
  int16_t target_rpm;
  int16_t actual_rpm;
  int32_t position_raw;
  float angle_deg;
  bool online;
} ZDT_WheelStatus_t;

typedef struct
{
  volatile uint16_t distance_mm;
  volatile uint16_t us100_uart2_distance_mm;
  volatile uint16_t us100_uart3_distance_mm;
  volatile int32_t drv8874_encoder_count;
  volatile float drv8874_encoder_pps;
  volatile float drv8874_encoder_rpm;
  volatile float drv8874_target_rpm;
  volatile float drv8874_pid_output;
  volatile uint16_t drv8874_current_ma;
  volatile bool us100_ok;
  volatile bool us100_uart2_ok;
  volatile bool us100_uart3_ok;
  /* 诊断：0=OK 1=无回包 2=保留 3=超量程 4=参数错 5=UART错；echo_us 为本次收到字节数 */
  volatile uint8_t us100_1_state;
  volatile uint8_t us100_2_state;
  volatile uint16_t us100_1_echo_us;
  volatile uint16_t us100_2_echo_us;
  volatile bool jy901s_ok;
  volatile bool jy901s_calibrated;
  volatile float jy901s_ax_g;
  volatile float jy901s_ay_g;
  volatile float jy901s_az_g;
  volatile float jy901s_gx_dps;
  volatile float jy901s_gy_dps;
  volatile float jy901s_gz_dps;
  volatile float jy901s_roll_raw_deg;
  volatile float jy901s_pitch_raw_deg;
  volatile float jy901s_yaw_raw_deg;
  volatile float jy901s_roll_offset_deg;
  volatile float jy901s_pitch_offset_deg;
  volatile float jy901s_yaw_offset_deg;
  volatile float jy901s_roll_deg;
  volatile float jy901s_pitch_deg;
  volatile float jy901s_yaw_deg;           /* 主 Yaw：gz 积分 + 融合辅助修正 */
  volatile float jy901s_yaw_fused_deg;     /* 模块融合 raw-offset，修正参考 */
  volatile float jy901s_gz_bias_dps;
  volatile float jy901s_yaw_gyro_deg;
  volatile float jy901s_yaw_gyro_wrap_deg;
  volatile float jy901s_yaw_diff_deg;
  volatile float jy901s_temp_c;
  volatile bool zdt_enabled;
  volatile ZDT_WheelStatus_t zdt_left;
  volatile ZDT_WheelStatus_t zdt_right;
  volatile float odom_x_m;
  volatile float odom_y_m;
  volatile float odom_yaw_rad;
} Pickbot_Status_t;

void Pickbot_Task_Drv8874(void);
void Pickbot_Task_ZDT(void);
void Pickbot_Task_US100(void);
void Pickbot_Task_JY901S(void);

void Pickbot_ZDT_SetWheelSpeed(int16_t left_rpm, int16_t right_rpm);
void Pickbot_ZDT_SetTargetRpm(int16_t rpm);
void Pickbot_Drv8874_SetSpeed(int16_t speed);
void Pickbot_Drv8874_SetTargetRpm(float target_rpm);
void Pickbot_Odom_Reset(void);

bool Pickbot_ZDT_ResetClogProtection(uint8_t motor_id);
bool Pickbot_ZDT_ResetClogProtectionAll(void);
bool Pickbot_ZDT_RestoreMotor(uint8_t motor_id);
bool Pickbot_ZDT_ReadWheelStatusFlag(uint8_t motor_id, uint16_t *flag);
bool Pickbot_ZDT_IsWheelStallProtected(uint8_t motor_id, bool *stall_active);

Pickbot_Status_t Pickbot_GetStatus(void);
const volatile Pickbot_Status_t *Pickbot_GetStatusPtr(void);

#endif

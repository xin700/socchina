#ifndef JY901S_H

#define JY901S_H



#include <stdint.h>

#include <stdbool.h>



typedef struct

{

  float ax_g;

  float ay_g;

  float az_g;

  float gx_dps;

  float gy_dps;

  float gz_dps;

  float gz_bias_dps;

  float roll_raw_deg;

  float pitch_raw_deg;

  float yaw_raw_deg;

  float roll_offset_deg;

  float pitch_offset_deg;

  float yaw_offset_deg;

  float roll_deg;

  float pitch_deg;

  float yaw_fused_deg;     /* 模块融合角 raw-offset，用于 0° 附近辅助修正 */

  float yaw_deg;           /* 主输出：gz 积分 + 融合辅助修正 */

  float yaw_gyro_deg;      /* 纯 gz 积分（未融合修正，未 wrap） */

  float yaw_gyro_wrap_deg; /* wrap 后的纯 gz 积分 */

  float yaw_diff_deg;      /* yaw_fused_deg - yaw_gyro_wrap_deg */

  float temp_c;

} JY901S_Data_t;



bool JY901S_Init(void);

bool JY901S_Read(JY901S_Data_t *data);

bool JY901S_IsCalibrated(void);

float JY901S_GetYawRad(void);

float JY901S_GetYawGyroDeg(void);

void JY901S_ResetGyroYaw(void);



#endif


#ifndef ZDT_ODOMETRY_H
#define ZDT_ODOMETRY_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  float x_m;
  float y_m;
  float yaw_rad;
} ZdtOdometryPose_t;

void ZdtOdometry_Init(void);
void ZdtOdometry_Reset(void);
void ZdtOdometry_ResetWithYaw(float yaw_rad);
void ZdtOdometry_Seed(int32_t left_raw, int32_t right_raw);
void ZdtOdometry_Update(int32_t left_raw, int32_t right_raw, uint32_t dt_ms);
void ZdtOdometry_UpdateWithYaw(int32_t left_raw,
                               int32_t right_raw,
                               float yaw_rad,
                               bool imu_yaw_valid,
                               uint32_t dt_ms);
ZdtOdometryPose_t ZdtOdometry_GetPose(void);

#endif

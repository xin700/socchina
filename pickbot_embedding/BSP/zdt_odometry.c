#include "zdt_odometry.h"

#include "board.h"



#include <math.h>



#ifndef M_PI

#define M_PI 3.14159265358979323846

#endif



static ZdtOdometryPose_t s_pose = {0};

static int32_t s_last_left_raw = 0;

static int32_t s_last_right_raw = 0;

static float s_last_yaw_rad = 0.0f;

static uint8_t s_has_last_sample = 0U;



static float zdt_raw_delta_to_m(int32_t delta_raw)

{

  const float rev = (float)delta_raw / ZDT_POS_FULL_SCALE;

  return rev * (2.0f * (float)M_PI) * PICKBOT_WHEEL_RADIUS_M;

}



static float zdt_wrap_pi(float rad)

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



void ZdtOdometry_Init(void)

{

  ZdtOdometry_Reset();

}



void ZdtOdometry_Reset(void)

{

  s_pose.x_m = 0.0f;

  s_pose.y_m = 0.0f;

  s_pose.yaw_rad = 0.0f;

  s_last_yaw_rad = 0.0f;

  s_last_left_raw = 0;

  s_last_right_raw = 0;

  s_has_last_sample = 0U;

}



void ZdtOdometry_ResetWithYaw(float yaw_rad)

{

  ZdtOdometry_Reset();

  s_pose.yaw_rad = yaw_rad;

  s_last_yaw_rad = yaw_rad;

}



void ZdtOdometry_Seed(int32_t left_raw, int32_t right_raw)

{

  s_last_left_raw = left_raw;

  s_last_right_raw = right_raw;

  s_has_last_sample = 1U;

}



void ZdtOdometry_UpdateWithYaw(int32_t left_raw,

                               int32_t right_raw,

                               float yaw_rad,

                               bool imu_yaw_valid,

                               uint32_t dt_ms)

{

  float ds_l;

  float ds_r;

  float ds;

  float dyaw;

  float yaw_mid;



  (void)dt_ms;



  if (s_has_last_sample == 0U)

  {

    s_last_left_raw = left_raw;

    s_last_right_raw = right_raw;

    s_has_last_sample = 1U;

    if (imu_yaw_valid)

    {

      s_pose.yaw_rad = yaw_rad;

      s_last_yaw_rad = yaw_rad;

    }

    return;

  }



  ds_l = zdt_raw_delta_to_m(left_raw - s_last_left_raw);

  ds_r = zdt_raw_delta_to_m((right_raw - s_last_right_raw) * ZDT_WHEEL_RIGHT_ODOM_SIGN);

  s_last_left_raw = left_raw;

  s_last_right_raw = right_raw;



  ds = (ds_l + ds_r) * 0.5f;



  if (imu_yaw_valid)

  {

    dyaw = zdt_wrap_pi(yaw_rad - s_last_yaw_rad);

    yaw_mid = s_last_yaw_rad + (dyaw * 0.5f);

    s_pose.x_m += ds * cosf(yaw_mid);

    s_pose.y_m += ds * sinf(yaw_mid);

    s_pose.yaw_rad = yaw_rad;

    s_last_yaw_rad = yaw_rad;

  }

  else

  {

    if (PICKBOT_TRACK_WIDTH_M > 0.0f)

    {

      dyaw = (ds_r - ds_l) / PICKBOT_TRACK_WIDTH_M;

    }

    else

    {

      dyaw = 0.0f;

    }



    yaw_mid = s_pose.yaw_rad + (dyaw * 0.5f);

    s_pose.x_m += ds * cosf(yaw_mid);

    s_pose.y_m += ds * sinf(yaw_mid);

    s_pose.yaw_rad += dyaw;

    s_last_yaw_rad = s_pose.yaw_rad;

  }

}



void ZdtOdometry_Update(int32_t left_raw, int32_t right_raw, uint32_t dt_ms)

{

  ZdtOdometry_UpdateWithYaw(left_raw, right_raw, 0.0f, false, dt_ms);

}



ZdtOdometryPose_t ZdtOdometry_GetPose(void)

{

  return s_pose;

}


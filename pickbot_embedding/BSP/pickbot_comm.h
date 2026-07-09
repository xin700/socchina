#ifndef PICKBOT_COMM_H
#define PICKBOT_COMM_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Pickbot 上位机串口协议 v1（USART1 @115200 8N1，小端 float）
 *
 * 帧格式：
 *   [0xAA][0x55][TYPE][LEN][PAYLOAD...][CRC16_L][CRC16_H]
 *   CRC16-CCITT(0x1021, init 0xFFFF) 覆盖 TYPE + LEN + PAYLOAD
 *
 * --- 下位机 → 上位机（TYPE=0x01, LEN=20, TIM1 约 50Hz）---
 *   float[0] odom_x_m        里程计 X (m)
 *   float[1] odom_y_m        里程计 Y (m)
 *   float[2] yaw_deg         IMU Yaw (deg)，gz 积分 + 融合辅助修正
 *   float[3] us100_left_mm   左超声 (mm)，无效时 -1
 *   float[4] us100_right_mm  右超声 (mm)，无效时 -1
 *
 * --- 上位机 → 下位机（TYPE=0x02, LEN=32）---
 *   float[0] lin_speed       目标线速度幅值 (m/s)，V=(VL+VR)/2 中的 V
 *   float[1] lin_dir         线速度方向：≥0.5 正，<0.5 负
 *   float[2] ang_speed       目标角速度幅值 (rad/s)，ω=(VR-VL)/轮距
 *   float[3] ang_dir         角速度方向：≥0.5 逆时针(+)，<0.5 顺时针(-)
 *   float[4] target_x_m      目标 X (m)，模式 2 用
 *   float[5] target_y_m      目标 Y (m)，模式 2 用
 *   float[6] control_mode    1=速度控制，2=先转向目标点再直线到达
 *   float[7] drv8874_speed   捡球电机速度 (-1000~1000)
 */

#define PICKBOT_COMM_SOF0           0xAAU
#define PICKBOT_COMM_SOF1           0x55U
#define PICKBOT_COMM_TYPE_TELEM     0x01U
#define PICKBOT_COMM_TYPE_CMD       0x02U
#define PICKBOT_COMM_TELEM_FLOATS   5U
#define PICKBOT_COMM_CMD_FLOATS     8U
#define PICKBOT_COMM_TELEM_BYTES    (PICKBOT_COMM_TELEM_FLOATS * 4U)
#define PICKBOT_COMM_CMD_BYTES      (PICKBOT_COMM_CMD_FLOATS * 4U)

/* 超过该时间未收到有效上位机指令则判定断连并停车 */
#define PICKBOT_COMM_LINK_TIMEOUT_MS  200U

typedef struct
{
  volatile bool fresh;
  float lin_speed_mps;
  float lin_dir;
  float ang_speed_rps;
  float ang_dir;
  float target_x_m;
  float target_y_m;
  float control_mode;
  float drv8874_speed;
} PickbotComm_Cmd_t;

void PickbotComm_Init(void);
void PickbotComm_OnUartRxCplt(void);
void PickbotComm_OnUartError(void);
void PickbotComm_OnTim1Tick(void);
void PickbotComm_ApplyMotion(void);

bool PickbotComm_IsLinkOk(void);
const PickbotComm_Cmd_t *PickbotComm_GetCmd(void);
uint32_t PickbotComm_GetRxFrameCount(void);
uint32_t PickbotComm_GetRxCrcErrorCount(void);
uint32_t PickbotComm_GetTxFrameCount(void);

#endif

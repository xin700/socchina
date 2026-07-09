#ifndef BOARD_H
#define BOARD_H

#include "stm32f1xx_hal.h"

/* 编码器 AB 相（GPIO 软件测速） */
#define ENC_A_GPIO_Port      GPIOA
#define ENC_A_Pin            GPIO_PIN_0
#define ENC_B_GPIO_Port      GPIOA
#define ENC_B_Pin            GPIO_PIN_1

/* DRV8874 捡球电机 CHP-36GP-555ABHL：17 线 AB 霍尔，减速比在铭牌/订单上查 */
#define DRV8874_ENC_LINES        17U
#define DRV8874_ENC_QUAD         4U
#define DRV8874_GEAR_RATIO       139U   /* 按实际减速比修改，如 51/139/515 */

/* DRV8874 速度环：speed=1000 对应的目标 RPM，及 PID 参数（按实测微调） */
#define DRV8874_TARGET_RPM_MAX     65.0f

#define DRV8874_PID_DT_MS          20U
#define DRV8874_PID_INTEGRAL_MAX   1200.0f

/* DRV8874 */
#define DRV_PH_GPIO_Port     GPIOA
#define DRV_PH_Pin           GPIO_PIN_7

/* 置 1 时 CAN 自环测试（不接总线也可过 HAL_CAN_Start），联调电机前务必改回 0 */
#define PICKBOT_CAN_LOOPBACK   0

/* 差速 ZDT 轮：EMM 固件，单圈位置量程 0~65535
 * 左轮 CAN 地址 2、右轮 CAN 地址 1；右轮机械安装方向与左轮相反 */
#define ZDT_WHEEL_LEFT_ID           2U
#define ZDT_WHEEL_RIGHT_ID          1U
#define ZDT_WHEEL_RIGHT_RPM_SIGN    (-1)
#define ZDT_WHEEL_RIGHT_ODOM_SIGN   (-1)
#define ZDT_WHEEL_COUNT        2U
#define ZDT_POS_FULL_SCALE     65535.0f
#define ZDT_CAN_RESP_TIMEOUT   30U
#define ZDT_CAN_F6_ACK_TIMEOUT_MS  20U   /* 速度命令 F6 确认帧，过短易污染后续读位置 */

/* 里程计：按实际底盘测量修改，单位 m */
#define PICKBOT_WHEEL_RADIUS_M 0.0423492357171f
#define PICKBOT_TRACK_WIDTH_M    0.180f

/* US100 超声：UART 模式 @9600（跳线帽插上）
 * #1: USART2  PA2=TX → 模块 RX, PA3=RX ← 模块 TX
 * #2: USART3  PB10=TX, PB11=RX
 */
#define US100_SENSOR_COUNT        2U
#define US100_CMD_MEASURE         0x55U
#define US100_UART_TX_TIMEOUT_MS  100U
#define US100_UART_RX_TIMEOUT_MS  100U
#define US100_DIST_MIN_MM         20U     /* 约 2cm */
#define US100_DIST_MAX_MM         4500U   /* 约 4.5m */
#define US100_SENSOR_GAP_MS       60U

/* JY901S I2C：PB6/PB7，默认 7-bit 地址 0x50 */
#define JY901S_I2C_ADDR_7BIT      0x50U
#define JY901S_I2C_TIMEOUT_MS     50U
#define JY901S_POWERUP_DELAY_MS   300U   /* 首次 Init 前等 IMU 上电稳定 */
#define JY901S_INIT_RETRY_MS      500U   /* Init 失败后重试间隔 */
#define JY901S_DEVICE_READY_TRIES  10U    /* IsDeviceReady 重试次数 */
/* 上电静止时采样次数与间隔，用于求 Roll/Pitch/Yaw 零点基准 */
#define JY901S_CALIB_SAMPLES      40U
#define JY901S_CALIB_DELAY_MS     10U
#define JY901S_CALIB_MIN_OK       32U
#define JY901S_CALIB_SETTLE_MS    800U   /* Init 前等模块内部滤波收敛 */

/*
 * 主 Yaw = gz 积分 + 模块融合(raw-offset) 在 0° 附近辅助修正（见 jy901s.c）。
 * JY901S_FUSION_TRUST_ANGLE_DEG：|融合角|超过此值不再用融合拉回。
 */
#define JY901S_USE_6AXIS_FUSION       0
#define JY901S_GZ_SIGN                1.0f   /* 左右转反了改为 -1.0f */
#define JY901S_GZ_SCALE                1.0f   /* 90° 只到 70° 时可试 1.28f 等 */
#define JY901S_GZ_STATIONARY_DPS      2.5f
#define JY901S_STATIONARY_SAMPLES     15U
#define JY901S_GZ_BIAS_LPF            0.02f
#define JY901S_FUSION_TRUST_ANGLE_DEG 45.0f  /* |yaw_fused| 越大越不信融合，0 处最强 */
#define JY901S_FUSION_BLEND_KP        0.06f  /* 运动时融合修正比例 */
#define JY901S_FUSION_BLEND_KP_STATIC 0.20f  /* 静止时更强拉回防漂 */
#define JY901S_FUSION_MAX_ERR_DEG     30.0f  /* 融合与积分差过大则跳过本次修正 */

/* 备用 GPIO */
#define SPARE_PA5_GPIO_Port   GPIOA
#define SPARE_PA5_Pin         GPIO_PIN_5
#define SPARE_PA8_GPIO_Port   GPIOA
#define SPARE_PA8_Pin         GPIO_PIN_8
#define SPARE_PB0_GPIO_Port   GPIOB
#define SPARE_PB0_Pin         GPIO_PIN_0
#define SPARE_PB1_GPIO_Port   GPIOB
#define SPARE_PB1_Pin         GPIO_PIN_1
#define SPARE_PB8_GPIO_Port   GPIOB
#define SPARE_PB8_Pin         GPIO_PIN_8
#define SPARE_PB9_GPIO_Port   GPIOB
#define SPARE_PB9_Pin         GPIO_PIN_9

#endif

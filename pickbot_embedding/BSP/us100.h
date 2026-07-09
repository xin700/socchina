#ifndef US100_H
#define US100_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"

typedef enum
{
  US100_OK = 0,              /* 测距成功 */
  US100_ERR_NO_RESPONSE,     /* 发 0x55 后超时未收满 2 字节 */
  US100_ERR_OUT_OF_RANGE,    /* 距离超出有效量程 */
  US100_ERR_UART,            /* UART 错误（ORE/FE 等） */
  US100_ERR_PARAM            /* 参数错误 */
} US100_Result_t;

void US100_Init(void);

/* index: 0=USART2(PA2/PA3), 1=USART3(PB10/PB11)。echo_us 可传 NULL；调试时可看本次收到字节数 */
US100_Result_t US100_MeasureSensor(uint8_t index, uint16_t *distance_mm, uint32_t *echo_us);

bool US100_Measure_mm(uint16_t *distance_mm);
void US100_MeasurePair_mm(uint16_t *sensor1_distance_mm, bool *sensor1_ok,
                          uint16_t *sensor2_distance_mm, bool *sensor2_ok);

/* 调试：累计收到字节数 / 成功帧数 / UART 错误次数 */
extern volatile uint32_t g_us100_byte_count[];
extern volatile uint32_t g_us100_frame_count[];
extern volatile uint32_t g_us100_err_count[];
extern volatile uint32_t g_us100_trig_count[];

#endif

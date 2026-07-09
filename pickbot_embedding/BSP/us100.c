#include "us100.h"
#include "pickbot_comm.h"

#include "board.h"
#include "usart.h"
#include "cmsis_os.h"

typedef struct
{
  UART_HandleTypeDef *huart;
  volatile uint8_t rx_byte;
  volatile uint8_t rx_buf[2];
  volatile uint8_t rx_index;
  volatile uint16_t distance_mm;
  volatile uint8_t frame_ready;
  volatile uint8_t bytes_this_frame;
} US100_Chan_t;

static US100_Chan_t s_us100[US100_SENSOR_COUNT] =
{
  {&huart2, 0U, {0U, 0U}, 0U, 0U, 0U, 0U},
  {&huart3, 0U, {0U, 0U}, 0U, 0U, 0U, 0U}
};

static uint8_t us100_chan_index(const US100_Chan_t *chan)
{
  return (uint8_t)(chan - s_us100);
}

static uint8_t s_us100_inited = 0U;

volatile uint32_t g_us100_byte_count[US100_SENSOR_COUNT] = {0U, 0U};
volatile uint32_t g_us100_frame_count[US100_SENSOR_COUNT] = {0U, 0U};
volatile uint32_t g_us100_err_count[US100_SENSOR_COUNT] = {0U, 0U};
volatile uint32_t g_us100_trig_count[US100_SENSOR_COUNT] = {0U, 0U};

static US100_Chan_t *us100_chan_from_index(uint8_t index)
{
  if (index >= US100_SENSOR_COUNT)
  {
    return NULL;
  }

  return &s_us100[index];
}

static US100_Chan_t *us100_chan_from_huart(UART_HandleTypeDef *huart)
{
  uint32_t i;

  for (i = 0U; i < US100_SENSOR_COUNT; i++)
  {
    if (s_us100[i].huart == huart)
    {
      return &s_us100[i];
    }
  }

  return NULL;
}

static void us100_reset_frame(US100_Chan_t *chan)
{
  chan->rx_index = 0U;
  chan->frame_ready = 0U;
  chan->bytes_this_frame = 0U;
  chan->distance_mm = 0U;
}

static void us100_start_rx_it(US100_Chan_t *chan)
{
  if (HAL_UART_Receive_IT(chan->huart, (uint8_t *)&chan->rx_byte, 1U) != HAL_OK)
  {
    g_us100_err_count[us100_chan_index(chan)]++;
  }
}

static void us100_on_rx_byte(US100_Chan_t *chan)
{
  uint8_t index = us100_chan_index(chan);

  g_us100_byte_count[index]++;
  chan->bytes_this_frame++;

  chan->rx_buf[chan->rx_index] = chan->rx_byte;
  chan->rx_index++;

  if (chan->rx_index >= 2U)
  {
    chan->rx_index = 0U;
    chan->distance_mm = (uint16_t)(((uint16_t)chan->rx_buf[0] << 8) | chan->rx_buf[1]);
    chan->frame_ready = 1U;
    g_us100_frame_count[index]++;
  }

  us100_start_rx_it(chan);
}

static bool us100_wait_frame(US100_Chan_t *chan, uint32_t timeout_ms)
{
  const uint32_t t0 = HAL_GetTick();

  while ((chan->frame_ready == 0U) && ((HAL_GetTick() - t0) < timeout_ms))
  {
  }

  return (chan->frame_ready != 0U);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  US100_Chan_t *chan = us100_chan_from_huart(huart);

  if (chan != NULL)
  {
    us100_on_rx_byte(chan);
    return;
  }

  if (huart == &huart1)
  {
    PickbotComm_OnUartRxCplt();
    return;
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  US100_Chan_t *chan = us100_chan_from_huart(huart);
  uint8_t index;

  if (huart == &huart1)
  {
    __HAL_UART_CLEAR_OREFLAG(huart);
    __HAL_UART_CLEAR_NEFLAG(huart);
    __HAL_UART_CLEAR_FEFLAG(huart);
    PickbotComm_OnUartError();
    return;
  }

  if (chan == NULL)
  {
    return;
  }

  index = us100_chan_index(chan);
  g_us100_err_count[index]++;

  __HAL_UART_CLEAR_OREFLAG(huart);
  __HAL_UART_CLEAR_NEFLAG(huart);
  __HAL_UART_CLEAR_FEFLAG(huart);

  chan->rx_index = 0U;
  us100_start_rx_it(chan);
}

void US100_Init(void)
{
  uint32_t i;

  if (s_us100_inited != 0U)
  {
    return;
  }

  for (i = 0U; i < US100_SENSOR_COUNT; i++)
  {
    us100_reset_frame(&s_us100[i]);
    us100_start_rx_it(&s_us100[i]);
  }

  s_us100_inited = 1U;
}

US100_Result_t US100_MeasureSensor(uint8_t index, uint16_t *distance_mm, uint32_t *echo_us)
{
  US100_Chan_t *chan;
  uint8_t cmd = US100_CMD_MEASURE;
  uint16_t dist_mm;

  if ((index >= US100_SENSOR_COUNT) || (distance_mm == NULL))
  {
    return US100_ERR_PARAM;
  }

  US100_Init();
  chan = us100_chan_from_index(index);
  if (chan == NULL)
  {
    return US100_ERR_PARAM;
  }

  us100_reset_frame(chan);
  g_us100_trig_count[index]++;

  if (HAL_UART_Transmit(chan->huart, &cmd, 1U, US100_UART_TX_TIMEOUT_MS) != HAL_OK)
  {
    if (echo_us != NULL)
    {
      *echo_us = 0U;
    }
    return US100_ERR_UART;
  }

  if (!us100_wait_frame(chan, US100_UART_RX_TIMEOUT_MS))
  {
    if (echo_us != NULL)
    {
      *echo_us = chan->bytes_this_frame;
    }
    return US100_ERR_NO_RESPONSE;
  }

  dist_mm = chan->distance_mm;

  if (echo_us != NULL)
  {
    *echo_us = chan->bytes_this_frame;
  }

  if ((dist_mm < US100_DIST_MIN_MM) || (dist_mm > US100_DIST_MAX_MM))
  {
    return US100_ERR_OUT_OF_RANGE;
  }

  *distance_mm = dist_mm;
  return US100_OK;
}

bool US100_Measure_mm(uint16_t *distance_mm)
{
  return (US100_MeasureSensor(0U, distance_mm, NULL) == US100_OK);
}

void US100_MeasurePair_mm(uint16_t *sensor1_distance_mm, bool *sensor1_ok,
                          uint16_t *sensor2_distance_mm, bool *sensor2_ok)
{
  uint16_t dist1 = 0U;
  uint16_t dist2 = 0U;
  bool ok1;
  bool ok2;

  ok1 = (US100_MeasureSensor(0U, &dist1, NULL) == US100_OK);
  osDelay(US100_SENSOR_GAP_MS);
  ok2 = (US100_MeasureSensor(1U, &dist2, NULL) == US100_OK);

  if (sensor1_ok != NULL)
  {
    *sensor1_ok = ok1;
  }

  if (sensor2_ok != NULL)
  {
    *sensor2_ok = ok2;
  }

  if (ok1 && (sensor1_distance_mm != NULL))
  {
    *sensor1_distance_mm = dist1;
  }

  if (ok2 && (sensor2_distance_mm != NULL))
  {
    *sensor2_distance_mm = dist2;
  }
}

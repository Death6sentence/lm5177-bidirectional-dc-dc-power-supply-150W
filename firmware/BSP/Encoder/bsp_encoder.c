#include "bsp_encoder.h"

#include "tim.h"

static uint32_t encoder_last_count;

HAL_StatusTypeDef BSP_Encoder_Init(void)
{
  HAL_StatusTypeDef status = HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

  if (status == HAL_OK)
  {
    encoder_last_count = __HAL_TIM_GET_COUNTER(&htim2);
  }

  return status;
}

int32_t BSP_Encoder_GetDelta(void)
{
  uint32_t current_count = __HAL_TIM_GET_COUNTER(&htim2);
  uint32_t unsigned_delta = current_count - encoder_last_count;

  encoder_last_count = current_count;
  return (int32_t)unsigned_delta;
}

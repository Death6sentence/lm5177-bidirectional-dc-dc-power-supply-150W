#include "bsp_pwm.h"

#include "tim.h"

#define BSP_PWM_DTRK_CONTROL_LEVEL_MAX 1000U

static uint16_t BSP_PWM_DtrkClampCompare(uint16_t compare)
{
  return (compare > BSP_PWM_DTRK_COMPARE_MAX) ? BSP_PWM_DTRK_COMPARE_MAX :
                                                 compare;
}

static uint16_t BSP_PWM_DtrkControlLevelToCompare(uint16_t control_level)
{
  if (control_level > BSP_PWM_DTRK_CONTROL_LEVEL_MAX)
  {
    control_level = BSP_PWM_DTRK_CONTROL_LEVEL_MAX;
  }

  return (uint16_t)(((uint32_t)control_level * BSP_PWM_DTRK_COMPARE_MAX +
                     (BSP_PWM_DTRK_CONTROL_LEVEL_MAX / 2U)) /
                    BSP_PWM_DTRK_CONTROL_LEVEL_MAX);
}

HAL_StatusTypeDef BSP_PWM_Init(void)
{
  HAL_StatusTypeDef status = BSP_PWM_SyncStart(0U);

  if (status != HAL_OK)
  {
    return status;
  }

  status = BSP_PWM_DtrkStart(BSP_PWM_DTRK_COMPARE_MAX);
  if (status != HAL_OK)
  {
    (void)BSP_PWM_SyncStop();
  }

  return status;
}

HAL_StatusTypeDef BSP_PWM_SyncStart(uint16_t compare)
{
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, compare);
  return HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

void BSP_PWM_SyncSetCompare(uint16_t compare)
{
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, compare);
}

HAL_StatusTypeDef BSP_PWM_SyncStop(void)
{
  return HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
}

HAL_StatusTypeDef BSP_PWM_FanStart(uint16_t compare)
{
  __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, compare);
  return HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
}

void BSP_PWM_FanSetCompare(uint16_t compare)
{
  __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, compare);
}

HAL_StatusTypeDef BSP_PWM_FanStop(void)
{
  return HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
}

HAL_StatusTypeDef BSP_PWM_DtrkStart(uint16_t compare)
{
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,
                        BSP_PWM_DtrkClampCompare(compare));
  return HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
}

void BSP_PWM_DtrkSetCompare(uint16_t compare)
{
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,
                        BSP_PWM_DtrkClampCompare(compare));
}

HAL_StatusTypeDef BSP_PWM_DtrkStop(void)
{
  return HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
}

HAL_StatusTypeDef BSP_PWM_DtrkSetControlLevel(uint16_t control_level)
{
  if (control_level > BSP_PWM_DTRK_CONTROL_LEVEL_MAX)
  {
    control_level = BSP_PWM_DTRK_CONTROL_LEVEL_MAX;
  }

  BSP_PWM_DtrkSetCompare(
      BSP_PWM_DtrkControlLevelToCompare(control_level));
  return HAL_OK;
}

void BSP_PWM_SinSampleInterruptEnable(void)
{
  uint16_t next_compare =
      (uint16_t)(__HAL_TIM_GET_COUNTER(&htim1) +
                 BSP_PWM_SIN_SAMPLE_INTERVAL_TICKS);

  __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, next_compare);
  __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC1);
  HAL_NVIC_ClearPendingIRQ(TIM1_CC_IRQn);
  __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
}

void BSP_PWM_SinSampleInterruptDisable(void)
{
  __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1);
  __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC1);
  HAL_NVIC_ClearPendingIRQ(TIM1_CC_IRQn);
}

HAL_StatusTypeDef BSP_PWM_DtrkSoftStart(uint16_t target_control_level,
                                         uint32_t duration_ms)
{
  uint32_t start_tick;
  uint32_t elapsed_ms;
  uint16_t control_level;
  uint16_t last_control_level = UINT16_MAX;

  if (target_control_level > BSP_PWM_DTRK_CONTROL_LEVEL_MAX)
  {
    target_control_level = BSP_PWM_DTRK_CONTROL_LEVEL_MAX;
  }

  if (duration_ms == 0U)
  {
    return BSP_PWM_DtrkSetControlLevel(target_control_level);
  }

  start_tick = HAL_GetTick();
  do
  {
    elapsed_ms = HAL_GetTick() - start_tick;
    if (elapsed_ms > duration_ms)
    {
      elapsed_ms = duration_ms;
    }

    control_level = (uint16_t)(((uint32_t)target_control_level * elapsed_ms +
                                (duration_ms / 2U)) / duration_ms);
    if (control_level != last_control_level)
    {
      if (BSP_PWM_DtrkSetControlLevel(control_level) != HAL_OK)
      {
        return HAL_ERROR;
      }
      last_control_level = control_level;
    }
  } while (elapsed_ms < duration_ms);

  return HAL_OK;
}

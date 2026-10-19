#include "bsp_dac.h"

#include "dac.h"

#define BSP_DAC_REFERENCE_MV 3300U
#define BSP_DAC_MAX_COUNTS 4095U

static uint32_t BSP_DAC_MillivoltsToCounts(uint16_t millivolts)
{
  return ((uint32_t)millivolts * BSP_DAC_MAX_COUNTS +
          (BSP_DAC_REFERENCE_MV / 2U)) / BSP_DAC_REFERENCE_MV;
}

HAL_StatusTypeDef BSP_DAC_SetMillivolts(uint16_t millivolts)
{
  if (millivolts > BSP_DAC_REFERENCE_MV)
  {
    return HAL_ERROR;
  }

  return HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,
                          BSP_DAC_MillivoltsToCounts(millivolts));
}

HAL_StatusTypeDef BSP_DAC_SoftStart(uint16_t target_millivolts,
                                    uint32_t duration_ms)
{
  HAL_StatusTypeDef status;
  uint32_t target_counts;
  uint32_t start_tick;
  uint32_t elapsed_ms;
  uint32_t output_counts;
  uint32_t last_counts = UINT32_MAX;

  if (target_millivolts > BSP_DAC_REFERENCE_MV)
  {
    return HAL_ERROR;
  }

  target_counts = BSP_DAC_MillivoltsToCounts(target_millivolts);

  /* 先装载 0 码再使能 DAC，避免输出使能瞬间跳到目标电压。 */
  status = HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0U);
  if (status != HAL_OK)
  {
    return status;
  }

  status = HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
  if (status != HAL_OK)
  {
    return status;
  }

  if (duration_ms == 0U)
  {
    return HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,
                            target_counts);
  }

  start_tick = HAL_GetTick();
  do
  {
    elapsed_ms = HAL_GetTick() - start_tick;
    if (elapsed_ms > duration_ms)
    {
      elapsed_ms = duration_ms;
    }

    output_counts = (uint32_t)(((uint64_t)target_counts * elapsed_ms +
                                (duration_ms / 2U)) / duration_ms);
    if (output_counts != last_counts)
    {
      status = HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,
                                output_counts);
      if (status != HAL_OK)
      {
        return status;
      }
      last_counts = output_counts;
    }
  } while (elapsed_ms < duration_ms);

  return HAL_OK;
}

#include "bsp_fan.h"

#include "bsp_pwm.h"

#define BSP_FAN_PWM_COMPARE_MAX 65535U

static uint8_t bsp_fan_duty_percent;

static uint8_t BSP_Fan_ClampDutyPercent(uint8_t duty_percent)
{
  if (duty_percent == BSP_FAN_DUTY_OFF_PERCENT)
  {
    return BSP_FAN_DUTY_OFF_PERCENT;
  }

  if (duty_percent < BSP_FAN_DUTY_MIN_PERCENT)
  {
    return BSP_FAN_DUTY_MIN_PERCENT;
  }

  return (duty_percent > BSP_FAN_DUTY_MAX_PERCENT) ?
      BSP_FAN_DUTY_MAX_PERCENT : duty_percent;
}

static uint16_t BSP_Fan_DutyPercentToCompare(uint8_t duty_percent)
{
  return (uint16_t)((((uint32_t)duty_percent *
                      BSP_FAN_PWM_COMPARE_MAX) + 50U) / 100U);
}

HAL_StatusTypeDef BSP_Fan_Init(uint8_t initial_duty_percent)
{
  HAL_StatusTypeDef status;

  initial_duty_percent = BSP_Fan_ClampDutyPercent(initial_duty_percent);
  status = BSP_PWM_FanStart(
      BSP_Fan_DutyPercentToCompare(initial_duty_percent));
  if (status == HAL_OK)
  {
    bsp_fan_duty_percent = initial_duty_percent;
  }

  return status;
}

HAL_StatusTypeDef BSP_Fan_SetDutyPercent(uint8_t duty_percent)
{
  duty_percent = BSP_Fan_ClampDutyPercent(duty_percent);
  BSP_PWM_FanSetCompare(BSP_Fan_DutyPercentToCompare(duty_percent));
  bsp_fan_duty_percent = duty_percent;
  return HAL_OK;
}

uint8_t BSP_Fan_GetDutyPercent(void)
{
  return bsp_fan_duty_percent;
}

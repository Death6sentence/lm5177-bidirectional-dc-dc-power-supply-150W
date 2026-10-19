#include "bsp_gpio.h"

static void BSP_GPIO_LedWrite(GPIO_TypeDef *port, uint16_t pin,
                              BSP_OutputState state)
{
  /* 三个板载 LED 均为低电平点亮。 */
  GPIO_PinState level = (state == BSP_OUTPUT_ON) ? GPIO_PIN_RESET :
                                                    GPIO_PIN_SET;

  HAL_GPIO_WritePin(port, pin, level);
}

void BSP_GPIO_Init(void)
{
  /* 上电保持功率级关闭，MODE 使用板级默认高电平。 */
  BSP_GPIO_ModeWrite(BSP_GPIO_HIGH);
  BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
}

void BSP_GPIO_ModeWrite(BSP_GPIO_Level level)
{
  HAL_GPIO_WritePin(MODE_GPIO_Port, MODE_Pin, (GPIO_PinState)level);
}

void BSP_GPIO_EnableWrite(BSP_GPIO_Level level)
{
  HAL_GPIO_WritePin(EN_UVLO_GPIO_Port, EN_UVLO_Pin, (GPIO_PinState)level);
}

BSP_GPIO_Level BSP_GPIO_FaultRead(void)
{
  return (BSP_GPIO_Level)HAL_GPIO_ReadPin(FLT_GPIO_Port, FLT_Pin);
}

BSP_GPIO_Level BSP_GPIO_ButtonRead(void)
{
  return (BSP_GPIO_Level)HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);
}

void BSP_GPIO_LedYellowWrite(BSP_OutputState state)
{
  BSP_GPIO_LedWrite(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, state);
}

void BSP_GPIO_LedBlueWrite(BSP_OutputState state)
{
  BSP_GPIO_LedWrite(LED_BLUE_GPIO_Port, LED_BLUE_Pin, state);
}

void BSP_GPIO_LedRedWrite(BSP_OutputState state)
{
  BSP_GPIO_LedWrite(LED_RED_GPIO_Port, LED_RED_Pin, state);
}

void BSP_GPIO_ReadStatus(BSP_GPIO_Status *status)
{
  status->mode = (BSP_GPIO_Level)HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin);
  status->enable = (BSP_GPIO_Level)HAL_GPIO_ReadPin(EN_UVLO_GPIO_Port,
                                                    EN_UVLO_Pin);
  status->sync = (BSP_GPIO_Level)HAL_GPIO_ReadPin(SYNC_GPIO_Port, SYNC_Pin);
  status->dtrk = (BSP_GPIO_Level)HAL_GPIO_ReadPin(DTRK_GPIO_Port, DTRK_Pin);
  status->fault = BSP_GPIO_FaultRead();
  status->fan = (BSP_GPIO_Level)HAL_GPIO_ReadPin(FAN_GPIO_Port, FAN_Pin);
  status->button = BSP_GPIO_ButtonRead();
  status->led_yellow = (BSP_GPIO_Level)HAL_GPIO_ReadPin(LED_YELLOW_GPIO_Port,
                                                         LED_YELLOW_Pin);
  status->led_blue = (BSP_GPIO_Level)HAL_GPIO_ReadPin(LED_BLUE_GPIO_Port,
                                                      LED_BLUE_Pin);
  status->led_red = (BSP_GPIO_Level)HAL_GPIO_ReadPin(LED_RED_GPIO_Port,
                                                     LED_RED_Pin);
}

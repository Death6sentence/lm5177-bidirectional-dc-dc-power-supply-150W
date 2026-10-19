#include "bsp_button.h"

#include "bsp_gpio.h"

#define BSP_BUTTON_DEBOUNCE_MS 20U

static BSP_GPIO_Level button_last_level;
static BSP_GPIO_Level button_stable_level;
static uint32_t button_transition_tick;
static uint8_t button_is_down;

void BSP_Button_Init(void)
{
  button_last_level = BSP_GPIO_ButtonRead();
  button_stable_level = button_last_level;
  button_transition_tick = HAL_GetTick();
  button_is_down = 0U;
}

BSP_ButtonEvent BSP_Button_Update(void)
{
  BSP_GPIO_Level button_level = BSP_GPIO_ButtonRead();
  uint32_t now = HAL_GetTick();

  if (button_level != button_last_level)
  {
    button_last_level = button_level;
    button_transition_tick = now;
    return BSP_BUTTON_EVENT_NONE;
  }

  if ((button_level == button_stable_level) ||
      ((now - button_transition_tick) < BSP_BUTTON_DEBOUNCE_MS))
  {
    return BSP_BUTTON_EVENT_NONE;
  }

  button_stable_level = button_level;
  if (button_is_down == 0U)
  {
    button_is_down = 1U;
    return BSP_BUTTON_EVENT_PRESSED;
  }

  button_is_down = 0U;
  return BSP_BUTTON_EVENT_RELEASED;
}

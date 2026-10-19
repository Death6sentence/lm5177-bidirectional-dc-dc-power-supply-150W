#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum
{
  BSP_GPIO_LOW = GPIO_PIN_RESET,
  BSP_GPIO_HIGH = GPIO_PIN_SET
} BSP_GPIO_Level;

typedef enum
{
  BSP_OUTPUT_OFF = 0U,
  BSP_OUTPUT_ON = 1U
} BSP_OutputState;

/**
 * @brief 状态页需要显示的板级 GPIO 原始电平
 */
typedef struct
{
  BSP_GPIO_Level mode;
  BSP_GPIO_Level enable;
  BSP_GPIO_Level sync;
  BSP_GPIO_Level dtrk;
  BSP_GPIO_Level fault;
  BSP_GPIO_Level fan;
  BSP_GPIO_Level button;
  BSP_GPIO_Level led_yellow;
  BSP_GPIO_Level led_blue;
  BSP_GPIO_Level led_red;
} BSP_GPIO_Status;

void BSP_GPIO_Init(void);
void BSP_GPIO_ModeWrite(BSP_GPIO_Level level);
void BSP_GPIO_EnableWrite(BSP_GPIO_Level level);
BSP_GPIO_Level BSP_GPIO_FaultRead(void);
BSP_GPIO_Level BSP_GPIO_ButtonRead(void);

void BSP_GPIO_LedYellowWrite(BSP_OutputState state);
void BSP_GPIO_LedBlueWrite(BSP_OutputState state);
void BSP_GPIO_LedRedWrite(BSP_OutputState state);

void BSP_GPIO_ReadStatus(BSP_GPIO_Status *status);

#ifdef __cplusplus
}
#endif

#endif

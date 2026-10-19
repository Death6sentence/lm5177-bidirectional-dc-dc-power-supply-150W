#ifndef BSP_BUTTON_H
#define BSP_BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum
{
  BSP_BUTTON_EVENT_NONE = 0U,
  BSP_BUTTON_EVENT_PRESSED,
  BSP_BUTTON_EVENT_RELEASED
} BSP_ButtonEvent;

/** 初始化按键去抖状态，需在 GPIO 初始化完成后调用。 */
void BSP_Button_Init(void);

/** 周期调用并返回稳定电平变化产生的按键事件。 */
BSP_ButtonEvent BSP_Button_Update(void);

#ifdef __cplusplus
}
#endif

#endif

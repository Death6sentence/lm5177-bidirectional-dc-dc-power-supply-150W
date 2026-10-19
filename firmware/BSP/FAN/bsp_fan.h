#ifndef BSP_FAN_H
#define BSP_FAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "main.h"

#define BSP_FAN_DUTY_MAX_PERCENT 100U
#define BSP_FAN_DUTY_OFF_PERCENT 0U
#define BSP_FAN_DUTY_MIN_PERCENT 20U

HAL_StatusTypeDef BSP_Fan_Init(uint8_t initial_duty_percent);
HAL_StatusTypeDef BSP_Fan_SetDutyPercent(uint8_t duty_percent);
uint8_t BSP_Fan_GetDutyPercent(void);

#ifdef __cplusplus
}
#endif

#endif

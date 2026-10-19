#ifndef BSP_ENCODER_H
#define BSP_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* TIM2 is configured for TI1 mode, so one valid TI1 edge is one step. */
#define BSP_ENCODER_MILLIVOLTS_PER_COUNT 10U

HAL_StatusTypeDef BSP_Encoder_Init(void);
int32_t BSP_Encoder_GetDelta(void);

#ifdef __cplusplus
}
#endif

#endif

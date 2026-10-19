#ifndef BSP_PWM_H
#define BSP_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define BSP_PWM_DTRK_COMPARE_MAX 32U
#define BSP_PWM_DTRK_CARRIER_HZ 250000U
#define BSP_PWM_SIN_SAMPLE_INTERVAL_TICKS 400U
#define BSP_PWM_SIN_SAMPLE_HZ 20000U

/** 启动板载默认 PWM 通道，初始比较值均为 0。 */
HAL_StatusTypeDef BSP_PWM_Init(void);

HAL_StatusTypeDef BSP_PWM_SyncStart(uint16_t compare);
void BSP_PWM_SyncSetCompare(uint16_t compare);
HAL_StatusTypeDef BSP_PWM_SyncStop(void);

HAL_StatusTypeDef BSP_PWM_FanStart(uint16_t compare);
void BSP_PWM_FanSetCompare(uint16_t compare);
HAL_StatusTypeDef BSP_PWM_FanStop(void);

HAL_StatusTypeDef BSP_PWM_DtrkStart(uint16_t compare);
void BSP_PWM_DtrkSetCompare(uint16_t compare);
HAL_StatusTypeDef BSP_PWM_DtrkStop(void);
HAL_StatusTypeDef BSP_PWM_DtrkSetControlLevel(uint16_t control_level);
void BSP_PWM_SinSampleInterruptEnable(void);
void BSP_PWM_SinSampleInterruptDisable(void);
HAL_StatusTypeDef BSP_PWM_DtrkSoftStart(uint16_t target_control_level,
                                         uint32_t duration_ms);

#ifdef __cplusplus
}
#endif

#endif

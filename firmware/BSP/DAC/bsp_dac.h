#ifndef BSP_DAC_H
#define BSP_DAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief 启动 DAC1，并将 PA4 输出由 0V 线性提升到目标电压
 * @param target_millivolts: 目标电压，范围 0 至 3300mV
 * @param duration_ms: 缓启动时间；传入 0 时立即输出目标电压
 * @return HAL 状态
 * @note 斜坡更新分辨率由 1ms 系统节拍决定。
 */
HAL_StatusTypeDef BSP_DAC_SoftStart(uint16_t target_millivolts,
                                    uint32_t duration_ms);

/** 立即更新 DAC 输出，不重启 DAC 或先写入 0 码。 */
HAL_StatusTypeDef BSP_DAC_SetMillivolts(uint16_t millivolts);

#ifdef __cplusplus
}
#endif

#endif

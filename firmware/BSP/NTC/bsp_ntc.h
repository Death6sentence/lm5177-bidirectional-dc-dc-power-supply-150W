#ifndef BSP_NTC_H
#define BSP_NTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define BSP_NTC_ADC_MAX_COUNTS 4095U
#define BSP_NTC_PULLUP_RESISTANCE_OHMS 10000.0f
#define BSP_NTC_NOMINAL_RESISTANCE_OHMS 100000.0f
#define BSP_NTC_NOMINAL_TEMPERATURE_KELVIN 298.15f
#define BSP_NTC_BETA_KELVIN 3950.0f

typedef enum
{
  BSP_NTC_STATUS_OK = 0U,
  BSP_NTC_STATUS_SHORT,
  BSP_NTC_STATUS_OPEN,
  BSP_NTC_STATUS_INVALID
} BSP_NTC_Status;

typedef struct
{
  uint16_t raw;
  int32_t temperature_deci_celsius;
  BSP_NTC_Status status;
} BSP_NTC_Sample;

/**
 * @brief 将 12 位 ADC 原始值换算为 100K/B3950 NTC 温度
 * @param raw: NTC 分压节点的 ADC 原始值
 * @return 原始值、摄氏温度（0.1°C）及采样状态
 */
BSP_NTC_Sample BSP_NTC_RawToSample(uint16_t raw);

#ifdef __cplusplus
}
#endif

#endif

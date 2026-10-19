#ifndef BSP_ADC_H
#define BSP_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "bsp_ntc.h"

/**
 * @brief ADC 原始值、ADC 节点电压及按板级分压比例还原后的电压值
 */
typedef struct
{
  uint16_t raw;
  uint16_t node_millivolts;
  uint16_t millivolts;
} BSP_ADC_Sample;

/**
 * @brief IMON 原始 ADC 值及按板级传输模型计算的有符号电流
 * @note 当前实现默认使用 1.590V 零点和 50.0mV/A 灵敏度；板上正电流使
 *       IMON 电压升高
 */
typedef struct
{
  uint16_t raw;
  int32_t milliamps;
} BSP_ADC_ImonSample;

/**
 * @brief 启动 VOUT/IMON 内部运放并校准板载 ADC1 和 ADC2
 * @return HAL_OK 表示两个运放均已启动且两个 ADC 均校准成功
 */
HAL_StatusTypeDef BSP_ADC_Init(void);

/**
 * @brief 使用当前 RAM 校准零点将 IMON 原始值换算为电流
 * @param raw: 12 位 ADC 原始值
 * @return 有符号电流，单位 mA
 */
int32_t BSP_ADC_ImonRawToMilliamps(uint16_t raw);

/**
 * @brief 使用当前 RAM 校准零点将目标电流换算为 ADC 原始值
 * @param milliamps: 有符号目标电流，单位 mA
 * @return 饱和到 12 位 ADC 范围的原始值
 */
uint16_t BSP_ADC_ImonMilliampsToRaw(int32_t milliamps);

/**
 * @brief 以当前原始值和参考电流更新本次上电期间的 IMON 零点
 * @param raw: 校准时的 12 位 ADC 原始值
 * @param reference_milliamps: 外部仪表测得的参考电流，单位 mA
 */
void BSP_ADC_ImonCalibrateOffset(uint16_t raw,
                                 int32_t reference_milliamps);

/**
 * @brief 按生成配置读取 IMON、VIN 和 VOUT 采样值
 * @note ADC1 Rank1 为 TEM，Rank2 为 VIN；ADC2 Rank1 为 VOUT，Rank2 为 IMON
 * @param vin: VIN 采样结果
 * @param vout: VOUT 采样结果
 * @param imon: IMON 原始值和计算电流
 * @param ntc: PA0/TEM_ADC 原始值和 NTC 温度
 * @return HAL 状态
 */
HAL_StatusTypeDef BSP_ADC_ReadMeasurements(BSP_ADC_Sample *vin,
                                           BSP_ADC_Sample *vout,
                                           BSP_ADC_ImonSample *imon,
                                           BSP_NTC_Sample *ntc);

#ifdef __cplusplus
}
#endif

#endif

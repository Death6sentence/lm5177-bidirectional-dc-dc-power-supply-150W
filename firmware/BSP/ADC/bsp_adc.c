#include "bsp_adc.h"

#include "adc.h"
#include "opamp.h"

#define BSP_ADC_VREF_MV 3300U
#define BSP_ADC_MAX_COUNTS 4095U
#define BSP_ADC_POLL_TIMEOUT_MS 10U
#define BSP_ADC_ADC1_SEQUENCE_LENGTH 2U
#define BSP_ADC_ADC2_SEQUENCE_LENGTH 2U

#define BSP_ADC_VIN_DIVIDER_TOP_OHMS 680000U
#define BSP_ADC_VIN_DIVIDER_BOTTOM_OHMS 73500U
#define BSP_ADC_VOUT_DIVIDER_TOP_OHMS 10000U
#define BSP_ADC_VOUT_DIVIDER_BOTTOM_OHMS 990U

#define BSP_ADC_IMON_DEFAULT_BIAS_UV_X10 15900000LL
#define BSP_ADC_IMON_SENSITIVITY_UV_X10_PER_MA 500LL
#define BSP_ADC_UV_PER_MV 1000LL
#define BSP_ADC_UV_SCALE 10LL

static int64_t bsp_adc_imon_bias_uv_x10 =
    BSP_ADC_IMON_DEFAULT_BIAS_UV_X10;

static uint16_t BSP_ADC_CountsToNodeMillivolts(uint16_t raw)
{
  return (uint16_t)(((uint32_t)raw * BSP_ADC_VREF_MV +
                     (BSP_ADC_MAX_COUNTS / 2U)) /
                    BSP_ADC_MAX_COUNTS);
}

static uint16_t BSP_ADC_ScaleMillivolts(uint16_t millivolts,
                                       uint64_t numerator,
                                       uint64_t denominator)
{
  return (uint16_t)((((uint64_t)millivolts * numerator) +
                     (denominator / 2U)) / denominator);
}

static uint16_t BSP_ADC_NodeMillivoltsToSourceMillivolts(
    uint16_t node_millivolts, uint32_t divider_top_ohms,
    uint32_t divider_bottom_ohms)
{
  uint64_t divider_numerator =
      (uint64_t)divider_top_ohms + divider_bottom_ohms;
  uint64_t divider_denominator = divider_bottom_ohms;

  return BSP_ADC_ScaleMillivolts(
      node_millivolts,
      divider_numerator, divider_denominator);
}

static int64_t BSP_ADC_DivideRoundedNearest(int64_t numerator,
                                           int64_t denominator)
{
  int64_t rounding = denominator / 2LL;

  return (numerator >= 0LL) ?
      ((numerator + rounding) / denominator) :
      -(((-numerator) + rounding) / denominator);
}

static int64_t BSP_ADC_CountsToScaledMicrovolts(uint16_t raw)
{
  return BSP_ADC_DivideRoundedNearest(
      (int64_t)raw * BSP_ADC_VREF_MV * BSP_ADC_UV_PER_MV *
          BSP_ADC_UV_SCALE,
      BSP_ADC_MAX_COUNTS);
}

int32_t BSP_ADC_ImonRawToMilliamps(uint16_t raw)
{
  int64_t numerator = ((int64_t)raw * BSP_ADC_VREF_MV *
                       BSP_ADC_UV_PER_MV * BSP_ADC_UV_SCALE) -
                      (bsp_adc_imon_bias_uv_x10 * BSP_ADC_MAX_COUNTS);
  int64_t denominator = BSP_ADC_IMON_SENSITIVITY_UV_X10_PER_MA *
                        BSP_ADC_MAX_COUNTS;

  return (int32_t)BSP_ADC_DivideRoundedNearest(numerator, denominator);
}

uint16_t BSP_ADC_ImonMilliampsToRaw(int32_t milliamps)
{
  int64_t imon_uv_x10 = bsp_adc_imon_bias_uv_x10 +
                        ((int64_t)milliamps *
                         BSP_ADC_IMON_SENSITIVITY_UV_X10_PER_MA);
  int64_t raw = BSP_ADC_DivideRoundedNearest(
      imon_uv_x10 * BSP_ADC_MAX_COUNTS,
      (int64_t)BSP_ADC_VREF_MV * BSP_ADC_UV_PER_MV * BSP_ADC_UV_SCALE);

  if (raw <= 0LL)
  {
    return 0U;
  }

  if (raw >= BSP_ADC_MAX_COUNTS)
  {
    return BSP_ADC_MAX_COUNTS;
  }

  return (uint16_t)raw;
}

void BSP_ADC_ImonCalibrateOffset(uint16_t raw,
                                 int32_t reference_milliamps)
{
  bsp_adc_imon_bias_uv_x10 =
      BSP_ADC_CountsToScaledMicrovolts(raw) -
      ((int64_t)reference_milliamps *
       BSP_ADC_IMON_SENSITIVITY_UV_X10_PER_MA);
}

static HAL_StatusTypeDef BSP_ADC_ReadRawSequence(ADC_HandleTypeDef *hadc,
                                                  uint32_t conversion_count,
                                                  uint16_t *raw_values)
{
  HAL_StatusTypeDef status = HAL_ADC_Start(hadc);
  uint32_t conversion;

  if (status != HAL_OK)
  {
    return status;
  }

  for (conversion = 0U; conversion < conversion_count; ++conversion)
  {
    status = HAL_ADC_PollForConversion(hadc, BSP_ADC_POLL_TIMEOUT_MS);
    if (status != HAL_OK)
    {
      break;
    }
    raw_values[conversion] = (uint16_t)HAL_ADC_GetValue(hadc);
  }

  {
    HAL_StatusTypeDef stop_status = HAL_ADC_Stop(hadc);

    if (status == HAL_OK)
    {
      status = stop_status;
    }
  }

  return status;
}

HAL_StatusTypeDef BSP_ADC_Init(void)
{
  HAL_StatusTypeDef status;

  status = HAL_OPAMP_Start(&hopamp2);
  if (status != HAL_OK)
  {
    return status;
  }

  status = HAL_OPAMP_Start(&hopamp3);
  if (status != HAL_OK)
  {
    return status;
  }

  status = HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  if (status != HAL_OK)
  {
    return status;
  }

  status = HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
  if (status != HAL_OK)
  {
    return status;
  }

  return HAL_OK;
}

HAL_StatusTypeDef BSP_ADC_ReadMeasurements(BSP_ADC_Sample *vin,
                                           BSP_ADC_Sample *vout,
                                           BSP_ADC_ImonSample *imon,
                                           BSP_NTC_Sample *ntc)
{
  HAL_StatusTypeDef status;
  uint16_t adc1_raw[BSP_ADC_ADC1_SEQUENCE_LENGTH];
  uint16_t adc2_raw[BSP_ADC_ADC2_SEQUENCE_LENGTH];

  status = BSP_ADC_ReadRawSequence(&hadc1, BSP_ADC_ADC1_SEQUENCE_LENGTH,
                                   adc1_raw);
  if (status != HAL_OK)
  {
    return status;
  }

  status = BSP_ADC_ReadRawSequence(&hadc2, BSP_ADC_ADC2_SEQUENCE_LENGTH,
                                   adc2_raw);
  if (status != HAL_OK)
  {
    return status;
  }

  imon->raw = adc2_raw[1];
  imon->milliamps = BSP_ADC_ImonRawToMilliamps(imon->raw);

  *ntc = BSP_NTC_RawToSample(adc1_raw[0]);

  vin->raw = adc1_raw[1];
  vin->node_millivolts = BSP_ADC_CountsToNodeMillivolts(vin->raw);
  vin->millivolts = BSP_ADC_NodeMillivoltsToSourceMillivolts(
      vin->node_millivolts, BSP_ADC_VIN_DIVIDER_TOP_OHMS,
      BSP_ADC_VIN_DIVIDER_BOTTOM_OHMS);

  vout->raw = adc2_raw[0];
  vout->node_millivolts = BSP_ADC_CountsToNodeMillivolts(vout->raw);
  vout->millivolts = BSP_ADC_NodeMillivoltsToSourceMillivolts(
      vout->node_millivolts, BSP_ADC_VOUT_DIVIDER_TOP_OHMS,
      BSP_ADC_VOUT_DIVIDER_BOTTOM_OHMS);

  return HAL_OK;
}

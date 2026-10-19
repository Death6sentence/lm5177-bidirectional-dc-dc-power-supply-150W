#include "bsp_ntc.h"

#include <math.h>

BSP_NTC_Sample BSP_NTC_RawToSample(uint16_t raw)
{
  BSP_NTC_Sample sample = {raw, 0, BSP_NTC_STATUS_INVALID};
  float ntc_resistance_ohms;
  float temperature_kelvin;
  float temperature_celsius;
  float temperature_deci_celsius;

  if (raw == 0U)
  {
    sample.status = BSP_NTC_STATUS_SHORT;
    return sample;
  }

  if (raw == BSP_NTC_ADC_MAX_COUNTS)
  {
    sample.status = BSP_NTC_STATUS_OPEN;
    return sample;
  }

  if (raw > BSP_NTC_ADC_MAX_COUNTS)
  {
    return sample;
  }

  ntc_resistance_ohms =
      (BSP_NTC_PULLUP_RESISTANCE_OHMS * (float)raw) /
      ((float)BSP_NTC_ADC_MAX_COUNTS - (float)raw);
  temperature_kelvin = 1.0f /
      ((1.0f / BSP_NTC_NOMINAL_TEMPERATURE_KELVIN) +
       (logf(ntc_resistance_ohms / BSP_NTC_NOMINAL_RESISTANCE_OHMS) /
        BSP_NTC_BETA_KELVIN));
  temperature_celsius = temperature_kelvin - 273.15f;
  temperature_deci_celsius = temperature_celsius * 10.0f;
  sample.temperature_deci_celsius = (int32_t)((temperature_deci_celsius >= 0.0f) ?
      (temperature_deci_celsius + 0.5f) :
      (temperature_deci_celsius - 0.5f));
  sample.status = BSP_NTC_STATUS_OK;
  return sample;
}

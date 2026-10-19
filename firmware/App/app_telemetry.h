#ifndef APP_TELEMETRY_H
#define APP_TELEMETRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef struct
{
  uint32_t timestamp_milliseconds;
  uint16_t imon_raw_adc_counts;
  int32_t imon_calculated_milliamps;
  uint8_t pid_active;
  uint16_t pid_target_raw_adc_counts;
  uint16_t pid_measured_raw_adc_counts;
  int32_t pid_error_raw_adc_counts;
  uint16_t dtrk_applied_control_level;
} App_TelemetryFrame;

HAL_StatusTypeDef App_Telemetry_Emit(const App_TelemetryFrame *frame);

#ifdef __cplusplus
}
#endif

#endif

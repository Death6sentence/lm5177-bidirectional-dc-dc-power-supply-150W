#include "app_telemetry.h"

#include <inttypes.h>
#include <stdio.h>

#include "usart.h"

#define APP_TELEMETRY_BUFFER_SIZE 192U
#define APP_TELEMETRY_UART_TIMEOUT_MS 20U

HAL_StatusTypeDef App_Telemetry_Emit(const App_TelemetryFrame *frame)
{
  char message[APP_TELEMETRY_BUFFER_SIZE];
  int message_length;

  message_length = snprintf(
      message, sizeof(message),
       "telemetry:%" PRIu32
       ",%" PRIu16
       ",%" PRId32
      ",%" PRIu8
      ",%" PRIu16
      ",%" PRIu16
      ",%" PRId32
      ",%" PRIu16 "\r\n",
       frame->timestamp_milliseconds,
       (unsigned int)frame->imon_raw_adc_counts,
       frame->imon_calculated_milliamps,
      (unsigned int)frame->pid_active,
      (unsigned int)frame->pid_target_raw_adc_counts,
      (unsigned int)frame->pid_measured_raw_adc_counts,
      frame->pid_error_raw_adc_counts,
       (unsigned int)frame->dtrk_applied_control_level);

  if ((message_length < 0) ||
      ((size_t)message_length >= sizeof(message)))
  {
    return HAL_ERROR;
  }

  return HAL_UART_Transmit(&huart1, (uint8_t *)message,
                           (uint16_t)message_length,
                           APP_TELEMETRY_UART_TIMEOUT_MS);
}

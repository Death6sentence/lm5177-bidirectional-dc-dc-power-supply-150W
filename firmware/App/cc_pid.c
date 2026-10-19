#include "cc_pid.h"

#define APP_CC_PID_OUTPUT_MIN_LEVEL 0U
#define APP_CC_PID_OUTPUT_MAX_LEVEL 1000U
#define APP_CC_PID_DEADBAND_COUNTS 2L
#define APP_CC_PID_FAR_ERROR_COUNTS 10L
#define APP_CC_PID_MAX_STEP_LEVEL 31U

static App_CcPidStatus pid_status;
static uint8_t active;
static uint16_t minimum_raw_counts;
static uint16_t maximum_raw_counts;
static uint16_t output_ceiling_level;
static uint16_t output_control_level;

static uint16_t App_CcPid_ClampTarget(uint16_t target_raw)
{
  if (target_raw < minimum_raw_counts)
  {
    return minimum_raw_counts;
  }

  if (target_raw > maximum_raw_counts)
  {
    return maximum_raw_counts;
  }

  return target_raw;
}

void App_CcPid_Init(void)
{
  pid_status.active = 0U;
  pid_status.target_raw = 0U;
  pid_status.measured_raw = 0U;
  pid_status.error_raw = 0L;
  active = 0U;
  minimum_raw_counts = 0U;
  maximum_raw_counts = 0U;
  output_ceiling_level = APP_CC_PID_OUTPUT_MIN_LEVEL;
  output_control_level = APP_CC_PID_OUTPUT_MIN_LEVEL;
}

void App_CcPid_Start(uint16_t dtrk_control_level, uint16_t minimum_raw,
                     uint16_t maximum_raw)
{
  if (dtrk_control_level > APP_CC_PID_OUTPUT_MAX_LEVEL)
  {
    dtrk_control_level = APP_CC_PID_OUTPUT_MAX_LEVEL;
  }

  minimum_raw_counts = minimum_raw;
  maximum_raw_counts = maximum_raw;
  output_ceiling_level = APP_CC_PID_OUTPUT_MAX_LEVEL;
  output_control_level = dtrk_control_level;
  pid_status.active = 1U;
  pid_status.target_raw = 0U;
  pid_status.measured_raw = 0U;
  pid_status.error_raw = 0L;
  active = 1U;
}

void App_CcPid_Stop(void)
{
  active = 0U;
  output_control_level = APP_CC_PID_OUTPUT_MIN_LEVEL;
  pid_status.active = 0U;
  pid_status.target_raw = 0U;
  pid_status.measured_raw = 0U;
  pid_status.error_raw = 0L;
}

uint8_t App_CcPid_IsActive(void)
{
  return active;
}

uint16_t App_CcPid_Update(const App_CcPidInput *input)
{
  uint16_t target_raw = input->target_raw;
  uint16_t measured_raw = input->measured_raw;
  int32_t error_raw_counts;
  uint32_t error_magnitude;
  uint16_t step_level = 0U;

  if (active == 0U)
  {
    return output_control_level;
  }

  target_raw = App_CcPid_ClampTarget(target_raw);
  error_raw_counts = (int32_t)target_raw - (int32_t)measured_raw;
  pid_status.active = 1U;
  pid_status.target_raw = target_raw;
  pid_status.measured_raw = measured_raw;
  pid_status.error_raw = error_raw_counts;

  if (error_raw_counts >= 0L)
  {
    error_magnitude = (uint32_t)error_raw_counts;
  }
  else
  {
    error_magnitude = (uint32_t)(-error_raw_counts);
  }

  if (error_magnitude > APP_CC_PID_DEADBAND_COUNTS)
  {
    step_level = (error_magnitude <= APP_CC_PID_FAR_ERROR_COUNTS) ?
                 (uint16_t)error_magnitude : APP_CC_PID_MAX_STEP_LEVEL;
  }

  if (error_raw_counts > APP_CC_PID_DEADBAND_COUNTS)
  {
    uint16_t available_level = output_ceiling_level - output_control_level;

    if (step_level >= available_level)
      output_control_level = output_ceiling_level;
    else output_control_level += step_level;
  }
  else if (error_raw_counts < -APP_CC_PID_DEADBAND_COUNTS)
  {
    if (step_level >= output_control_level)
      output_control_level = APP_CC_PID_OUTPUT_MIN_LEVEL;
    else output_control_level -= step_level;
  }

  return output_control_level;
}

App_CcPidStatus App_CcPid_GetStatus(void)
{
  return pid_status;
}

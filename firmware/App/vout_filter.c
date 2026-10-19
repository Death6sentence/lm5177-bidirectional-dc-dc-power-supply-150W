#include "vout_filter.h"

#define VOUT_FILTER_IIR_ALPHA 0.5f
#define VOUT_FILTER_KALMAN_INITIAL_COVARIANCE_MV2 100.0f
#define VOUT_FILTER_KALMAN_PROCESS_NOISE_MV2 50.0f
#define VOUT_FILTER_KALMAN_MEASUREMENT_NOISE_MV2 100.0f

static uint16_t VoutFilter_ToMillivolts(float32_t estimate_millivolts)
{
  if (estimate_millivolts <= 0.0f)
  {
    return 0U;
  }

  if (estimate_millivolts >= (float32_t)UINT16_MAX)
  {
    return UINT16_MAX;
  }

  return (uint16_t)(estimate_millivolts + 0.5f);
}

void VoutFilter_Init(VoutFilter *filter)
{
  filter->iir_coefficients[0] = VOUT_FILTER_IIR_ALPHA;
  filter->iir_coefficients[1] = 0.0f;
  filter->iir_coefficients[2] = 0.0f;
  filter->iir_coefficients[3] = 1.0f - VOUT_FILTER_IIR_ALPHA;
  filter->iir_coefficients[4] = 0.0f;

  arm_biquad_cascade_df2T_init_f32(&filter->iir, 1U,
                                   filter->iir_coefficients,
                                   filter->iir_state);
  filter->kalman_estimate_millivolts = 0.0f;
  filter->kalman_covariance_millivolts2 =
      VOUT_FILTER_KALMAN_INITIAL_COVARIANCE_MV2;
  filter->initialized = 0U;
}

void VoutFilter_Reset(VoutFilter *filter, uint16_t initial_millivolts)
{
  float32_t initial = (float32_t)initial_millivolts;

  filter->iir_state[0] = filter->iir_coefficients[3] * initial;
  filter->iir_state[1] = 0.0f;
  filter->kalman_estimate_millivolts = initial;
  filter->kalman_covariance_millivolts2 =
      VOUT_FILTER_KALMAN_INITIAL_COVARIANCE_MV2;
  filter->initialized = 1U;
}

uint16_t VoutFilter_Update(VoutFilter *filter, uint16_t raw_millivolts)
{
  float32_t input = (float32_t)raw_millivolts;
  float32_t iir_output;
  float32_t predicted_covariance;
  float32_t kalman_gain;

  if (filter->initialized == 0U)
  {
    VoutFilter_Reset(filter, raw_millivolts);
    return raw_millivolts;
  }

  arm_biquad_cascade_df2T_f32(&filter->iir, &input, &iir_output, 1U);

  predicted_covariance =
      filter->kalman_covariance_millivolts2 +
      VOUT_FILTER_KALMAN_PROCESS_NOISE_MV2;
  kalman_gain = predicted_covariance /
                (predicted_covariance +
                 VOUT_FILTER_KALMAN_MEASUREMENT_NOISE_MV2);
  filter->kalman_estimate_millivolts +=
      kalman_gain * (iir_output - filter->kalman_estimate_millivolts);
  filter->kalman_covariance_millivolts2 =
      (1.0f - kalman_gain) * predicted_covariance;

  return VoutFilter_ToMillivolts(filter->kalman_estimate_millivolts);
}

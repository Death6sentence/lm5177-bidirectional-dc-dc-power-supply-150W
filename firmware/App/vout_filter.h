#ifndef VOUT_FILTER_H
#define VOUT_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "arm_math.h"

typedef struct
{
  arm_biquad_cascade_df2T_instance_f32 iir;
  float32_t iir_coefficients[5U];
  float32_t iir_state[2U];
  float32_t kalman_estimate_millivolts;
  float32_t kalman_covariance_millivolts2;
  uint8_t initialized;
} VoutFilter;

void VoutFilter_Init(VoutFilter *filter);
void VoutFilter_Reset(VoutFilter *filter, uint16_t initial_millivolts);
uint16_t VoutFilter_Update(VoutFilter *filter, uint16_t raw_millivolts);

#ifdef __cplusplus
}
#endif

#endif

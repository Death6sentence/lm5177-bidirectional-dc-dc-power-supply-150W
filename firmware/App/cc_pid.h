#ifndef CC_PID_H
#define CC_PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
  uint16_t target_raw;
  uint16_t measured_raw;
} App_CcPidInput;

typedef struct
{
  uint8_t active;
  uint16_t target_raw;
  uint16_t measured_raw;
  int32_t error_raw;
} App_CcPidStatus;

void App_CcPid_Init(void);
void App_CcPid_Start(uint16_t dtrk_control_level, uint16_t minimum_raw,
                     uint16_t maximum_raw);
void App_CcPid_Stop(void);
uint8_t App_CcPid_IsActive(void);
uint16_t App_CcPid_Update(const App_CcPidInput *input);
App_CcPidStatus App_CcPid_GetStatus(void);

#ifdef __cplusplus
}
#endif

#endif

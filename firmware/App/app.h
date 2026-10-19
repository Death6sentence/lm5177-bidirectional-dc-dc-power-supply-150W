#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/** 初始化所有板级驱动和应用状态。 */
HAL_StatusTypeDef App_Init(void);

/** 执行一次非阻塞应用调度，应在主循环中持续调用。 */
HAL_StatusTypeDef App_Process(void);

/** 由 20 kHz 采样中断调用，推进正弦波形。 */
void App_SinSampleElapsed(void);

#ifdef __cplusplus
}
#endif

#endif

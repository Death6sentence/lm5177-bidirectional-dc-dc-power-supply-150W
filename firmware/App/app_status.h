#ifndef APP_STATUS_H
#define APP_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_menu.h"
#include "bsp_adc.h"
#include "bsp_ntc.h"

HAL_StatusTypeDef App_Status_Init(void);
HAL_StatusTypeDef App_Status_ShowAdcError(void);
HAL_StatusTypeDef App_Status_ShowPage(
    App_MenuPage page, App_MenuItem selected_item,
    const BSP_ADC_Sample *vin, const BSP_ADC_Sample *vout,
    uint16_t vout_display_millivolts, const BSP_ADC_ImonSample *imon,
    const BSP_NTC_Sample *ntc,
    uint8_t fan_duty_percent, uint8_t power_enabled,
    int32_t imon_milliamps, uint16_t vout_target_millivolts,
    uint16_t dtrk_target_control_level, int32_t cc_max_milliamps,
    int32_t cc_target_milliamps,
    int32_t imon_calibration_reference_milliamps,
    uint16_t dtrk_applied_control_level);
HAL_StatusTypeDef App_Status_Show(const BSP_ADC_Sample *vin,
                                  const BSP_ADC_Sample *vout,
                                   uint16_t vout_display_millivolts,
                                   const BSP_ADC_ImonSample *imon,
                                   const BSP_NTC_Sample *ntc,
                                   uint8_t fan_duty_percent,
                                   int32_t imon_milliamps,
                                  uint16_t dtrk_target_control_level);

#ifdef __cplusplus
}
#endif

#endif

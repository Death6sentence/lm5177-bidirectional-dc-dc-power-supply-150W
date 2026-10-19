#include "app.h"

#include "cc_pid.h"
#include "app_telemetry.h"
#include "app_menu.h"
#include "app_status.h"
#include "bsp_adc.h"
#include "bsp_button.h"
#include "bsp_dac.h"
#include "bsp_encoder.h"
#include "bsp_fan.h"
#include "bsp_gpio.h"
#include "bsp_pwm.h"
#include "vout_filter.h"

#define APP_STATUS_REFRESH_MS 100U
#define APP_CC_CONTROL_PERIOD_MS 10U
#define APP_LED_BLINK_HALF_PERIOD_MS 500U
#define APP_DTRK_IDLE_LEVEL 0U
#define APP_DTRK_CONTROL_LEVEL_MAX 1000U
#define APP_DTRK_DEFAULT_LEVEL APP_DTRK_CONTROL_LEVEL_MAX
#define APP_VOLTAGE_PROTECTION_ENABLED 1U
#define APP_DTRK_SOFT_START_MS 100U
#define APP_OVERCURRENT_LIMIT_MA 5000L
#define APP_VIN_MIN_MV 8000U
#define APP_VIN_SATURATION_MV 30000U
#define APP_VOUT_SATURATION_MV 28000U
#define APP_SIN_SAMPLE_COUNT 400U
#define APP_SIN_DTRK_SCALE 400U
#define APP_DAC_DEFAULT_MV 1100U
#define APP_IMON_DAC_CAL_MV 0U
#define APP_IMON_ZERO_CAL_SETTLE_MS 100U
#define APP_IMON_ZERO_CAL_SAMPLE_COUNT 16U
#define APP_POWER_ENABLE_DEFAULT 1U
#define APP_ANALOG_SIN_DAC_FULL_SCALE_MV 1000U
#define APP_FAN_DEFAULT_DUTY_PERCENT 0U

typedef enum
{
  APP_SIN_OUTPUT_NONE = 0U,
  APP_SIN_OUTPUT_DTRK,
  APP_SIN_OUTPUT_DAC
} App_SinOutput;

static const uint16_t app_sin_dtrk_scaled_compare[APP_SIN_SAMPLE_COUNT] =
{
  250U, 252U, 254U, 256U, 258U, 260U, 262U, 264U, 266U, 268U,
  270U, 271U, 273U, 275U, 277U, 279U, 281U, 283U, 285U, 287U,
  289U, 290U, 292U, 294U, 296U, 298U, 300U, 301U, 303U, 305U,
  307U, 308U, 310U, 312U, 314U, 315U, 317U, 319U, 320U, 322U,
  323U, 325U, 327U, 328U, 330U, 331U, 333U, 334U, 336U, 337U,
  338U, 340U, 341U, 342U, 344U, 345U, 346U, 348U, 349U, 350U,
  351U, 352U, 353U, 354U, 356U, 357U, 358U, 359U, 360U, 360U,
  361U, 362U, 363U, 364U, 365U, 365U, 366U, 367U, 368U, 368U,
  369U, 369U, 370U, 371U, 371U, 372U, 372U, 372U, 373U, 373U,
  373U, 374U, 374U, 374U, 374U, 375U, 375U, 375U, 375U, 375U,
  375U, 375U, 375U, 375U, 375U, 375U, 374U, 374U, 374U, 374U,
  373U, 373U, 373U, 372U, 372U, 372U, 371U, 371U, 370U, 369U,
  369U, 368U, 368U, 367U, 366U, 365U, 365U, 364U, 363U, 362U,
  361U, 360U, 360U, 359U, 358U, 357U, 356U, 354U, 353U, 352U,
  351U, 350U, 349U, 348U, 346U, 345U, 344U, 342U, 341U, 340U,
  338U, 337U, 336U, 334U, 333U, 331U, 330U, 328U, 327U, 325U,
  323U, 322U, 320U, 319U, 317U, 315U, 314U, 312U, 310U, 308U,
  307U, 305U, 303U, 301U, 300U, 298U, 296U, 294U, 292U, 290U,
  289U, 287U, 285U, 283U, 281U, 279U, 277U, 275U, 273U, 271U,
  270U, 268U, 266U, 264U, 262U, 260U, 258U, 256U, 254U, 252U,
  250U, 248U, 246U, 244U, 242U, 240U, 238U, 236U, 234U, 232U,
  230U, 229U, 227U, 225U, 223U, 221U, 219U, 217U, 215U, 213U,
  211U, 210U, 208U, 206U, 204U, 202U, 200U, 199U, 197U, 195U,
  193U, 192U, 190U, 188U, 186U, 185U, 183U, 181U, 180U, 178U,
  177U, 175U, 173U, 172U, 170U, 169U, 167U, 166U, 164U, 163U,
  162U, 160U, 159U, 158U, 156U, 155U, 154U, 152U, 151U, 150U,
  149U, 148U, 147U, 146U, 144U, 143U, 142U, 141U, 140U, 140U,
  139U, 138U, 137U, 136U, 135U, 135U, 134U, 133U, 132U, 132U,
  131U, 131U, 130U, 129U, 129U, 128U, 128U, 128U, 127U, 127U,
  127U, 126U, 126U, 126U, 126U, 125U, 125U, 125U, 125U, 125U,
  125U, 125U, 125U, 125U, 125U, 125U, 126U, 126U, 126U, 126U,
  127U, 127U, 127U, 128U, 128U, 128U, 129U, 129U, 130U, 131U,
  131U, 132U, 132U, 133U, 134U, 135U, 135U, 136U, 137U, 138U,
  139U, 140U, 140U, 141U, 142U, 143U, 144U, 146U, 147U, 148U,
  149U, 150U, 151U, 152U, 154U, 155U, 156U, 158U, 159U, 160U,
  162U, 163U, 164U, 166U, 167U, 169U, 170U, 172U, 173U, 175U,
  177U, 178U, 180U, 181U, 183U, 185U, 186U, 188U, 190U, 192U,
  193U, 195U, 197U, 199U, 200U, 202U, 204U, 206U, 208U, 210U,
  211U, 213U, 215U, 217U, 219U, 221U, 223U, 225U, 227U, 229U,
  230U, 232U, 234U, 236U, 238U, 240U, 242U, 244U, 246U, 248U
};

static BSP_ADC_Sample vin_sample;
static BSP_ADC_Sample vout_sample;
static BSP_ADC_ImonSample imon_sample;
static BSP_NTC_Sample ntc_sample;
static BSP_ADC_Sample protection_vin_sample;
static BSP_ADC_Sample protection_vout_sample;
static BSP_ADC_ImonSample protection_imon_sample;
static BSP_NTC_Sample protection_ntc_sample;
static VoutFilter vout_filter;
static uint16_t vout_display_millivolts;
static uint16_t protection_vout_display_millivolts;
static uint32_t status_refresh_tick;
static uint32_t cc_control_tick;
static uint32_t led_blink_tick;
static uint8_t power_stage_enabled;
static uint8_t overcurrent_latched;
static uint8_t voltage_protection_latched;
static uint8_t protection_snapshot_valid;
static BSP_OutputState led_blink_state;
static uint16_t dtrk_target_control_level;
static volatile uint16_t dtrk_applied_control_level;
static uint16_t sin_restore_control_level;
static volatile App_SinOutput sin_output;
static volatile uint16_t sin_sample_index;
static volatile uint16_t sin_dither_accumulator;
static App_MenuState menu_state;

static void App_TripOvercurrentProtection(void);
static void App_TripVoltageProtection(void);

static uint16_t App_SinDitheredCompare(uint16_t scaled_compare)
{
  uint32_t scaled_total =
      ((uint32_t)scaled_compare * BSP_PWM_DTRK_COMPARE_MAX) +
      sin_dither_accumulator;
  uint16_t compare = (uint16_t)(scaled_total / APP_SIN_DTRK_SCALE);

  sin_dither_accumulator = (uint16_t)(scaled_total % APP_SIN_DTRK_SCALE);
  return compare;
}

static uint16_t App_SinControlLevel(uint16_t scaled_compare)
{
  return (uint16_t)(((uint32_t)scaled_compare *
                     APP_DTRK_CONTROL_LEVEL_MAX +
                     (APP_SIN_DTRK_SCALE / 2U)) /
                    APP_SIN_DTRK_SCALE);
}

static uint16_t App_AnalogSinDacMillivolts(uint16_t scaled_compare)
{
  return (uint16_t)(((uint32_t)scaled_compare *
                     APP_ANALOG_SIN_DAC_FULL_SCALE_MV +
                     (APP_SIN_DTRK_SCALE / 2U)) /
                    APP_SIN_DTRK_SCALE);
}

static void App_StopActiveModeAndReturnMain(uint8_t restore_dac)
{
  App_CcPid_Stop();
  BSP_PWM_SinSampleInterruptDisable();
  sin_output = APP_SIN_OUTPUT_NONE;
  if (restore_dac != 0U)
  {
    (void)BSP_DAC_SetMillivolts(APP_DAC_DEFAULT_MV);
  }
  menu_state.page = APP_MENU_PAGE_MAIN;
}

static HAL_StatusTypeDef App_StartSinMode(void)
{
  uint16_t first_scaled_compare = app_sin_dtrk_scaled_compare[0];
  HAL_StatusTypeDef status;

  BSP_PWM_SinSampleInterruptDisable();
  sin_output = APP_SIN_OUTPUT_NONE;
  status = BSP_DAC_SetMillivolts(APP_DAC_DEFAULT_MV);
  if (status != HAL_OK)
  {
    return status;
  }

  sin_restore_control_level = dtrk_target_control_level;
  sin_sample_index = 1U;
  sin_dither_accumulator = 0U;
  BSP_PWM_DtrkSetCompare(App_SinDitheredCompare(first_scaled_compare));
  dtrk_applied_control_level = App_SinControlLevel(first_scaled_compare);
  sin_output = APP_SIN_OUTPUT_DTRK;
  BSP_PWM_SinSampleInterruptEnable();
  return HAL_OK;
}

static HAL_StatusTypeDef App_StartAnalogSinMode(void)
{
  uint16_t first_scaled_compare = app_sin_dtrk_scaled_compare[0];
  HAL_StatusTypeDef status;

  BSP_PWM_SinSampleInterruptDisable();
  sin_output = APP_SIN_OUTPUT_NONE;
  sin_restore_control_level = dtrk_target_control_level;
  sin_sample_index = 1U;
  sin_dither_accumulator = 0U;

  status = BSP_PWM_DtrkSetControlLevel(APP_DTRK_CONTROL_LEVEL_MAX);
  if (status != HAL_OK)
  {
    return status;
  }

  status = BSP_DAC_SetMillivolts(
      App_AnalogSinDacMillivolts(first_scaled_compare));
  if (status != HAL_OK)
  {
    (void)BSP_PWM_DtrkSetControlLevel(sin_restore_control_level);
    dtrk_applied_control_level = sin_restore_control_level;
    return status;
  }

  dtrk_applied_control_level = APP_DTRK_CONTROL_LEVEL_MAX;
  sin_output = APP_SIN_OUTPUT_DAC;
  BSP_PWM_SinSampleInterruptEnable();
  return HAL_OK;
}

static HAL_StatusTypeDef App_StopSinModeAndRestore(void)
{
  HAL_StatusTypeDef dac_status;
  HAL_StatusTypeDef dtrk_status;

  BSP_PWM_SinSampleInterruptDisable();
  sin_output = APP_SIN_OUTPUT_NONE;
  dac_status = BSP_DAC_SetMillivolts(APP_DAC_DEFAULT_MV);
  dtrk_status = BSP_PWM_DtrkSetControlLevel(sin_restore_control_level);
  if (dtrk_status == HAL_OK)
  {
    dtrk_target_control_level = sin_restore_control_level;
    dtrk_applied_control_level = sin_restore_control_level;
  }

  return (dac_status != HAL_OK) ? dac_status : dtrk_status;
}

static HAL_StatusTypeDef App_ReadMeasurements(void)
{
  HAL_StatusTypeDef status = BSP_ADC_ReadMeasurements(&vin_sample,
                                                      &vout_sample,
                                                      &imon_sample,
                                                      &ntc_sample);
  if (status == HAL_OK)
  {
    vout_display_millivolts = vout_sample.millivolts;
  }

  return status;
}

static HAL_StatusTypeDef App_CalibrateImonZero(void)
{
  HAL_StatusTypeDef status;
  uint32_t imon_raw_sum = 0U;
  uint16_t imon_zero_raw;
  uint32_t sample_index;

  HAL_Delay(APP_IMON_ZERO_CAL_SETTLE_MS);
  for (sample_index = 0U;
       sample_index < APP_IMON_ZERO_CAL_SAMPLE_COUNT;
       sample_index++)
  {
    status = App_ReadMeasurements();
    if (status != HAL_OK)
    {
      return status;
    }
    imon_raw_sum += imon_sample.raw;
  }

  imon_zero_raw = (uint16_t)(
      (imon_raw_sum + (APP_IMON_ZERO_CAL_SAMPLE_COUNT / 2U)) /
      APP_IMON_ZERO_CAL_SAMPLE_COUNT);
  BSP_ADC_ImonCalibrateOffset(imon_zero_raw, 0L);
  imon_sample.raw = imon_zero_raw;
  imon_sample.milliamps = BSP_ADC_ImonRawToMilliamps(imon_zero_raw);
  return HAL_OK;
}

static HAL_StatusTypeDef App_AutoCalibrateImon(void)
{
  BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
  power_stage_enabled = 0U;
  return App_CalibrateImonZero();
}

static HAL_StatusTypeDef App_DacCalibrateImon(void)
{
  HAL_StatusTypeDef status =
      BSP_DAC_SetMillivolts(APP_IMON_DAC_CAL_MV);

  return (status == HAL_OK) ? App_CalibrateImonZero() : status;
}

static void App_CaptureProtectionSnapshotOnce(void)
{
  if (protection_snapshot_valid != 0U)
  {
    return;
  }

  /* 仅保留首次保护故障采样，避免后续刷新覆盖故障现场。 */
  protection_vin_sample = vin_sample;
  protection_vout_sample = vout_sample;
  protection_imon_sample = imon_sample;
  protection_ntc_sample = ntc_sample;
  protection_vout_display_millivolts = vout_display_millivolts;
  protection_snapshot_valid = 1U;
}

static HAL_StatusTypeDef App_ShowStatus(void)
{
  if (protection_snapshot_valid != 0U)
  {
    return App_Status_ShowPage(APP_MENU_PAGE_RUN_INFO,
                                APP_MENU_ITEM_RUN_INFO,
                                &protection_vin_sample,
                                &protection_vout_sample,
                                protection_vout_display_millivolts,
                                &protection_imon_sample,
                                &protection_ntc_sample,
                                menu_state.fan_duty_percent,
                                menu_state.power_enabled,
                                protection_imon_sample.milliamps,
                                menu_state.vout_target_millivolts,
                                dtrk_target_control_level,
                                menu_state.cc_max_milliamps,
                                menu_state.cc_target_milliamps,
                                menu_state.imon_calibration_reference_milliamps,
                                dtrk_applied_control_level);
  }

  return App_Status_ShowPage(menu_state.page, menu_state.selected_item,
                             &vin_sample, &vout_sample,
                             vout_display_millivolts, &imon_sample,
                              &ntc_sample,
                              menu_state.fan_duty_percent,
                              menu_state.power_enabled,
                              imon_sample.milliamps,
                              menu_state.vout_target_millivolts,
                              dtrk_target_control_level,
                              menu_state.cc_max_milliamps,
                              menu_state.cc_target_milliamps,
                              menu_state.imon_calibration_reference_milliamps,
                              dtrk_applied_control_level);
}

static void App_TryShowStatus(void)
{
  (void)App_ShowStatus();
}

static uint8_t App_IsProtectionLatched(void)
{
  return (uint8_t)((voltage_protection_latched != 0U) ||
                   (overcurrent_latched != 0U));
}

static HAL_StatusTypeDef App_ProcessMenu(int32_t encoder_delta,
                                          BSP_ButtonEvent button_event,
                                          uint8_t *display_immediately)
{
  App_MenuAction action;
  HAL_StatusTypeDef status;
  int32_t fresh_imon_milliamps = imon_sample.milliamps;

  if ((button_event == BSP_BUTTON_EVENT_PRESSED) &&
      (menu_state.page == APP_MENU_PAGE_MAIN) &&
      ((menu_state.selected_item == APP_MENU_ITEM_CC_MODE) ||
       (menu_state.selected_item == APP_MENU_ITEM_CAL_IMON)))
  {
    status = App_ReadMeasurements();
    if (status != HAL_OK)
    {
      App_TripVoltageProtection();
      (void)App_Status_ShowAdcError();
      return status;
    }
    fresh_imon_milliamps = imon_sample.milliamps;
  }

  App_Menu_Update(&menu_state, encoder_delta,
                  (button_event == BSP_BUTTON_EVENT_PRESSED) ? 1U : 0U,
                  fresh_imon_milliamps,
                  &action);
  *display_immediately = (uint8_t)((action.page_changed != 0U) ||
                                   (action.selection_changed != 0U) ||
                                   (action.cc_target_changed != 0U) ||
                                   (action.fan_duty_changed != 0U) ||
                                   (action.power_enabled_changed != 0U) ||
                                   (action.imon_calibration_reference_changed !=
                                    0U));

  if (action.start_cc != 0U)
  {
    cc_control_tick = HAL_GetTick();
    App_CcPid_Start(
        dtrk_applied_control_level,
        BSP_ADC_ImonMilliampsToRaw(APP_MENU_CC_TARGET_MIN_MA),
        BSP_ADC_ImonMilliampsToRaw(APP_MENU_CC_TARGET_MAX_MA));
  }

  if (action.stop_cc != 0U)
  {
    App_CcPid_Stop();
    dtrk_target_control_level = dtrk_applied_control_level;
    menu_state.vout_target_millivolts =
        App_Menu_VoutMillivoltsFromDtrk(dtrk_applied_control_level);
  }

  if (action.start_sin != 0U)
  {
    status = App_StartSinMode();
    if (status != HAL_OK)
    {
      App_StopActiveModeAndReturnMain(1U);
      BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
      power_stage_enabled = 0U;
      return status;
    }
  }

  if (action.start_analog_sin != 0U)
  {
    status = App_StartAnalogSinMode();
    if (status != HAL_OK)
    {
      App_StopActiveModeAndReturnMain(1U);
      BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
      power_stage_enabled = 0U;
      return status;
    }
  }

  if ((action.stop_sin != 0U) || (action.stop_analog_sin != 0U))
  {
    status = App_StopSinModeAndRestore();
    if (status != HAL_OK)
    {
      App_StopActiveModeAndReturnMain(1U);
      BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
      power_stage_enabled = 0U;
      return status;
    }
  }

  if (action.apply_imon_calibration != 0U)
  {
    status = App_ReadMeasurements();
    if (status != HAL_OK)
    {
      App_TripVoltageProtection();
      (void)App_Status_ShowAdcError();
      return status;
    }

    BSP_ADC_ImonCalibrateOffset(
        imon_sample.raw,
        action.imon_calibration_reference_milliamps);
    imon_sample.milliamps =
        BSP_ADC_ImonRawToMilliamps(imon_sample.raw);
  }

  if (action.start_auto_imon_calibration != 0U)
  {
    status = App_AutoCalibrateImon();
    if (status != HAL_OK)
    {
      menu_state.page = APP_MENU_PAGE_MAIN;
      BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
      power_stage_enabled = 0U;
      menu_state.power_enabled = 0U;
      return status;
    }
  }

  if (action.stop_auto_imon_calibration != 0U)
  {
    BSP_GPIO_EnableWrite((menu_state.power_enabled != 0U) ?
                         BSP_GPIO_HIGH : BSP_GPIO_LOW);
    power_stage_enabled = menu_state.power_enabled;
  }

  if (action.start_dac_imon_calibration != 0U)
  {
    status = App_DacCalibrateImon();
    if (status != HAL_OK)
    {
      App_StopActiveModeAndReturnMain(1U);
      BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
      power_stage_enabled = 0U;
      menu_state.power_enabled = 0U;
      return status;
    }
  }

  if (action.stop_dac_imon_calibration != 0U)
  {
    status = BSP_DAC_SetMillivolts(APP_DAC_DEFAULT_MV);
    if (status != HAL_OK)
    {
      BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
      power_stage_enabled = 0U;
      menu_state.power_enabled = 0U;
      return status;
    }
  }

  if (action.apply_power_enable != 0U)
  {
    BSP_GPIO_EnableWrite((action.power_enabled != 0U) ?
                         BSP_GPIO_HIGH : BSP_GPIO_LOW);
    power_stage_enabled = action.power_enabled;
  }

  if (action.fan_duty_changed != 0U)
  {
    status = BSP_Fan_SetDutyPercent(action.fan_duty_percent);
    if (status != HAL_OK)
    {
      return status;
    }
  }

  if (action.dtrk_changed == 0U)
  {
    return HAL_OK;
  }

  dtrk_target_control_level = action.dtrk_target_control_level;

  if ((power_stage_enabled != 0U) &&
      (dtrk_target_control_level != dtrk_applied_control_level))
  {
    status = BSP_PWM_DtrkSetControlLevel(dtrk_target_control_level);

    if (status != HAL_OK)
    {
      App_StopActiveModeAndReturnMain(1U);
      BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
      power_stage_enabled = 0U;
      return status;
    }
    dtrk_applied_control_level = dtrk_target_control_level;
  }

  return HAL_OK;
}

static void App_ResetLedBlinkPhase(void)
{
  led_blink_state = BSP_OUTPUT_ON;
  led_blink_tick = HAL_GetTick();
}

static void App_UpdateLedStatus(void)
{
  BSP_GPIO_Status gpio;
  BSP_OutputState blue_state;
  BSP_OutputState red_state;
  BSP_OutputState yellow_state;
  uint32_t now = HAL_GetTick();

  BSP_GPIO_ReadStatus(&gpio);

  if ((now - led_blink_tick) >= APP_LED_BLINK_HALF_PERIOD_MS)
  {
    led_blink_state = (led_blink_state == BSP_OUTPUT_ON) ?
        BSP_OUTPUT_OFF : BSP_OUTPUT_ON;
    led_blink_tick = now;
  }

  blue_state = (gpio.enable == BSP_GPIO_LOW) ?
               BSP_OUTPUT_ON : led_blink_state;
  red_state = BSP_OUTPUT_OFF;
  yellow_state = BSP_OUTPUT_OFF;

  if (voltage_protection_latched != 0U)
  {
    red_state = led_blink_state;
  }
  else if (overcurrent_latched != 0U)
  {
    yellow_state = led_blink_state;
  }

  BSP_GPIO_LedBlueWrite(blue_state);
  BSP_GPIO_LedRedWrite(red_state);
  BSP_GPIO_LedYellowWrite(yellow_state);
}

static uint8_t App_VoltageProtectionFaulted(void)
{
  return (uint8_t)((vin_sample.millivolts < APP_VIN_MIN_MV) ||
                   (vin_sample.millivolts >= APP_VIN_SATURATION_MV) ||
                   (vout_sample.millivolts >= APP_VOUT_SATURATION_MV));
}

static uint8_t App_IsVoltageProtectionActive(void)
{
  return (uint8_t)((APP_VOLTAGE_PROTECTION_ENABLED != 0U) &&
                   (App_VoltageProtectionFaulted() != 0U));
}

static void App_TripVoltageProtection(void)
{
  uint8_t was_latched = voltage_protection_latched;

  App_StopActiveModeAndReturnMain(1U);
  BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
  (void)BSP_PWM_DtrkSetControlLevel(APP_DTRK_IDLE_LEVEL);
  power_stage_enabled = 0U;
  voltage_protection_latched = 1U;

  if (was_latched == 0U)
  {
    App_ResetLedBlinkPhase();
  }
  /* 电压跳闸优先显示红灯，覆盖过流黄灯。 */
  App_UpdateLedStatus();
}

static HAL_StatusTypeDef App_ProcessStartRequest(BSP_ButtonEvent button_event,
                                                 uint8_t *status_already_shown)
{
  HAL_StatusTypeDef status;

  *status_already_shown = 0U;
  if ((button_event != BSP_BUTTON_EVENT_PRESSED) ||
      (power_stage_enabled != 0U) ||
      (App_IsProtectionLatched() == 0U))
  {
    return HAL_OK;
  }

  App_StopActiveModeAndReturnMain(1U);

  status = App_ReadMeasurements();
  if (status != HAL_OK)
  {
    App_TripVoltageProtection();
    return status;
  }

  if (App_IsVoltageProtectionActive() != 0U)
  {
    App_CaptureProtectionSnapshotOnce();
    App_TripVoltageProtection();
    *status_already_shown = 1U;
    App_TryShowStatus();
    return HAL_OK;
  }

  status = BSP_PWM_DtrkSetControlLevel(APP_DTRK_IDLE_LEVEL);
  if (status != HAL_OK)
  {
    BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
    return status;
  }

  BSP_GPIO_EnableWrite(BSP_GPIO_HIGH);
  status = BSP_PWM_DtrkSoftStart(dtrk_target_control_level,
                                 APP_DTRK_SOFT_START_MS);
  if (status != HAL_OK)
  {
    BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
    return status;
  }
  power_stage_enabled = 1U;

  status = App_ReadMeasurements();
  if (status != HAL_OK)
  {
    BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
    (void)BSP_PWM_DtrkSetControlLevel(APP_DTRK_IDLE_LEVEL);
    power_stage_enabled = 0U;
    return status;
  }

  if ((imon_sample.milliamps >= APP_OVERCURRENT_LIMIT_MA) ||
      (imon_sample.milliamps <= -APP_OVERCURRENT_LIMIT_MA))
  {
    App_TripOvercurrentProtection();
    *status_already_shown = 1U;
    App_TryShowStatus();
    return HAL_OK;
  }

  dtrk_applied_control_level = dtrk_target_control_level;
  overcurrent_latched = 0U;
  voltage_protection_latched = 0U;
  protection_snapshot_valid = 0U;

  return HAL_OK;
}

static void App_TripOvercurrentProtection(void)
{
  App_CaptureProtectionSnapshotOnce();
  App_StopActiveModeAndReturnMain(1U);
  BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
  (void)BSP_PWM_DtrkSetControlLevel(APP_DTRK_IDLE_LEVEL);
  power_stage_enabled = 0U;
  overcurrent_latched = 1U;
  App_ResetLedBlinkPhase();
}

static uint8_t App_IsCcActiveAndHealthy(void)
{
  return (uint8_t)((menu_state.page == APP_MENU_PAGE_CC_ACTIVE) &&
                   (App_CcPid_IsActive() != 0U) &&
                   (power_stage_enabled != 0U) &&
                   (App_IsProtectionLatched() == 0U));
}

static HAL_StatusTypeDef App_ProcessCcControlIfDue(void)
{
  App_CcPidInput input;
  HAL_StatusTypeDef status;
  uint16_t cc_dtrk_control_level;
  uint32_t now = HAL_GetTick();

  if ((App_IsCcActiveAndHealthy() == 0U) ||
      ((now - cc_control_tick) < APP_CC_CONTROL_PERIOD_MS))
  {
    return HAL_OK;
  }

  cc_control_tick = now;
  status = App_ReadMeasurements();
  if (status != HAL_OK)
  {
    App_StopActiveModeAndReturnMain(1U);
    App_TripVoltageProtection();
    return status;
  }

  if ((imon_sample.milliamps >= APP_OVERCURRENT_LIMIT_MA) ||
      (imon_sample.milliamps <= -APP_OVERCURRENT_LIMIT_MA))
  {
    App_TripOvercurrentProtection();
    return HAL_OK;
  }

  if (App_IsVoltageProtectionActive() != 0U)
  {
    App_TripVoltageProtection();
    return HAL_OK;
  }

  input.target_raw =
      BSP_ADC_ImonMilliampsToRaw(menu_state.cc_target_milliamps);
  input.measured_raw = imon_sample.raw;
  cc_dtrk_control_level = App_CcPid_Update(&input);
  if (cc_dtrk_control_level == dtrk_applied_control_level)
  {
    return HAL_OK;
  }

  status = BSP_PWM_DtrkSetControlLevel(cc_dtrk_control_level);
  if (status != HAL_OK)
  {
    App_StopActiveModeAndReturnMain(1U);
    BSP_GPIO_EnableWrite(BSP_GPIO_LOW);
    power_stage_enabled = 0U;
    return status;
  }

  dtrk_applied_control_level = cc_dtrk_control_level;
  dtrk_target_control_level = cc_dtrk_control_level;
  return HAL_OK;
}

static HAL_StatusTypeDef App_RefreshStatusIfDue(uint8_t display_immediately)
{
  uint32_t now = HAL_GetTick();
  uint8_t periodic_refresh_due;
  HAL_StatusTypeDef status;

  periodic_refresh_due = (uint8_t)((now - status_refresh_tick) >=
                                   APP_STATUS_REFRESH_MS);
  if ((display_immediately == 0U) && (periodic_refresh_due == 0U))
  {
    return HAL_OK;
  }

  status_refresh_tick = now;
  if (App_IsCcActiveAndHealthy() == 0U)
  {
    status = App_ReadMeasurements();
    if (status != HAL_OK)
    {
      App_StopActiveModeAndReturnMain(
          (menu_state.page == APP_MENU_PAGE_AUTO_IMON_CALIBRATION) ?
          0U : 1U);
      App_TripVoltageProtection();
      return status;
    }

    if ((power_stage_enabled != 0U) &&
        ((imon_sample.milliamps >= APP_OVERCURRENT_LIMIT_MA) ||
         (imon_sample.milliamps <= -APP_OVERCURRENT_LIMIT_MA)))
    {
      App_TripOvercurrentProtection();
    }

    if (App_IsVoltageProtectionActive() != 0U)
    {
      App_TripVoltageProtection();
    }
  }

  if (periodic_refresh_due != 0U)
  {
    App_CcPidStatus pid_status = App_CcPid_GetStatus();
    App_TelemetryFrame telemetry =
    {
      now,
      imon_sample.raw,
      imon_sample.milliamps,
      pid_status.active,
      pid_status.target_raw,
      pid_status.measured_raw,
      pid_status.error_raw,
      dtrk_applied_control_level
    };

    /* Telemetry is best effort and must not stop the control loop. */
    (void)App_Telemetry_Emit(&telemetry);
  }

  App_UpdateLedStatus();
  App_TryShowStatus();
  return HAL_OK;
}

HAL_StatusTypeDef App_Init(void)
{
  HAL_StatusTypeDef status;

  BSP_GPIO_Init();
  BSP_GPIO_LedBlueWrite(BSP_OUTPUT_OFF);
  BSP_GPIO_LedRedWrite(BSP_OUTPUT_OFF);
  BSP_GPIO_LedYellowWrite(BSP_OUTPUT_OFF);
  App_ResetLedBlinkPhase();
  power_stage_enabled = 0U;
  App_CcPid_Init();
  dtrk_target_control_level = APP_DTRK_DEFAULT_LEVEL;
  dtrk_applied_control_level = APP_DTRK_IDLE_LEVEL;
  sin_restore_control_level = APP_DTRK_DEFAULT_LEVEL;
  sin_output = APP_SIN_OUTPUT_NONE;
  sin_sample_index = 0U;
  sin_dither_accumulator = 0U;
  App_Menu_Init(&menu_state, dtrk_target_control_level,
                APP_FAN_DEFAULT_DUTY_PERCENT,
                APP_POWER_ENABLE_DEFAULT);
  overcurrent_latched = 0U;
  voltage_protection_latched = 0U;
  protection_snapshot_valid = 0U;
  vout_display_millivolts = 0U;
  protection_vout_display_millivolts = 0U;
  VoutFilter_Init(&vout_filter);

  status = BSP_DAC_SoftStart(APP_DAC_DEFAULT_MV, 0U);
  if (status != HAL_OK)
  {
    return status;
  }

  status = BSP_PWM_DtrkSoftStart(APP_DTRK_IDLE_LEVEL, 0U);
  if (status != HAL_OK)
  {
    return status;
  }

  (void)BSP_Encoder_Init();

  status = BSP_PWM_Init();
  if (status != HAL_OK)
  {
    return status;
  }

  status = BSP_Fan_Init(menu_state.fan_duty_percent);
  if (status != HAL_OK)
  {
    return status;
  }
  BSP_Button_Init();

  (void)App_Status_Init();
  App_TryShowStatus();

  status = BSP_ADC_Init();
  if (status != HAL_OK)
  {
    return status;
  }

  status = App_ReadMeasurements();
  if (status != HAL_OK)
  {
    App_TripVoltageProtection();
    return status;
  }

  if (App_IsVoltageProtectionActive() != 0U)
  {
    App_CaptureProtectionSnapshotOnce();
    App_TripVoltageProtection();
  }
  else
  {
    status = BSP_PWM_DtrkSetControlLevel(APP_DTRK_DEFAULT_LEVEL);
    if (status != HAL_OK)
    {
      App_TripVoltageProtection();
      return status;
    }

    BSP_GPIO_EnableWrite(BSP_GPIO_HIGH);
    power_stage_enabled = 1U;
    dtrk_applied_control_level = dtrk_target_control_level;
  }

  App_UpdateLedStatus();
  App_TryShowStatus();
  cc_control_tick = HAL_GetTick();
  status_refresh_tick = cc_control_tick;
  return HAL_OK;
}

HAL_StatusTypeDef App_Process(void)
{
  int32_t encoder_delta = BSP_Encoder_GetDelta();
  BSP_ButtonEvent button_event = BSP_Button_Update();
  HAL_StatusTypeDef status;
  uint8_t status_already_shown = 0U;
  uint8_t display_immediately = 0U;

  App_UpdateLedStatus();

  if (App_IsProtectionLatched() != 0U)
  {
    status = App_ProcessStartRequest(button_event, &status_already_shown);
    if ((status == HAL_OK) && (status_already_shown == 0U) &&
        (button_event == BSP_BUTTON_EVENT_PRESSED) &&
        (App_IsProtectionLatched() == 0U))
    {
      menu_state.page = APP_MENU_PAGE_MAIN;
      App_CcPid_Stop();
      display_immediately = 1U;
    }
  }
  else
  {
    status = App_ProcessMenu(encoder_delta, button_event,
                             &display_immediately);
  }

  if ((status != HAL_OK) || (status_already_shown != 0U))
  {
    return status;
  }

  status = App_ProcessCcControlIfDue();
  if (status != HAL_OK)
  {
    return status;
  }

  return App_RefreshStatusIfDue(display_immediately);
}

void App_SinSampleElapsed(void)
{
  App_SinOutput active_output;
  uint16_t sample_index;
  uint16_t scaled_compare;

  active_output = sin_output;
  if (active_output == APP_SIN_OUTPUT_NONE)
  {
    return;
  }

  sample_index = sin_sample_index;
  scaled_compare = app_sin_dtrk_scaled_compare[sample_index];
  if (active_output == APP_SIN_OUTPUT_DTRK)
  {
    BSP_PWM_DtrkSetCompare(App_SinDitheredCompare(scaled_compare));
    dtrk_applied_control_level = App_SinControlLevel(scaled_compare);
  }
  else
  {
    (void)BSP_DAC_SetMillivolts(
        App_AnalogSinDacMillivolts(scaled_compare));
  }

  sample_index++;
  if (sample_index >= APP_SIN_SAMPLE_COUNT)
  {
    sample_index = 0U;
  }
  sin_sample_index = sample_index;
}

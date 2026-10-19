#include "app_status.h"

#include <stdio.h>

#include "oled.h"

#define APP_STATUS_ADC_REFERENCE_MV 3300U
#define APP_STATUS_ADC_MAX_COUNTS 4095U
#define APP_STATUS_MAIN_MENU_VISIBLE_ITEMS 6U
#define APP_STATUS_MAIN_MENU_FIRST_Y 10U
#define APP_STATUS_MAIN_MENU_STEP_Y 9U

static const char *const app_status_menu_labels[APP_MENU_ITEM_COUNT] =
{
  "RUN INFO",
  "POWER SWITCH",
  "ADJUST VOUT",
  "FAN SPEED",
  "CC MODE",
  "SIN PWM",
  "ANALOG SIN",
  "CAL IMON",
  "AUTO CAL IMON",
  "DAC CAL IMON"
};

static uint16_t App_Status_ImonRawToMillivolts(uint16_t raw)
{
  return (uint16_t)(((uint32_t)raw * APP_STATUS_ADC_REFERENCE_MV +
                     (APP_STATUS_ADC_MAX_COUNTS / 2U)) /
                    APP_STATUS_ADC_MAX_COUNTS);
}

static void App_Status_PrintVoltage(uint8_t y, const char *name,
                                    uint16_t millivolts)
{
  char text[20];

  (void)snprintf(text, sizeof(text), "%s %lu.%03luV", name,
                 (unsigned long)(millivolts / 1000U),
                 (unsigned long)(millivolts % 1000U));
  OLED_PrintASCIIString(0U, y, text, &afont8x6, OLED_COLOR_NORMAL);
}

static void App_Status_PrintCurrent(uint8_t y, int32_t milliamps)
{
  char text[20];
  const char *sign = (milliamps < 0L) ? "-" : "+";
  uint32_t magnitude_milliamps = (milliamps < 0L) ?
      (uint32_t)(-(int64_t)milliamps) :
      (uint32_t)milliamps;

  (void)snprintf(text, sizeof(text), "IMON I %s%lu.%03luA", sign,
                 (unsigned long)(magnitude_milliamps / 1000U),
                 (unsigned long)(magnitude_milliamps % 1000U));
  OLED_PrintASCIIString(0U, y, text, &afont8x6, OLED_COLOR_NORMAL);
}

static void App_Status_PrintImonVoltage(uint8_t y, uint16_t raw)
{
  App_Status_PrintVoltage(y, "IMON", App_Status_ImonRawToMillivolts(raw));
}

static void App_Status_PrintNtc(uint8_t y, const BSP_NTC_Sample *sample)
{
  char text[20];

  if (sample->status == BSP_NTC_STATUS_OK)
  {
    int32_t temperature_deci_celsius = sample->temperature_deci_celsius;
    const char *sign = (temperature_deci_celsius < 0L) ? "-" : "";
    uint32_t magnitude_deci_celsius = (temperature_deci_celsius < 0L) ?
        (uint32_t)(-(int64_t)temperature_deci_celsius) :
        (uint32_t)temperature_deci_celsius;

    (void)snprintf(text, sizeof(text), "NTC %s%lu.%luC", sign,
                   (unsigned long)(magnitude_deci_celsius / 10U),
                   (unsigned long)(magnitude_deci_celsius % 10U));
  }
  else if (sample->status == BSP_NTC_STATUS_SHORT)
  {
    (void)snprintf(text, sizeof(text), "NTC SHORT");
  }
  else if (sample->status == BSP_NTC_STATUS_OPEN)
  {
    (void)snprintf(text, sizeof(text), "NTC OPEN");
  }
  else
  {
    (void)snprintf(text, sizeof(text), "NTC ERROR");
  }

  OLED_PrintASCIIString(0U, y, text, &afont8x6, OLED_COLOR_NORMAL);
}

static void App_Status_PrintCcCurrent(uint8_t y, const char *label,
                                      int32_t milliamps)
{
  char text[20];
  const char *sign = (milliamps < 0L) ? "-" : "+";
  uint32_t magnitude_milliamps = (milliamps < 0L) ?
      (uint32_t)(-(int64_t)milliamps) : (uint32_t)milliamps;

  (void)snprintf(text, sizeof(text), "%s %s%lu.%03luA", label, sign,
                 (unsigned long)(magnitude_milliamps / 1000U),
                 (unsigned long)(magnitude_milliamps % 1000U));
  OLED_PrintASCIIString(0U, y, text, &afont8x6, OLED_COLOR_NORMAL);
}

static void App_Status_PrintMenuItem(uint8_t y, App_MenuItem item,
                                     App_MenuItem selected_item,
                                     const char *label)
{
  char text[22];
  OLED_ColorMode color = (item == selected_item) ? OLED_COLOR_REVERSED :
                                                    OLED_COLOR_NORMAL;

  (void)snprintf(text, sizeof(text), "%c %-18s",
                 (item == selected_item) ? '>' : ' ', label);
  OLED_PrintASCIIString(0U, y, text, &afont8x6, color);
}

static HAL_StatusTypeDef App_Status_ShowMainMenu(App_MenuItem selected_item)
{
  App_MenuItem first_visible_item = APP_MENU_ITEM_RUN_INFO;
  uint32_t first_visible_index = 0U;
  uint8_t item_index;

  if (((uint32_t)selected_item + 1U) > APP_STATUS_MAIN_MENU_VISIBLE_ITEMS)
  {
    first_visible_index = (uint32_t)selected_item + 1U -
                          APP_STATUS_MAIN_MENU_VISIBLE_ITEMS;
  }
  if (first_visible_index >
      (APP_MENU_ITEM_COUNT - APP_STATUS_MAIN_MENU_VISIBLE_ITEMS))
  {
    first_visible_index = APP_MENU_ITEM_COUNT -
                          APP_STATUS_MAIN_MENU_VISIBLE_ITEMS;
  }
  first_visible_item = (App_MenuItem)first_visible_index;

  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "LM5177 MENU", &afont8x6,
                        OLED_COLOR_NORMAL);

  for (item_index = 0U; item_index < APP_STATUS_MAIN_MENU_VISIBLE_ITEMS;
       item_index++)
  {
    App_MenuItem item = (App_MenuItem)(first_visible_item + item_index);
    App_Status_PrintMenuItem(
        (uint8_t)(APP_STATUS_MAIN_MENU_FIRST_Y +
                  (item_index * APP_STATUS_MAIN_MENU_STEP_Y)),
        item, selected_item, app_status_menu_labels[item]);
  }

  return OLED_ShowFrame();
}

static HAL_StatusTypeDef App_Status_ShowRunInfo(
    const BSP_ADC_Sample *vin, const BSP_ADC_Sample *vout,
    uint16_t vout_display_millivolts, const BSP_ADC_ImonSample *imon,
    const BSP_NTC_Sample *ntc,
    int32_t imon_milliamps)
{
  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "RUN INFO", &afont8x6, OLED_COLOR_NORMAL);
  App_Status_PrintNtc(8U, ntc);
  App_Status_PrintVoltage(16U, "VOUT", vout_display_millivolts);
  App_Status_PrintVoltage(24U, "VIN", vin->millivolts);
  App_Status_PrintImonVoltage(32U, imon->raw);
  App_Status_PrintCurrent(40U, imon_milliamps);
  App_Status_PrintVoltage(48U, "VOUT ADC", vout->node_millivolts);
  App_Status_PrintVoltage(56U, "VIN ADC", vin->node_millivolts);
  return OLED_ShowFrame();
}

static HAL_StatusTypeDef App_Status_ShowAdjustVout(
    const BSP_ADC_Sample *vout, uint16_t vout_target_millivolts,
    uint16_t dtrk_target_control_level)
{
  char text[20];

  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "ADJUST VOUT", &afont8x6,
                        OLED_COLOR_NORMAL);
  App_Status_PrintVoltage(16U, "NOW", vout->millivolts);
  App_Status_PrintVoltage(24U, "SET", vout_target_millivolts);
  (void)snprintf(text, sizeof(text), "DTRK %u%%",
                 (unsigned int)(dtrk_target_control_level / 10U));
  OLED_PrintASCIIString(0U, 32U, text, &afont8x6, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 48U, "PUSH: BACK", &afont8x6,
                        OLED_COLOR_NORMAL);
  return OLED_ShowFrame();
}

static HAL_StatusTypeDef App_Status_ShowPowerSwitch(uint8_t power_enabled)
{
  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "POWER SWITCH", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 16U,
                        (power_enabled != 0U) ? "EN HIGH" : "EN LOW",
                        &afont8x6, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 28U,
                        (power_enabled != 0U) ? "RUN LED BLINK" :
                                               "RUN LED ON",
                        &afont8x6, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 44U, "TURN: SELECT", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 56U, "PUSH: SAVE", &afont8x6,
                        OLED_COLOR_NORMAL);
  return OLED_ShowFrame();
}

static HAL_StatusTypeDef App_Status_ShowAdjustFan(uint8_t duty_percent)
{
  char text[20];

  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "FAN SPEED", &afont8x6,
                        OLED_COLOR_NORMAL);
  (void)snprintf(text, sizeof(text), "DUTY %u%%", (unsigned int)duty_percent);
  OLED_PrintASCIIString(0U, 16U, text, &afont8x6, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 32U, "TURN: 5%", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 48U, "PUSH: BACK", &afont8x6,
                        OLED_COLOR_NORMAL);
  return OLED_ShowFrame();
}

static HAL_StatusTypeDef App_Status_ShowCcSetup(
    uint16_t imon_raw, int32_t imon_milliamps, int32_t cc_max_milliamps,
    int32_t cc_target_milliamps)
{
  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "CC SETUP", &afont8x6, OLED_COLOR_NORMAL);
  App_Status_PrintCcCurrent(16U, "CUR", imon_milliamps);
  App_Status_PrintImonVoltage(24U, imon_raw);
  App_Status_PrintCcCurrent(32U, "MAX", cc_max_milliamps);
  App_Status_PrintCcCurrent(40U, "SET", cc_target_milliamps);
  OLED_PrintASCIIString(0U, 48U, "PUSH: START", &afont8x6,
                        OLED_COLOR_NORMAL);
  return OLED_ShowFrame();
}

static HAL_StatusTypeDef App_Status_ShowCcActive(
    uint16_t imon_raw, int32_t imon_milliamps,
    int32_t cc_target_milliamps,
    uint16_t dtrk_applied_control_level)
{
  char text[20];

  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "CC ACTIVE", &afont8x6, OLED_COLOR_NORMAL);
  App_Status_PrintCcCurrent(16U, "CUR", imon_milliamps);
  App_Status_PrintImonVoltage(24U, imon_raw);
  App_Status_PrintCcCurrent(32U, "SET", cc_target_milliamps);
  (void)snprintf(text, sizeof(text), "DTRK %u%%",
                 (unsigned int)(dtrk_applied_control_level / 10U));
  OLED_PrintASCIIString(0U, 40U, text, &afont8x6, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 48U, "PUSH: EXIT", &afont8x6,
                        OLED_COLOR_NORMAL);
  return OLED_ShowFrame();
}

static HAL_StatusTypeDef App_Status_ShowImonCalibration(
    uint16_t imon_raw, int32_t imon_milliamps,
    int32_t reference_milliamps)
{
  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "CAL IMON", &afont8x6,
                        OLED_COLOR_NORMAL);
  App_Status_PrintCcCurrent(16U, "CUR", imon_milliamps);
  App_Status_PrintImonVoltage(24U, imon_raw);
  App_Status_PrintCcCurrent(32U, "REF", reference_milliamps);
  OLED_PrintASCIIString(0U, 48U, "TURN: 0.05A", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 56U, "PUSH: APPLY", &afont8x6,
                        OLED_COLOR_NORMAL);
  return OLED_ShowFrame();
}

static HAL_StatusTypeDef App_Status_ShowAutoImonCalibration(
    uint16_t imon_raw, int32_t imon_milliamps)
{
  char text[20];

  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "AUTO IMON CAL", &afont8x6,
                        OLED_COLOR_NORMAL);
  App_Status_PrintImonVoltage(16U, imon_raw);
  (void)snprintf(text, sizeof(text), "ZERO ADC %u",
                 (unsigned int)imon_raw);
  OLED_PrintASCIIString(0U, 28U, text, &afont8x6, OLED_COLOR_NORMAL);
  App_Status_PrintCcCurrent(40U, "ZERO", imon_milliamps);
  OLED_PrintASCIIString(0U, 48U, "EN LOW", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 56U, "PUSH: BACK", &afont8x6,
                        OLED_COLOR_NORMAL);
  return OLED_ShowFrame();
}

static HAL_StatusTypeDef App_Status_ShowDacImonCalibration(
    uint16_t imon_raw, int32_t imon_milliamps)
{
  char text[20];

  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "DAC IMON CAL", &afont8x6,
                        OLED_COLOR_NORMAL);
  App_Status_PrintImonVoltage(16U, imon_raw);
  (void)snprintf(text, sizeof(text), "ZERO ADC %u",
                 (unsigned int)imon_raw);
  OLED_PrintASCIIString(0U, 28U, text, &afont8x6, OLED_COLOR_NORMAL);
  App_Status_PrintCcCurrent(40U, "ZERO", imon_milliamps);
  OLED_PrintASCIIString(0U, 48U, "DAC 0.000V", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 56U, "PUSH: BACK", &afont8x6,
                        OLED_COLOR_NORMAL);
  return OLED_ShowFrame();
}

static HAL_StatusTypeDef App_Status_ShowSinActive(void)
{
  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "SIN MODE", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 16U, "OUTPUT 5-15V", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 28U, "FREQ 50Hz", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 40U, "DTRK 31.3-93.8%", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 52U, "PUSH: EXIT", &afont8x6,
                        OLED_COLOR_NORMAL);
  return OLED_ShowFrame();
}

static HAL_StatusTypeDef App_Status_ShowAnalogSinActive(void)
{
  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 0U, "ANALOG SIN", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 13U, "OUTPUT 5-15V", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 23U, "FREQ 50Hz", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 33U, "DAC 0.313-0.938V", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 43U, "DTRK 100%", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 54U, "PUSH: EXIT", &afont8x6,
                        OLED_COLOR_NORMAL);
  return OLED_ShowFrame();
}

HAL_StatusTypeDef App_Status_Init(void)
{
  /* SSD1306 上电后需等待电源稳定，延时值由 OLED 驱动统一定义。 */
  HAL_Delay(OLED_POWER_ON_DELAY_MS);
  return OLED_Init();
}

HAL_StatusTypeDef App_Status_ShowAdcError(void)
{
  OLED_NewFrame();
  OLED_PrintASCIIString(0U, 8U, "ADC ERROR", &afont8x6,
                        OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(0U, 24U, "POWER DISABLED", &afont8x6,
                        OLED_COLOR_NORMAL);
  return OLED_ShowFrame();
}

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
    uint16_t dtrk_applied_control_level)
{
  if (page == APP_MENU_PAGE_MAIN)
  {
    return App_Status_ShowMainMenu(selected_item);
  }

  if (page == APP_MENU_PAGE_ADJUST_VOUT)
  {
    return App_Status_ShowAdjustVout(vout, vout_target_millivolts,
                                     dtrk_target_control_level);
  }

  if (page == APP_MENU_PAGE_POWER_SWITCH)
  {
    return App_Status_ShowPowerSwitch(power_enabled);
  }

  if (page == APP_MENU_PAGE_ADJUST_FAN)
  {
    return App_Status_ShowAdjustFan(fan_duty_percent);
  }

  if (page == APP_MENU_PAGE_CC_SETUP)
  {
    return App_Status_ShowCcSetup(imon->raw, imon_milliamps,
                                  cc_max_milliamps,
                                  cc_target_milliamps);
  }

  if (page == APP_MENU_PAGE_CC_ACTIVE)
  {
    return App_Status_ShowCcActive(imon->raw,
                                   imon_milliamps,
                                   cc_target_milliamps,
                                   dtrk_applied_control_level);
  }

  if (page == APP_MENU_PAGE_SIN_ACTIVE)
  {
    return App_Status_ShowSinActive();
  }

  if (page == APP_MENU_PAGE_ANALOG_SIN_ACTIVE)
  {
    return App_Status_ShowAnalogSinActive();
  }

  if (page == APP_MENU_PAGE_IMON_CALIBRATION)
  {
    return App_Status_ShowImonCalibration(
        imon->raw, imon_milliamps,
        imon_calibration_reference_milliamps);
  }

  if (page == APP_MENU_PAGE_AUTO_IMON_CALIBRATION)
  {
    return App_Status_ShowAutoImonCalibration(imon->raw,
                                              imon_milliamps);
  }

  if (page == APP_MENU_PAGE_DAC_IMON_CALIBRATION)
  {
    return App_Status_ShowDacImonCalibration(imon->raw,
                                             imon_milliamps);
  }

  return App_Status_ShowRunInfo(vin, vout, vout_display_millivolts, imon, ntc,
                                imon_milliamps);
}

HAL_StatusTypeDef App_Status_Show(const BSP_ADC_Sample *vin,
                                   const BSP_ADC_Sample *vout,
                                   uint16_t vout_display_millivolts,
                                   const BSP_ADC_ImonSample *imon,
                                   const BSP_NTC_Sample *ntc,
                                   uint8_t fan_duty_percent,
                                   int32_t imon_milliamps,
                                   uint16_t dtrk_target_control_level)
{
  (void)fan_duty_percent;
  (void)dtrk_target_control_level;
  return App_Status_ShowRunInfo(vin, vout, vout_display_millivolts, imon, ntc,
                                imon_milliamps);
}

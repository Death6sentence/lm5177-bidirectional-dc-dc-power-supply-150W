#ifndef APP_MENU_H
#define APP_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define APP_MENU_VOUT_MIN_MV 0U
#define APP_MENU_VOUT_MAX_MV 16000U
#define APP_MENU_VOUT_STEP_MV 80U
#define APP_MENU_DTRK_CONTROL_LEVEL_MIN 0U
#define APP_MENU_DTRK_CONTROL_LEVEL_MAX 1000U
#define APP_MENU_CC_TARGET_MIN_MA 0L
#define APP_MENU_CC_TARGET_MAX_MA 2800L
#define APP_MENU_CC_STEP_MA 50L
#define APP_MENU_IMON_CAL_REFERENCE_MIN_MA 0L
#define APP_MENU_IMON_CAL_REFERENCE_MAX_MA 2800L
#define APP_MENU_IMON_CAL_REFERENCE_STEP_MA 50L
#define APP_MENU_FAN_DUTY_OFF_PERCENT 0U
#define APP_MENU_FAN_DUTY_MIN_PERCENT 20U
#define APP_MENU_FAN_DUTY_MAX_PERCENT 100U
#define APP_MENU_FAN_DUTY_STEP_PERCENT 5U

typedef enum
{
  APP_MENU_PAGE_MAIN = 0U,
  APP_MENU_PAGE_RUN_INFO,
  APP_MENU_PAGE_POWER_SWITCH,
  APP_MENU_PAGE_ADJUST_VOUT,
  APP_MENU_PAGE_ADJUST_FAN,
  APP_MENU_PAGE_CC_SETUP,
  APP_MENU_PAGE_CC_ACTIVE,
  APP_MENU_PAGE_SIN_ACTIVE,
  APP_MENU_PAGE_ANALOG_SIN_ACTIVE,
  APP_MENU_PAGE_IMON_CALIBRATION,
  APP_MENU_PAGE_AUTO_IMON_CALIBRATION,
  APP_MENU_PAGE_DAC_IMON_CALIBRATION
} App_MenuPage;

typedef enum
{
  APP_MENU_ITEM_RUN_INFO = 0U,
  APP_MENU_ITEM_POWER_SWITCH,
  APP_MENU_ITEM_ADJUST_VOUT,
  APP_MENU_ITEM_ADJUST_FAN,
  APP_MENU_ITEM_CC_MODE,
  APP_MENU_ITEM_SIN_MODE,
  APP_MENU_ITEM_ANALOG_SIN_MODE,
  APP_MENU_ITEM_CAL_IMON,
  APP_MENU_ITEM_AUTO_CAL_IMON,
  APP_MENU_ITEM_DAC_CAL_IMON,
  APP_MENU_ITEM_COUNT
} App_MenuItem;

typedef struct
{
  App_MenuPage page;
  App_MenuItem selected_item;
  uint16_t vout_target_millivolts;
  uint8_t fan_duty_percent;
  uint8_t power_enabled;
  int32_t cc_max_milliamps;
  int32_t cc_target_milliamps;
  int32_t imon_calibration_reference_milliamps;
  int32_t main_menu_encoder_remainder;
} App_MenuState;

typedef struct
{
  uint8_t page_changed;
  uint8_t selection_changed;
  uint8_t dtrk_changed;
  uint8_t enter_cc_setup;
  uint8_t start_cc;
  uint8_t stop_cc;
  uint8_t start_sin;
  uint8_t stop_sin;
  uint8_t start_analog_sin;
  uint8_t stop_analog_sin;
  uint8_t cc_target_changed;
  uint8_t fan_duty_changed;
  uint8_t power_enabled_changed;
  uint8_t apply_power_enable;
  uint8_t enter_imon_calibration;
  uint8_t imon_calibration_reference_changed;
  uint8_t apply_imon_calibration;
  uint8_t start_auto_imon_calibration;
  uint8_t stop_auto_imon_calibration;
  uint8_t start_dac_imon_calibration;
  uint8_t stop_dac_imon_calibration;
  uint16_t dtrk_target_control_level;
  uint8_t fan_duty_percent;
  uint8_t power_enabled;
  int32_t imon_calibration_reference_milliamps;
} App_MenuAction;

void App_Menu_Init(App_MenuState *state, uint16_t initial_dtrk_control_level,
                   uint8_t initial_fan_duty_percent,
                   uint8_t initial_power_enabled);
void App_Menu_Update(App_MenuState *state, int32_t encoder_delta,
                     uint8_t button_pressed, int32_t fresh_imon_milliamps,
                     App_MenuAction *action);
uint16_t App_Menu_DtrkControlLevelFromVout(uint16_t vout_millivolts);
uint16_t App_Menu_VoutMillivoltsFromDtrk(uint16_t control_level);

#ifdef __cplusplus
}
#endif

#endif

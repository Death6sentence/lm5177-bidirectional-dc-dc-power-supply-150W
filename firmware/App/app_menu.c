#include "app_menu.h"

#define APP_MENU_MAIN_ENCODER_COUNTS_PER_ITEM 2L

static uint16_t App_Menu_ClampDtrkControlLevel(uint16_t control_level)
{
  if (control_level > APP_MENU_DTRK_CONTROL_LEVEL_MAX)
  {
    return APP_MENU_DTRK_CONTROL_LEVEL_MAX;
  }

  return control_level;
}

static void App_Menu_ClearAction(App_MenuAction *action)
{
  action->page_changed = 0U;
  action->selection_changed = 0U;
  action->dtrk_changed = 0U;
  action->enter_cc_setup = 0U;
  action->start_cc = 0U;
  action->stop_cc = 0U;
  action->start_sin = 0U;
  action->stop_sin = 0U;
  action->start_analog_sin = 0U;
  action->stop_analog_sin = 0U;
  action->cc_target_changed = 0U;
  action->fan_duty_changed = 0U;
  action->power_enabled_changed = 0U;
  action->apply_power_enable = 0U;
  action->enter_imon_calibration = 0U;
  action->imon_calibration_reference_changed = 0U;
  action->apply_imon_calibration = 0U;
  action->start_auto_imon_calibration = 0U;
  action->stop_auto_imon_calibration = 0U;
  action->start_dac_imon_calibration = 0U;
  action->stop_dac_imon_calibration = 0U;
  action->dtrk_target_control_level = 0U;
  action->fan_duty_percent = 0U;
  action->power_enabled = 0U;
  action->imon_calibration_reference_milliamps = 0L;
}

uint16_t App_Menu_DtrkControlLevelFromVout(uint16_t vout_millivolts)
{
  uint32_t control_level;

  if (vout_millivolts > APP_MENU_VOUT_MAX_MV)
  {
    vout_millivolts = APP_MENU_VOUT_MAX_MV;
  }

  control_level = (((uint32_t)vout_millivolts *
                    APP_MENU_DTRK_CONTROL_LEVEL_MAX) +
                   (APP_MENU_VOUT_MAX_MV / 2U)) / APP_MENU_VOUT_MAX_MV;
  return (uint16_t)control_level;
}

uint16_t App_Menu_VoutMillivoltsFromDtrk(uint16_t control_level)
{
  control_level = App_Menu_ClampDtrkControlLevel(control_level);
  return (uint16_t)((((uint32_t)control_level * APP_MENU_VOUT_MAX_MV) +
                     (APP_MENU_DTRK_CONTROL_LEVEL_MAX / 2U)) /
                    APP_MENU_DTRK_CONTROL_LEVEL_MAX);
}

void App_Menu_Init(App_MenuState *state, uint16_t initial_dtrk_control_level,
                   uint8_t initial_fan_duty_percent,
                   uint8_t initial_power_enabled)
{
  state->page = APP_MENU_PAGE_MAIN;
  state->selected_item = APP_MENU_ITEM_RUN_INFO;
  state->vout_target_millivolts =
      App_Menu_VoutMillivoltsFromDtrk(initial_dtrk_control_level);
  if (initial_fan_duty_percent == APP_MENU_FAN_DUTY_OFF_PERCENT)
  {
    state->fan_duty_percent = APP_MENU_FAN_DUTY_OFF_PERCENT;
  }
  else if (initial_fan_duty_percent < APP_MENU_FAN_DUTY_MIN_PERCENT)
  {
    state->fan_duty_percent = APP_MENU_FAN_DUTY_MIN_PERCENT;
  }
  else if (initial_fan_duty_percent > APP_MENU_FAN_DUTY_MAX_PERCENT)
  {
    state->fan_duty_percent = APP_MENU_FAN_DUTY_MAX_PERCENT;
  }
  else
  {
    state->fan_duty_percent = initial_fan_duty_percent;
  }
  state->power_enabled = (initial_power_enabled != 0U) ? 1U : 0U;
  state->cc_max_milliamps = 0L;
  state->cc_target_milliamps = 0L;
  state->imon_calibration_reference_milliamps = 0L;
  state->main_menu_encoder_remainder = 0L;
}

static void App_Menu_UpdateSelection(App_MenuState *state,
                                     int32_t encoder_delta,
                                     App_MenuAction *action)
{
  int64_t accumulated = (int64_t)state->main_menu_encoder_remainder +
                        encoder_delta;
  int32_t whole_steps = (int32_t)(accumulated /
                                  APP_MENU_MAIN_ENCODER_COUNTS_PER_ITEM);
  int32_t selected_item = (int32_t)state->selected_item + whole_steps;

  state->main_menu_encoder_remainder =
      (int32_t)(accumulated % APP_MENU_MAIN_ENCODER_COUNTS_PER_ITEM);

  if (selected_item < (int32_t)APP_MENU_ITEM_RUN_INFO)
  {
    selected_item = (int32_t)APP_MENU_ITEM_RUN_INFO;
  }
  else if (selected_item >= (int32_t)APP_MENU_ITEM_COUNT)
  {
    selected_item = (int32_t)APP_MENU_ITEM_COUNT - 1;
  }

  if ((App_MenuItem)selected_item != state->selected_item)
  {
    state->selected_item = (App_MenuItem)selected_item;
    action->selection_changed = 1U;
  }
}

static void App_Menu_ReturnMain(App_MenuState *state,
                                App_MenuAction *action)
{
  state->page = APP_MENU_PAGE_MAIN;
  state->main_menu_encoder_remainder = 0L;
  action->page_changed = 1U;
}

static void App_Menu_InitCcSetup(App_MenuState *state,
                                 int32_t fresh_imon_milliamps,
                                 App_MenuAction *action)
{
  state->cc_max_milliamps = APP_MENU_CC_TARGET_MAX_MA;
  if (fresh_imon_milliamps < APP_MENU_CC_TARGET_MIN_MA)
  {
    state->cc_target_milliamps = APP_MENU_CC_TARGET_MIN_MA;
  }
  else if (fresh_imon_milliamps > APP_MENU_CC_TARGET_MAX_MA)
  {
    state->cc_target_milliamps = APP_MENU_CC_TARGET_MAX_MA;
  }
  else
  {
    state->cc_target_milliamps = fresh_imon_milliamps;
  }
  state->page = APP_MENU_PAGE_CC_SETUP;
  action->enter_cc_setup = 1U;
  action->page_changed = 1U;
}

static int32_t App_Menu_RoundImonCalibrationReference(
    int32_t fresh_imon_milliamps)
{
  int32_t rounded_milliamps;

  if (fresh_imon_milliamps <= APP_MENU_IMON_CAL_REFERENCE_MIN_MA)
  {
    return APP_MENU_IMON_CAL_REFERENCE_MIN_MA;
  }

  if (fresh_imon_milliamps >= APP_MENU_IMON_CAL_REFERENCE_MAX_MA)
  {
    return APP_MENU_IMON_CAL_REFERENCE_MAX_MA;
  }

  rounded_milliamps =
      ((fresh_imon_milliamps +
        (APP_MENU_IMON_CAL_REFERENCE_STEP_MA / 2L)) /
       APP_MENU_IMON_CAL_REFERENCE_STEP_MA) *
      APP_MENU_IMON_CAL_REFERENCE_STEP_MA;

  if (rounded_milliamps > APP_MENU_IMON_CAL_REFERENCE_MAX_MA)
  {
    return APP_MENU_IMON_CAL_REFERENCE_MAX_MA;
  }

  return rounded_milliamps;
}

static void App_Menu_InitImonCalibration(App_MenuState *state,
                                         int32_t fresh_imon_milliamps,
                                         App_MenuAction *action)
{
  state->imon_calibration_reference_milliamps =
      App_Menu_RoundImonCalibrationReference(fresh_imon_milliamps);
  state->page = APP_MENU_PAGE_IMON_CALIBRATION;
  state->main_menu_encoder_remainder = 0L;
  action->enter_imon_calibration = 1U;
  action->page_changed = 1U;
}

static void App_Menu_UpdateCcTarget(App_MenuState *state, int32_t encoder_delta,
                                    App_MenuAction *action)
{
  int64_t requested_milliamps = (int64_t)state->cc_target_milliamps +
                                ((int64_t)encoder_delta *
                                 APP_MENU_CC_STEP_MA);

  if (requested_milliamps < APP_MENU_CC_TARGET_MIN_MA)
  {
    requested_milliamps = APP_MENU_CC_TARGET_MIN_MA;
  }
  else if (requested_milliamps > APP_MENU_CC_TARGET_MAX_MA)
  {
    requested_milliamps = APP_MENU_CC_TARGET_MAX_MA;
  }

  if ((int32_t)requested_milliamps != state->cc_target_milliamps)
  {
    state->cc_target_milliamps = (int32_t)requested_milliamps;
    action->cc_target_changed = 1U;
  }
}

static void App_Menu_UpdateVout(App_MenuState *state, int32_t encoder_delta,
                                App_MenuAction *action)
{
  int64_t requested_millivolts =
      (int64_t)state->vout_target_millivolts +
      ((int64_t)encoder_delta * APP_MENU_VOUT_STEP_MV);

  if (requested_millivolts < APP_MENU_VOUT_MIN_MV)
  {
    requested_millivolts = APP_MENU_VOUT_MIN_MV;
  }
  else if (requested_millivolts > APP_MENU_VOUT_MAX_MV)
  {
    requested_millivolts = APP_MENU_VOUT_MAX_MV;
  }

  if ((uint16_t)requested_millivolts != state->vout_target_millivolts)
  {
    state->vout_target_millivolts = (uint16_t)requested_millivolts;
    action->dtrk_changed = 1U;
    action->dtrk_target_control_level =
        App_Menu_DtrkControlLevelFromVout(state->vout_target_millivolts);
  }
}

static void App_Menu_UpdateImonCalibrationReference(
    App_MenuState *state, int32_t encoder_delta, App_MenuAction *action)
{
  int64_t requested_milliamps =
      (int64_t)state->imon_calibration_reference_milliamps +
      ((int64_t)encoder_delta * APP_MENU_IMON_CAL_REFERENCE_STEP_MA);

  if (requested_milliamps < APP_MENU_IMON_CAL_REFERENCE_MIN_MA)
  {
    requested_milliamps = APP_MENU_IMON_CAL_REFERENCE_MIN_MA;
  }
  else if (requested_milliamps > APP_MENU_IMON_CAL_REFERENCE_MAX_MA)
  {
    requested_milliamps = APP_MENU_IMON_CAL_REFERENCE_MAX_MA;
  }

  if ((int32_t)requested_milliamps !=
      state->imon_calibration_reference_milliamps)
  {
    state->imon_calibration_reference_milliamps =
        (int32_t)requested_milliamps;
    action->imon_calibration_reference_changed = 1U;
  }
}

static void App_Menu_UpdateFan(App_MenuState *state, int32_t encoder_delta,
                               App_MenuAction *action)
{
  int64_t requested_duty_percent =
      (int64_t)state->fan_duty_percent +
      ((int64_t)encoder_delta * APP_MENU_FAN_DUTY_STEP_PERCENT);

  if (requested_duty_percent <= APP_MENU_FAN_DUTY_OFF_PERCENT)
  {
    requested_duty_percent = APP_MENU_FAN_DUTY_OFF_PERCENT;
  }
  else if (requested_duty_percent < APP_MENU_FAN_DUTY_MIN_PERCENT)
  {
    requested_duty_percent = (encoder_delta > 0) ?
        APP_MENU_FAN_DUTY_MIN_PERCENT : APP_MENU_FAN_DUTY_OFF_PERCENT;
  }
  else if (requested_duty_percent > APP_MENU_FAN_DUTY_MAX_PERCENT)
  {
    requested_duty_percent = APP_MENU_FAN_DUTY_MAX_PERCENT;
  }

  if ((uint8_t)requested_duty_percent != state->fan_duty_percent)
  {
    state->fan_duty_percent = (uint8_t)requested_duty_percent;
    action->fan_duty_changed = 1U;
    action->fan_duty_percent = state->fan_duty_percent;
  }
}

static void App_Menu_UpdatePowerEnabled(App_MenuState *state,
                                        int32_t encoder_delta,
                                        App_MenuAction *action)
{
  uint8_t power_enabled = state->power_enabled;

  if (encoder_delta > 0L)
  {
    power_enabled = 1U;
  }
  else if (encoder_delta < 0L)
  {
    power_enabled = 0U;
  }

  if (power_enabled != state->power_enabled)
  {
    state->power_enabled = power_enabled;
    action->power_enabled_changed = 1U;
  }
}

void App_Menu_Update(App_MenuState *state, int32_t encoder_delta,
                     uint8_t button_pressed, int32_t fresh_imon_milliamps,
                     App_MenuAction *action)
{
  App_Menu_ClearAction(action);

  if (state->page == APP_MENU_PAGE_MAIN)
  {
    App_Menu_UpdateSelection(state, encoder_delta, action);

    if (button_pressed != 0U)
    {
      if (state->selected_item == APP_MENU_ITEM_RUN_INFO)
      {
        state->page = APP_MENU_PAGE_RUN_INFO;
        state->main_menu_encoder_remainder = 0L;
        action->page_changed = 1U;
      }
      else if (state->selected_item == APP_MENU_ITEM_POWER_SWITCH)
      {
        state->page = APP_MENU_PAGE_POWER_SWITCH;
        state->main_menu_encoder_remainder = 0L;
        action->page_changed = 1U;
      }
      else if (state->selected_item == APP_MENU_ITEM_ADJUST_VOUT)
      {
        state->page = APP_MENU_PAGE_ADJUST_VOUT;
        state->main_menu_encoder_remainder = 0L;
        action->page_changed = 1U;
      }
      else if (state->selected_item == APP_MENU_ITEM_ADJUST_FAN)
      {
        state->page = APP_MENU_PAGE_ADJUST_FAN;
        state->main_menu_encoder_remainder = 0L;
        action->page_changed = 1U;
      }
      else if (state->selected_item == APP_MENU_ITEM_CC_MODE)
      {
        state->main_menu_encoder_remainder = 0L;
        App_Menu_InitCcSetup(state, fresh_imon_milliamps, action);
      }
      else if (state->selected_item == APP_MENU_ITEM_SIN_MODE)
      {
        state->page = APP_MENU_PAGE_SIN_ACTIVE;
        state->main_menu_encoder_remainder = 0L;
        action->start_sin = 1U;
        action->page_changed = 1U;
      }
      else if (state->selected_item == APP_MENU_ITEM_ANALOG_SIN_MODE)
      {
        state->page = APP_MENU_PAGE_ANALOG_SIN_ACTIVE;
        state->main_menu_encoder_remainder = 0L;
        action->start_analog_sin = 1U;
        action->page_changed = 1U;
      }
      else if (state->selected_item == APP_MENU_ITEM_CAL_IMON)
      {
        App_Menu_InitImonCalibration(state, fresh_imon_milliamps, action);
      }
      else if (state->selected_item == APP_MENU_ITEM_AUTO_CAL_IMON)
      {
        state->page = APP_MENU_PAGE_AUTO_IMON_CALIBRATION;
        state->main_menu_encoder_remainder = 0L;
        action->start_auto_imon_calibration = 1U;
        action->page_changed = 1U;
      }
      else
      {
        state->page = APP_MENU_PAGE_DAC_IMON_CALIBRATION;
        state->main_menu_encoder_remainder = 0L;
        action->start_dac_imon_calibration = 1U;
        action->page_changed = 1U;
      }
    }
    return;
  }

  if ((state->page == APP_MENU_PAGE_CC_SETUP) &&
      (button_pressed != 0U))
  {
    state->page = APP_MENU_PAGE_CC_ACTIVE;
    action->start_cc = 1U;
    action->page_changed = 1U;
    return;
  }

  if ((state->page == APP_MENU_PAGE_CC_ACTIVE) &&
      (button_pressed != 0U))
  {
    App_Menu_ReturnMain(state, action);
    action->stop_cc = 1U;
    return;
  }

  if ((state->page == APP_MENU_PAGE_SIN_ACTIVE) &&
      (button_pressed != 0U))
  {
    App_Menu_ReturnMain(state, action);
    action->stop_sin = 1U;
    return;
  }

  if ((state->page == APP_MENU_PAGE_ANALOG_SIN_ACTIVE) &&
      (button_pressed != 0U))
  {
    App_Menu_ReturnMain(state, action);
    action->stop_analog_sin = 1U;
    return;
  }

  if ((state->page == APP_MENU_PAGE_IMON_CALIBRATION) &&
      (button_pressed != 0U))
  {
    action->apply_imon_calibration = 1U;
    action->imon_calibration_reference_milliamps =
        state->imon_calibration_reference_milliamps;
    App_Menu_ReturnMain(state, action);
    return;
  }

  if ((state->page == APP_MENU_PAGE_AUTO_IMON_CALIBRATION) &&
      (button_pressed != 0U))
  {
    App_Menu_ReturnMain(state, action);
    action->stop_auto_imon_calibration = 1U;
    return;
  }

  if ((state->page == APP_MENU_PAGE_DAC_IMON_CALIBRATION) &&
      (button_pressed != 0U))
  {
    App_Menu_ReturnMain(state, action);
    action->stop_dac_imon_calibration = 1U;
    return;
  }

  if ((state->page == APP_MENU_PAGE_POWER_SWITCH) &&
      (button_pressed != 0U))
  {
    action->apply_power_enable = 1U;
    action->power_enabled = state->power_enabled;
    App_Menu_ReturnMain(state, action);
    return;
  }

  if (button_pressed != 0U)
  {
    App_Menu_ReturnMain(state, action);
    return;
  }

  if ((state->page == APP_MENU_PAGE_ADJUST_VOUT) &&
      (encoder_delta != 0))
  {
    App_Menu_UpdateVout(state, encoder_delta, action);
  }
  else if ((state->page == APP_MENU_PAGE_POWER_SWITCH) &&
           (encoder_delta != 0))
  {
    App_Menu_UpdatePowerEnabled(state, encoder_delta, action);
  }
  else if ((state->page == APP_MENU_PAGE_ADJUST_FAN) &&
           (encoder_delta != 0))
  {
    App_Menu_UpdateFan(state, encoder_delta, action);
  }
  else if ((state->page == APP_MENU_PAGE_CC_SETUP) &&
           (encoder_delta != 0))
  {
    App_Menu_UpdateCcTarget(state, encoder_delta, action);
  }
  else if ((state->page == APP_MENU_PAGE_IMON_CALIBRATION) &&
           (encoder_delta != 0))
  {
    App_Menu_UpdateImonCalibrationReference(state, encoder_delta, action);
  }
}

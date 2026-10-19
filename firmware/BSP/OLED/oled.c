/**
 * @file oled.c
 * @brief 波特律动 OLED 驱动（SSD1306）项目适配版
 * @anchor 波特律动（Keysking 博哥在学习）
 * @version 1.0
 * @date 2023-08-19
 * @license MIT License
 *
 * @attention
 * 本文件的显存、帧工作流及文本 API 结构参考用户提供的 Keysking/波特律动
 * stm32_hal_ssd1306 源码，并适配 STM32G431KBT6、硬件 I2C2 和有限通信超时。
 */

#include "oled.h"

#include <stddef.h>
#include <string.h>

#include "i2c.h"

#define OLED_COMMAND_CONTROL_BYTE 0x00U
#define OLED_DATA_CONTROL_BYTE 0x40U
#define OLED_REPLACEMENT_CHARACTER ((uint8_t)'?')
#define OLED_INIT_RETRY_COUNT 3U
#define OLED_INIT_RETRY_DELAY_MS 10U

static uint8_t OLED_GRAM[OLED_PAGE_COUNT][OLED_WIDTH];

static HAL_StatusTypeDef OLED_Send(uint8_t *data, uint16_t length)
{
  return HAL_I2C_Master_Transmit(&hi2c2, OLED_I2C_HAL_ADDRESS, data, length,
                                 OLED_I2C_TIMEOUT_MS);
}

static HAL_StatusTypeDef OLED_SendCommand(uint8_t command)
{
  uint8_t send_buffer[2] = {OLED_COMMAND_CONTROL_BYTE, command};

  return OLED_Send(send_buffer, (uint16_t)sizeof(send_buffer));
}

static HAL_StatusTypeDef OLED_SendCommands(const uint8_t *commands,
                                           uint16_t command_count)
{
  uint16_t index;

  for (index = 0U; index < command_count; ++index)
  {
    HAL_StatusTypeDef status = OLED_SendCommand(commands[index]);

    if (status != HAL_OK)
    {
      return status;
    }
  }

  return HAL_OK;
}

static void OLED_SetBlock(uint8_t x, uint8_t y, const uint8_t *data,
                          uint8_t width, uint8_t height,
                          OLED_ColorMode color)
{
  uint16_t column;
  uint16_t row;

  if ((data == NULL) ||
      ((color != OLED_COLOR_NORMAL) && (color != OLED_COLOR_REVERSED)))
  {
    return;
  }

  for (column = 0U; column < width; ++column)
  {
    uint16_t pixel_x = (uint16_t)x + column;

    if (pixel_x >= OLED_WIDTH)
    {
      break;
    }

    for (row = 0U; row < height; ++row)
    {
      uint16_t pixel_y = (uint16_t)y + row;
      uint16_t data_index;
      uint8_t pixel_is_set;
      OLED_ColorMode pixel_color;

      if (pixel_y >= OLED_HEIGHT)
      {
        break;
      }

      data_index = (uint16_t)(((row / 8U) * width) + column);
      pixel_is_set = (uint8_t)((data[data_index] >> (row % 8U)) & 0x01U);
      pixel_color = (pixel_is_set != 0U) ? color :
                    ((color == OLED_COLOR_NORMAL) ? OLED_COLOR_REVERSED :
                                                    OLED_COLOR_NORMAL);
      OLED_SetPixel((uint8_t)pixel_x, (uint8_t)pixel_y, pixel_color);
    }
  }
}

static uint8_t OLED_GetUTF8Length(const char *string)
{
  uint8_t lead = (uint8_t)string[0];
  uint8_t length;
  uint8_t index;

  if ((lead & 0x80U) == 0U)
  {
    length = 1U;
  }
  else if ((lead & 0xE0U) == 0xC0U)
  {
    length = 2U;
  }
  else if ((lead & 0xF0U) == 0xE0U)
  {
    length = 3U;
  }
  else if ((lead & 0xF8U) == 0xF0U)
  {
    length = 4U;
  }
  else
  {
    return 0U;
  }

  for (index = 1U; index < length; ++index)
  {
    uint8_t continuation = (uint8_t)string[index];

    if ((continuation == 0U) || ((continuation & 0xC0U) != 0x80U))
    {
      return 0U;
    }
  }

  return length;
}

HAL_StatusTypeDef OLED_Init(void)
{
  static const uint8_t init_commands[] =
  {
    0xAEU, 0xD5U, 0x80U, 0xA8U, 0x3FU, 0xD3U, 0x00U, 0x40U,
    0x8DU, 0x14U, 0x20U, 0x02U, 0xA1U, 0xC8U, 0xDAU, 0x12U,
    0x81U, 0xCFU, 0xD9U, 0xF1U, 0xDBU, 0x30U, 0xA4U, 0xA6U,
    0x2EU, 0xAFU
  };
  HAL_StatusTypeDef status;
  uint32_t attempt;

  for (attempt = 0U; attempt < OLED_INIT_RETRY_COUNT; ++attempt)
  {
    status = HAL_I2C_IsDeviceReady(&hi2c2, OLED_I2C_HAL_ADDRESS, 1U,
                                   OLED_I2C_TIMEOUT_MS);
    if (status == HAL_OK)
    {
      status = OLED_SendCommands(init_commands,
                                 (uint16_t)(sizeof(init_commands) /
                                            sizeof(init_commands[0])));
      if (status == HAL_OK)
      {
        OLED_NewFrame();
        return OLED_ShowFrame();
      }
    }

    if ((attempt + 1U) < OLED_INIT_RETRY_COUNT)
    {
      HAL_Delay(OLED_INIT_RETRY_DELAY_MS);
    }
  }

  return status;
}

HAL_StatusTypeDef OLED_DisPlay_On(void)
{
  static const uint8_t commands[] = {0x8DU, 0x14U, 0xAFU};

  return OLED_SendCommands(commands,
                           (uint16_t)(sizeof(commands) / sizeof(commands[0])));
}

HAL_StatusTypeDef OLED_DisPlay_Off(void)
{
  static const uint8_t commands[] = {0x8DU, 0x10U, 0xAEU};

  return OLED_SendCommands(commands,
                           (uint16_t)(sizeof(commands) / sizeof(commands[0])));
}

HAL_StatusTypeDef OLED_SetColorMode(OLED_ColorMode mode)
{
  if (mode == OLED_COLOR_NORMAL)
  {
    return OLED_SendCommand(0xA6U);
  }
  if (mode == OLED_COLOR_REVERSED)
  {
    return OLED_SendCommand(0xA7U);
  }

  return HAL_ERROR;
}

void OLED_NewFrame(void)
{
  (void)memset(OLED_GRAM, 0, sizeof(OLED_GRAM));
}

HAL_StatusTypeDef OLED_ShowFrame(void)
{
  static uint8_t send_buffer[OLED_WIDTH + 1U];
  uint8_t page;

  send_buffer[0] = OLED_DATA_CONTROL_BYTE;
  for (page = 0U; page < OLED_PAGE_COUNT; ++page)
  {
    uint8_t page_commands[] = {(uint8_t)(0xB0U | page), 0x00U, 0x10U};
    HAL_StatusTypeDef status;

    status = OLED_SendCommands(page_commands,
                               (uint16_t)(sizeof(page_commands) /
                                          sizeof(page_commands[0])));
    if (status != HAL_OK)
    {
      return status;
    }

    (void)memcpy(&send_buffer[1], OLED_GRAM[page], OLED_WIDTH);
    status = OLED_Send(send_buffer, (uint16_t)sizeof(send_buffer));
    if (status != HAL_OK)
    {
      return status;
    }
  }

  return HAL_OK;
}

void OLED_SetPixel(uint8_t x, uint8_t y, OLED_ColorMode color)
{
  uint8_t mask;

  if ((x >= OLED_WIDTH) || (y >= OLED_HEIGHT))
  {
    return;
  }

  mask = (uint8_t)(1U << (y % 8U));
  if (color == OLED_COLOR_NORMAL)
  {
    OLED_GRAM[y / 8U][x] |= mask;
  }
  else if (color == OLED_COLOR_REVERSED)
  {
    OLED_GRAM[y / 8U][x] &= (uint8_t)~mask;
  }
}

void OLED_PrintASCIIChar(uint8_t x, uint8_t y, char character,
                         const ASCIIFont *font, OLED_ColorMode color)
{
  uint8_t code = (uint8_t)character;
  uint16_t bytes_per_glyph;
  const uint8_t *glyph;

  if ((font == NULL) || (font->chars == NULL) ||
      (font->w == 0U) || (font->h == 0U))
  {
    return;
  }

  if ((code < OLED_ASCII_FIRST_CHARACTER) ||
      (code > OLED_ASCII_LAST_CHARACTER))
  {
    code = OLED_REPLACEMENT_CHARACTER;
  }

  bytes_per_glyph = (uint16_t)((((uint16_t)font->h + 7U) / 8U) * font->w);
  glyph = font->chars +
          ((uint16_t)(code - OLED_ASCII_FIRST_CHARACTER) * bytes_per_glyph);
  OLED_SetBlock(x, y, glyph, font->w, font->h, color);
}

void OLED_PrintASCIIString(uint8_t x, uint8_t y, const char *string,
                           const ASCIIFont *font, OLED_ColorMode color)
{
  uint16_t cursor_x = x;

  if ((string == NULL) || (font == NULL) || (font->w == 0U))
  {
    return;
  }

  while ((*string != '\0') && (cursor_x < OLED_WIDTH))
  {
    OLED_PrintASCIIChar((uint8_t)cursor_x, y, *string, font, color);
    cursor_x += font->w;
    ++string;
  }
}

void OLED_PrintString(uint8_t x, uint8_t y, const char *string,
                      const Font *font, OLED_ColorMode color)
{
  size_t string_index = 0U;
  uint16_t cursor_x = x;

  if ((string == NULL) || (font == NULL) || (font->ascii == NULL) ||
      (font->ascii->chars == NULL) || (font->ascii->w == 0U))
  {
    return;
  }

  while ((string[string_index] != '\0') && (cursor_x < OLED_WIDTH))
  {
    uint8_t utf8_length = OLED_GetUTF8Length(&string[string_index]);
    uint8_t found = 0U;
    uint8_t glyph_index;

    if (utf8_length == 0U)
    {
      return;
    }

    if (font->chars != NULL)
    {
      size_t glyph_bytes = ((((size_t)font->h + 7U) / 8U) * font->w);
      size_t record_bytes = glyph_bytes + 4U;

      for (glyph_index = 0U; glyph_index < font->len; ++glyph_index)
      {
        const uint8_t *record = font->chars + ((size_t)glyph_index * record_bytes);

        if (memcmp(&string[string_index], record, utf8_length) == 0)
        {
          OLED_SetBlock((uint8_t)cursor_x, y, &record[4],
                        font->w, font->h, color);
          cursor_x += font->w;
          string_index += utf8_length;
          found = 1U;
          break;
        }
      }
    }

    if (found == 0U)
    {
      char fallback = (utf8_length == 1U) ? string[string_index] : ' ';

      OLED_PrintASCIIChar((uint8_t)cursor_x, y, fallback,
                          font->ascii, color);
      cursor_x += font->ascii->w;
      string_index += utf8_length;
    }
  }
}

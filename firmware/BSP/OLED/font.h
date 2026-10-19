#ifndef OLED_FONT_H
#define OLED_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define OLED_ASCII_FIRST_CHARACTER 0x20U
#define OLED_ASCII_LAST_CHARACTER 0x7EU
#define OLED_ASCII_CHARACTER_COUNT 95U

/**
 * @brief ASCII 字体描述符
 */
typedef struct ASCIIFont
{
  uint8_t h;             /**< 字符高度，单位为像素 */
  uint8_t w;             /**< 字符宽度，单位为像素 */
  const uint8_t *chars;  /**< 从空格 0x20 开始连续排列的列行式字模 */
} ASCIIFont;

/**
 * @brief UTF-8 字体描述符
 * @note 每个扩展字符记录由 4 字节 UTF-8 编码区和紧随其后的列行式字模组成。
 *       当 chars 为 NULL 且 len 为 0 时，仅使用 ascii 指向的 ASCII 字体。
 */
typedef struct Font
{
  uint8_t h;              /**< 扩展字符高度，单位为像素 */
  uint8_t w;              /**< 扩展字符宽度，单位为像素 */
  const uint8_t *chars;   /**< 扩展字符字模记录 */
  uint8_t len;            /**< 扩展字符数量 */
  const ASCIIFont *ascii; /**< 未命中扩展字模时使用的 ASCII 字体 */
} Font;

/** @brief 6x8 可打印 ASCII 字体。 */
extern const ASCIIFont afont8x6;

/** @brief 仅包含 6x8 ASCII 回退字体的通用字体描述符。 */
extern const Font font8x6;

#ifdef __cplusplus
}
#endif

#endif

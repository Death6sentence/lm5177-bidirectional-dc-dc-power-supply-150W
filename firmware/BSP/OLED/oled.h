#ifndef OLED_H
#define OLED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "font.h"
#include "main.h"

#define OLED_WIDTH 128U
#define OLED_HEIGHT 64U
#define OLED_PAGE_COUNT 8U
#define OLED_I2C_7BIT_ADDRESS 0x3CU
#define OLED_I2C_HAL_ADDRESS 0x78U
#define OLED_I2C_TIMEOUT_MS 50U
#define OLED_POWER_ON_DELAY_MS 1U

/**
 * @brief OLED 像素颜色模式
 */
typedef enum
{
  OLED_COLOR_NORMAL = 0U, /**< 正常绘制，字模有效位点亮像素 */
  OLED_COLOR_REVERSED     /**< 反色绘制，字模有效位熄灭像素 */
} OLED_ColorMode;

/**
 * @brief 初始化 SSD1306 OLED，并清空后刷新首帧
 * @param 无
 * @return HAL_OK 表示成功，其他值为底层 I2C 失败状态
 * @note 必须在 MX_I2C2_Init() 完成且 OLED 上电等待结束后调用。
 */
HAL_StatusTypeDef OLED_Init(void);

/**
 * @brief 开启 OLED 电荷泵和显示输出
 * @param 无
 * @return HAL_OK 表示成功，其他值为底层 I2C 失败状态
 */
HAL_StatusTypeDef OLED_DisPlay_On(void);

/**
 * @brief 关闭 OLED 显示输出和电荷泵
 * @param 无
 * @return HAL_OK 表示成功，其他值为底层 I2C 失败状态
 */
HAL_StatusTypeDef OLED_DisPlay_Off(void);

/**
 * @brief 设置 OLED 控制器的正常或反色显示模式
 * @param mode: OLED_COLOR_NORMAL 或 OLED_COLOR_REVERSED
 * @return HAL_OK 表示成功，HAL_ERROR 表示参数无效，其他值为 I2C 失败状态
 */
HAL_StatusTypeDef OLED_SetColorMode(OLED_ColorMode mode);

/**
 * @brief 清空显存并开始绘制新的一帧
 * @param 无
 * @return 无
 * @note 本函数仅修改 OLED_GRAM，不会立即访问 I2C。
 */
void OLED_NewFrame(void);

/**
 * @brief 将 OLED_GRAM 的完整帧按 8 页刷新到 OLED
 * @param 无
 * @return HAL_OK 表示成功，其他值为底层 I2C 失败状态
 */
HAL_StatusTypeDef OLED_ShowFrame(void);

/**
 * @brief 在显存中设置一个像素
 * @param x: 横坐标，有效范围为 0 到 127
 * @param y: 纵坐标，有效范围为 0 到 63
 * @param color: 正常模式点亮像素，反色模式熄灭像素
 * @return 无
 * @note 坐标或颜色模式无效时不修改显存。
 */
void OLED_SetPixel(uint8_t x, uint8_t y, OLED_ColorMode color);

/**
 * @brief 使用外部 ASCII 字体描述符绘制一个字符到显存
 * @param x: 字符左上角横坐标
 * @param y: 字符左上角纵坐标
 * @param character: 待绘制字符，不可打印字符按问号绘制
 * @param font: ASCII 字体描述符
 * @param color: 正常或反色绘制模式
 * @return 无
 */
void OLED_PrintASCIIChar(uint8_t x, uint8_t y, char character,
                         const ASCIIFont *font, OLED_ColorMode color);

/**
 * @brief 使用外部 ASCII 字体描述符绘制字符串到显存
 * @param x: 首字符左上角横坐标
 * @param y: 首字符左上角纵坐标
 * @param string: 以空字符结尾的 ASCII 字符串
 * @param font: ASCII 字体描述符
 * @param color: 正常或反色绘制模式
 * @return 无
 * @note 字符串不自动换行，超出屏幕右边界后停止绘制。
 */
void OLED_PrintASCIIString(uint8_t x, uint8_t y, const char *string,
                           const ASCIIFont *font, OLED_ColorMode color);

/**
 * @brief 使用通用字体描述符绘制 UTF-8 字符串到显存
 * @param x: 首字符左上角横坐标
 * @param y: 首字符左上角纵坐标
 * @param string: 以空字符结尾的 UTF-8 字符串
 * @param font: 通用字体描述符
 * @param color: 正常或反色绘制模式
 * @return 无
 * @note 当前随附 font8x6 仅包含 ASCII；未提供字模的非 ASCII 字符显示为空格。
 */
void OLED_PrintString(uint8_t x, uint8_t y, const char *string,
                      const Font *font, OLED_ColorMode color);

#ifdef __cplusplus
}
#endif

#endif

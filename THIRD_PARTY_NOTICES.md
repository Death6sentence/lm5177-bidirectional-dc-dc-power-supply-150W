# 第三方声明

本文件记录发布内容中包含的或用于准备发布内容的第三方源代码和工具。仓库的许可证映射见 [`LICENSE`](LICENSE)。

## 包含的源代码

### ST STM32G4 HAL

位置：[`firmware/Drivers/STM32G4xx_HAL_Driver`](firmware/Drivers/STM32G4xx_HAL_Driver)

随附的 `LICENSE.txt` 规定：作为软件包组成部分提供的 STM32G4 HAL 源代码，其适用许可条款位于 `Package_license`；在本发布树中未发现单独的 `Package_license` 文件。若该组件在软件包之外获得或没有适用许可条款，则适用 BSD-3-Clause 条款。

### STM32G4 CMSIS 设备文件

位置：[`firmware/Drivers/CMSIS/Device/ST/STM32G4xx`](firmware/Drivers/CMSIS/Device/ST/STM32G4xx)

随附的 `LICENSE.txt` 规定：作为软件包组成部分提供的 STM32G4 设备文件，其适用许可条款位于 `Package_license`；在本发布树中未发现单独的 `Package_license` 文件。若该组件在软件包之外获得或没有适用许可条款，则适用 Apache-2.0 条款。

### ARM CMSIS 和 CMSIS-DSP

位置：[`firmware/Drivers/CMSIS`](firmware/Drivers/CMSIS)

包含的 ARM CMSIS 和 CMSIS-DSP 源代码采用 Apache-2.0。完整文本见 [`firmware/Drivers/CMSIS/LICENSE.txt`](firmware/Drivers/CMSIS/LICENSE.txt)。发布内容包含并使用 CMSIS-DSP 函数 `arm_biquad_cascade_df2T_f32` 和 `arm_biquad_cascade_df2T_init_f32`。

### Daniel Hepper font8x8

OLED 字体转换源自 Daniel Hepper 的 font8x8 源代码，该源代码属于 Public Domain。固定使用的源代码 commit 为 `8e279d2d864e79128e96188a6b9526cfa3fbfef9`。上游链接和转换详情见 [`firmware/BSP/OLED/FONT_SOURCE.md`](firmware/BSP/OLED/FONT_SOURCE.md)。

## 外部工具

Keil MDK、STM32CubeMX 和 Altium Designer 是外部工具。本仓库不分发这些工具，它们仍受各自供应商条款约束。

SPDX-License-Identifier: CC-BY-SA-4.0

SPDX-FileCopyrightText: 2026 LM5177 Power Converter Project Contributors

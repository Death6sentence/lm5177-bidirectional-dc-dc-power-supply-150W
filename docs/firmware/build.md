# 固件构建指南

## 目标器件与工具版本

固件面向 STM32G431KBTx 器件系列，具体为 LQFP32 封装的 STM32G431KBT6。项目元数据记录的版本如下：

- STM32CubeMX 6.17.0
- STM32Cube firmware package for STM32G4, FW_G4 1.6.3
- Keil MDK-ARM V5.32
- ARMCLANG 6.24
- Keil STM32G4xx DFP 2.2.0
- CMSIS pack 6.3.0
- CMSIS-DSP 1.16.2

## 打开项目

在受支持的 Keil 安装中打开 [`firmware/MDK-ARM/LM5177MCU.uvprojx`](../../firmware/MDK-ARM/LM5177MCU.uvprojx)。预期目标名称为 `LM5177MCU`；项目配置为在目标输出目录生成 `LM5177MCU` 输出文件和 hex 文件。

本环境未执行 Keil 构建。必须另行使用所需的厂商工具和硬件完成并确认构建、烧录和目标运行成功。

## CubeMX 注意事项

[`firmware/LM5177MCU.ioc`](../../firmware/LM5177MCU.ioc) 是 CubeMX 源配置。重新生成代码可能替换本地修改或改变外设初始化。接受生成结果前请比较变更，保留应用代码和板级支持代码，并在重新生成后再次检查 ADC rank、OPAMP 路由、时钟、TIM3 设置和项目路径。生成文件和构建产物已被忽略，不属于本次源代码发布内容。

## 源代码布局

应用代码位于 [`firmware/App`](../../firmware/App)，板级支持代码位于 [`firmware/BSP`](../../firmware/BSP)，STM32 和 CMSIS 依赖位于 [`firmware/Drivers`](../../firmware/Drivers)。IMON 和 CC 设计说明见 [`firmware/IMON_CALCULATION.md`](../../firmware/IMON_CALCULATION.md)。

SPDX-License-Identifier: CC-BY-SA-4.0

SPDX-FileCopyrightText: 2026 LM5177 Power Converter Project Contributors

# LM5177 电源转换器项目

本仓库是 LM5177 双向升降压转换器控制器及参考项目，包含 STM32G4 固件源代码、Altium 设计源文件、机械外壳文件，以及用于项目验证的测量工作簿。

## 项目图示

![两个 LM5177 功率板共享直流输入：功率板 A 输出带直流偏置的正弦电压，功率板 B 输出恒定电压，负载使用两板差分输出](docs/images/paper/figure-3-3-dual-board-collaboration.png)

图 3-3 双板协同 DC-AC 输出示意（源自 GitHub 用户 `Death6sentence` 的论文《基于 LM5177 的双向 DC-DC 电源设计》，原图 3-3；从 PDF 裁剪）。

图 3-3 是双板应用的概念图，不代表本次发布已完成硬件或性能台架验证。

图中给出的概念关系为：`VA(t)=10+5sin(2π·50t) V`，`VB(t)=10 V`，`VAC(t)=VA(t)-VB(t)=5sin(2π·50t) V`；这些是论文图中的标注，不是测量结果。

![电流经 ADC 采样后进行分段 DTRK 控制，并通过 400 点正弦查表，经 PWM/DTRK 或 DAC/SS-ATRK 路径生成控制量](docs/images/paper/figure-3-2-control-algorithm-flow.png)

图 3-2 恒流控制与正弦波形生成流程（源自 GitHub 用户 `Death6sentence` 的论文《基于 LM5177 的双向 DC-DC 电源设计》，原图 3-2；DOCX 内嵌原图，未重绘）。

图 3-2 概括固件和控制意图，实际行为仍须结合源代码和目标验证判断。

图 3-2 的两条控制路径分别为：400 点正弦查表后通过数字 PWM 调节 DTRK，或通过模拟 DAC 输出 SS-ATRK 电压；此等价描述不构成运行时验证。

完整的图片来源、处理记录和许可证说明见 [`docs/images/README.md`](docs/images/README.md)。

## 仓库目录

- [`firmware/`](firmware/): STM32G431 固件、CubeMX 配置、Keil 工程、驱动和设计说明。
- [`hardware/altium/`](hardware/altium/): Altium 原理图、PCB、工程、库和输出作业源文件。
- [`mechanical/`](mechanical/): STEP 源模型及配套的 OBJ、MTL 和纹理文件。请参阅 [`mechanical/README.md`](mechanical/README.md)。
- [`test-data/`](test-data/): 测量工作簿，包括公式和缓存结果。
- [`docs/`](docs/): 构建、源文件使用、验证和发布指南。
- [`LICENSE`](LICENSE): 仓库许可证映射及完整许可证文本索引。
- [`LICENSES/`](LICENSES/): GPL、CERN-OHL-S 和 CC-BY-SA 许可证文本。

## 关键信息

- 控制器参考型号：Texas Instruments LM5177 双向升降压转换器。
- MCU 目标型号：STM32G431KBTx，具体为 LQFP32 封装的 STM32G431KBT6。
- PWM 控制在当前 8 MHz 定时器时钟配置下使用 TIM3，频率为 250 kHz。DTRK 比较值范围为 0 到 32。
- 固件包含 IMON 采样、偏移校准、显示和遥测通路、保护检查，以及离散 CC 控制环路。
- Altium 发布内容包含可编辑的设计源文件，不包含制造输出文件包。

## 快速开始

1. 打开 Keil 工程前，请阅读[固件构建指南](docs/firmware/build.md)。
2. 编译或检查 PCB 工程前，请阅读[Altium 源文件指南](docs/hardware/altium.md)。
3. 使用工作簿前，请阅读[测试数据指南](docs/verification/test-data.md)。
4. 使用[发布检查清单](docs/verification/release-checklist.md)，区分已完成的静态检查与仍需目标硬件或厂商工具的检查。
5. 如需修改，请阅读 [`CONTRIBUTING.md`](CONTRIBUTING.md) 和[第三方声明](THIRD_PARTY_NOTICES.md)。

## 安全与验证状态

电力电子设备可能使用户暴露于危险电压、电流、储能、热量和意外启动风险。请采用合适的隔离、熔断、限流、防护设备、探测方法和放电流程。未经适当的工程评审，不要连接或给本设计上电。

本发布版本仅记录源码级和静态验证，不宣称已完成认证、具备量产条件、成功烧录目标设备、通过台架验证、完成机械适配或制造验证。工作簿包含项目数据和公式，但其中数值必须根据用户自己的配置进行核对。

## 许可证

本仓库采用 [`LICENSE`](LICENSE) 中所述的混合许可证模式：固件采用 GPL-3.0-or-later，硬件和机械文件采用 CERN-OHL-S-2.0，文档和测试数据采用 CC-BY-SA-4.0。具有单独条款的组件和工具列于 [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md)。

## 第三方组件

有关 STM32 HAL 和 CMSIS 源文件、Daniel Hepper 字体源文件及外部厂商工具的声明，请参阅 [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md)。

SPDX-License-Identifier: CC-BY-SA-4.0

SPDX-FileCopyrightText: 2026 LM5177 Power Converter Project Contributors

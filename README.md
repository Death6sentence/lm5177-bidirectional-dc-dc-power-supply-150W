# LM5177 150 W 双向 DC-DC 电源设计

## 摘要

本仓库是一个面向研究、复核和后续开发的源文件与参考发布，围绕 Texas Instruments LM5177 双向升降压控制器、STM32G431KBT6 固件、Altium 设计源文件、机械模型和测量工作簿组织。本文以项目维护者批准的 150 W 目标包络为主线，说明设计动机、系统分工、当前源代码证据和验证边界。仓库不是经认证的产品发布，也不是已完成台架验证的功率模块。目标规格、历史论文记录和当前实现状态在下文分开列出，避免把设计意图误读为额定能力或安全保证。

## 设计目标与验证边界

下表是项目维护者批准的设计目标，属于未经验证的目标参数，不是已证明的额定值、连续工作能力或安全限值。

| 项目 | 批准目标 |
| --- | --- |
| 输入电压 | 10 至 30 V |
| 输出电压 | 0 至 24 V 可调 |
| 标称输出电压 | 16 V |
| 最大目标输出电流 | 6.25 A |
| 最大目标功率 | 仅在 24 V × 6.25 A 时为 150 W |
| 目标包络 | `Vout × Iout <= 150 W` 且 `Iout <= 6.25 A` |

这里的 0 V 是调节范围下界，不代表在带载条件下已经证明可以稳定调节到 0 V。16 V × 6.25 A = 100 W，24 V × 6.25 A = 150 W。150 W 只描述批准目标包络的一个端点，不能外推为所有输入、输出、电流和温度组合下的能力。

### 历史论文与工作簿记录

历史资料必须保持原样理解：论文基线为 16 V / 6.25 A / 100 W，工作簿最高记录约为 15.9 V / 6.51 A / 103.5 W。两者都不能验证 150 W，历史记录也没有被改写为 150 W 证据。

### 当前实现差距

当前固件源代码仍暴露 0 至 16000 mV 的 UI 电压目标范围，以及 0 至 2800 mA 的恒流目标范围。这些当前源代码设置尚未实现新的 0 至 24 V、6.25 A 目标，需要协调固件、硬件、保护和控制工作。这里是对当前源代码的事实记录，不是对实现质量的缺陷判定。

## 1. 项目背景与目标

LM5177 提供双向升降压功率级所需的控制基础，适合在输入和输出电压范围交叠或变化时进行能量双向传递。本项目将其与 STM32G4 数字控制、采样、显示和遥测路径结合，形成可编辑、可追溯的参考设计资料。

项目目标不是只给出一张电路图，而是保留从功率级、控制固件、机械载体到测量数据的关联入口。150 W 目标用于约束后续设计审查和验证计划，当前发布则侧重源文件可读性、历史资料保真和边界透明度。

## 2. 系统架构与 LM5177 角色

系统可分为四层：

1. 功率层由 LM5177、开关器件、磁性器件、输入输出滤波和相关检测网络组成。
2. 控制层由 STM32G431KBT6 采集 VIN、VOUT、IMON 和温度，并生成 DTRK、PWM、DAC 或 SS-ATRK 相关控制量。
3. 人机与通信层包括 OLED 显示和 UART 遥测。
4. 机械层提供功率板及控制板的安装和外壳参考模型。

LM5177 在此架构中承担双向 DC-DC 控制器角色，具体功率能力仍取决于整机的输入条件、器件选型、磁性器件、热路径、布局、补偿、保护和验证结果，不能由控制器型号或目标功率单独推出。

## 3. 双板概念运行方式

历史论文中的双板概念将两块 LM5177 功率板置于共享直流输入下，通过两路输出的差分关系构造交流分量。图 3-3 是概念说明，不是 150 W 图，也不是实测验证。

![双板协同概念图：功率板 A 输出带直流偏置的正弦电压，功率板 B 输出恒定电压，负载使用两板差分输出](docs/images/paper/figure-3-3-dual-board-collaboration.png)

图 3-3 双板协同 DC-AC 输出概念图。等价文本为：功率板 A 与功率板 B 共享直流输入，负载取两路输出之差。图中历史论文标签原样保留：`VA(t)=10+5sin(2π·50t) V`，`VB(t)=10 V`，`VAC(t)=VA(t)-VB(t)=5sin(2π·50t) V`。这些方程是历史图示的概念关系，不是本仓库的测量结果。

## 4. 硬件设计与 Altium 源文件

功率板的硬件资料以可编辑 Altium 工程为权威入口：[`hardware/altium/PCB_Project1.PrjPCB`](hardware/altium/PCB_Project1.PrjPCB)。该目录包含原理图、PCB、库和相关设计源文件。功率板总览如下，图片用于帮助读者定位板级功能区域，不替代原理图、网络检查或制造审查。

![LM5177 功率板总览，展示功率级、检测和接口区域](docs/images/paper/figure-2-2-power-pcb.png)

图 2-2 功率板总览，来源和处理记录见 [`docs/images/README.md`](docs/images/README.md)。发布内容是经过整理的源文件，未包含制造输出文件包。本仓库没有声称已运行 Altium 编译、ERC 或 DRC，也没有声称已完成 PCB 制造、装配或上电验证。

## 5. STM32G431KBT6 固件架构

固件目标器件为 STM32G431KBT6，采用 LQFP32 封装。`firmware/` 按 App、BSP、Core、Drivers、CubeMX 和 Keil 等区域组织，分别承载应用控制、板级支持、核心工程文件、外设驱动、初始化配置和构建工程。

源代码提供采样、恒流控制、显示、串口遥测、偏移校准和保护状态处理的实现线索，但目标构建、烧录和运行仍未执行。因此，本 README 不把当前源代码描述为已经支持 24 V 或 6.25 A 目标设置。

### 5.1 定时与控制量

源代码中可追溯到以下配置事实：TIM3 使用 8 MHz 时钟，预分频值为 0，周期值为 31，对应配置的 DTRK PWM 为 250 kHz。DTRK 比较值范围为 0 至 32，软件控制等级 0 至 1000 映射到比较值 0 至 32。恒流控制周期为 10 ms，显示和遥测周期为 100 ms。

### 5.2 控制流程与正弦生成

控制资料中的图 3-2 展示了电流采样、分段控制和正弦参考生成的关系。固件包含 400 点正弦表，并保留 DTRK/PWM 和 DAC/SS-ATRK 两类控制路径的实现线索。该图是控制意图和源代码结构的辅助说明，不能替代运行时波形、闭环稳定性或功率测试。

![恒流控制与正弦波形生成流程，包含 400 点正弦表以及 DTRK/PWM、DAC/SS-ATRK 路径](docs/images/paper/figure-3-2-control-algorithm-flow.png)

图 3-2 控制算法流程，来源和处理记录见 [`docs/images/README.md`](docs/images/README.md)。

## 6. 采样、校准、保护、显示与遥测

固件采集 IMON、VIN、VOUT 和温度，并将结果用于显示、UART 遥测和控制状态处理。IMON 偏移校准仅保存于 RAM，重新上电后的状态不能由此推断。保护状态具有锁存行为，具体阈值未在本 README 中发布为保证值，使用者必须结合源代码、器件数据手册和目标硬件审查。

采样网络和模型应区分理论值与源代码默认值：

| 通道或模型 | 记录 |
| --- | --- |
| VIN 分压 | 680 kΩ / 73.5 kΩ |
| VOUT 分压 | 10 kΩ / 990 Ω |
| IMON 理论模型 | 0 A 时 1.65 V，斜率 46.4 mV/A |
| 固件默认模型 | 0 A 时 1.590 V，斜率 50.0 mV/A |

上述差异需要在具体硬件和校准流程中核对，不能把任一模型直接解释为 150 W 电流精度或保护能力。OLED 用于本地状态展示，UART 用于遥测输出，二者同样需要目标板和仪器验证。

## 7. 机械外壳

机械资料用于表达安装关系和外壳设计意图。推荐以 STEP 文件作为可编辑源，OBJ、MTL 和 PNG 作为配套资产。以下预览图帮助读者快速识别外壳和板级空间关系。

![LM5177 外壳预览，展示板卡安装空间和外壳外形](mechanical/lm5177-enclosure-preview.png)

当前发布没有完成装配适配、尺寸公差、热设计、制造工艺或批量一致性验证。预览图不构成 fit、tolerance、thermal 或 manufacturing 结论。

## 8. 仓库内容与权威入口

| 路径 | 内容与用途 |
| --- | --- |
| [`firmware/`](firmware/) | STM32G431KBT6 固件、CubeMX、Keil、BSP、核心代码和驱动 |
| [`hardware/altium/`](hardware/altium/) | Altium 可编辑工程、原理图、PCB 和设计源 |
| [`mechanical/`](mechanical/) | STEP 源模型及 OBJ、MTL、PNG 配套资产 |
| [`test-data/`](test-data/) | 测量工作簿、公式和序列化缓存结果 |
| [`docs/`](docs/) | 固件、硬件、验证、发布和图片来源指南 |
| [`LICENSE`](LICENSE) | 仓库许可证映射和许可证文本索引 |
| [`LICENSES/`](LICENSES/) | GPL、CERN-OHL-S、CC-BY-SA 等许可证文本 |
| [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) | HAL、CMSIS、字体和外部工具的第三方声明 |

Altium 的权威工程入口是 [`hardware/altium/PCB_Project1.PrjPCB`](hardware/altium/PCB_Project1.PrjPCB)，固件使用前应先阅读 [`docs/firmware/build.md`](docs/firmware/build.md)，图片来源应以 [`docs/images/README.md`](docs/images/README.md) 为准。

## 9. 开始使用

1. 阅读 [`docs/firmware/build.md`](docs/firmware/build.md)，再检查 Keil 工程和固件源码。
2. 阅读 [`docs/hardware/altium.md`](docs/hardware/altium.md)，再打开或审查 Altium 源文件。
3. 阅读 [`docs/verification/test-data.md`](docs/verification/test-data.md)，再解释测量工作簿。
4. 对照 [`docs/verification/release-checklist.md`](docs/verification/release-checklist.md)，区分静态证据和仍需工具、硬件或实验室的项目。
5. 贡献前阅读 [`CONTRIBUTING.md`](CONTRIBUTING.md)、[`SECURITY.md`](SECURITY.md) 和 [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md)。

工作簿的静态元数据为：9 个工作表、103 个公式、103 个序列化缓存节点、97 个非空缓存、6 个有意产生的空字符串结果、8 个 Excel 表格、0 个外部链接。这些是文件结构信息，不是功率性能证据。

## 10. 验证状态矩阵

| 项目 | 状态 | 证据边界 |
| --- | --- | --- |
| 150 W 目标包络 | 项目维护者批准，未验证 | 设计目标，不是额定值或安全限值 |
| 源代码与文档静态检查 | 已记录 | 以当前仓库源文件和文档为依据 |
| 历史论文基线 | 已记录 | 16 V / 6.25 A / 100 W，未改写 |
| 历史工作簿最高记录 | 已记录 | 约 15.9 V / 6.51 A / 103.5 W，不能验证 150 W |
| Keil 目标构建 | 未执行 | 未声称编译成功 |
| 固件烧录与目标运行 | 未执行 | 未声称目标板运行 |
| 台架测试 | 未执行 | 未提供新目标的电压、电流或功率证明 |
| Altium 编译、ERC、DRC | 未执行 | 未声称工具检查通过 |
| 机械适配、公差、热和制造验证 | 未执行 | STEP、OBJ、MTL、PNG 仅为设计资产 |
| 150 W 硬件、磁性器件、保护、稳定性、效率检查 | 未执行 | 不得视为已满足目标 |
| 0 V 全范围带载行为 | 未执行 | 0 V 仅为调节下界 |

## 11. 安全与限制

电力电子设备可能产生危险电压、电流、储能、热量和意外启动。使用前必须由具备相应经验的人员完成隔离、熔断、限流、预充、放电、探测、接地和防护评审。不要在缺少合适仪器、负载、探头、熔断和防护设备时连接或给本设计上电。

150 W 目标对应的硬件能力、热能力、磁性器件适用性、保护动作、环路稳定性、效率和 0 V 全范围行为均未验证。仓库不构成认证、量产、额定功率或安全承诺。任何使用者都必须根据自己的器件、PCB、装配、环境和测量方案重新评估风险。

## 12. 图像来源与发布溯源

论文图片来自 GitHub 用户 `Death6sentence` 的论文《基于 LM5177 的双向 DC-DC 电源设计》相关图件。图 2-2 和图 3-2 从 DOCX 媒体资源提取，图 3-3 从 PDF 页面紧密裁剪，均未重绘。它们是解释性文档资产，不是硬件或性能验证的独立证据。完整来源、处理记录和 CC-BY-SA-4.0 说明见 [`docs/images/README.md`](docs/images/README.md)。

## 13. 许可证、第三方、贡献与安全报告

仓库采用混合许可证：固件按 GPL-3.0-or-later，硬件和机械文件按 CERN-OHL-S-2.0，文档和测试数据按 CC-BY-SA-4.0。具体映射以 [`LICENSE`](LICENSE) 为准。STM32G4 HAL、CMSIS、CMSIS-DSP、Daniel Hepper font8x8 以及 Keil、STM32CubeMX、Altium Designer 等第三方源代码和工具的条款见 [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md)。

欢迎提交范围明确、可复现且有证据边界的贡献。请先阅读 [`CONTRIBUTING.md`](CONTRIBUTING.md)，不要提交生成的制造输出、构建目录、缓存、个人路径或凭据。疑似漏洞请按 [`SECURITY.md`](SECURITY.md) 的私密报告流程处理，不要在公开问题中发布凭据、私人数据或完整利用细节。

SPDX-License-Identifier: CC-BY-SA-4.0
SPDX-FileCopyrightText: 2026 LM5177 Power Converter Project Contributors

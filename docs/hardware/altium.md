# Altium 源文件指南

## 入口文件

在受支持的 Altium Designer 安装中，将 [`hardware/altium/PCB_Project1.PrjPCB`](../../hardware/altium/PCB_Project1.PrjPCB) 作为项目入口文件打开。

本次发布包含以下源文件和项目文件：

- `BMC096C.schdoc`
- `Copy of ASCIIPCB2.pcbdoc`
- `PCB_Project1.IntLib`
- `PCB_Project1.OutJob`
- `PCB_Project1.PrjPCB`
- `PCB_Project1.PrjPCBStructure`
- `libraries/Copy of ASCIIPCB2.PcbLib`
- `libraries/PCB_Project1.SCHLIB`

## PCB 概览

![双向 DC-DC 电源功率板 PCB 概览：VIN/VOUT 端子位于两侧，上方为开关和功率区域，中部及下方集中控制器与信号电路](../images/paper/figure-2-2-power-pcb.png)

图 2-2 双向 DC-DC 电源功率板 PCB 概览（源自 GitHub 用户 `Death6sentence` 的论文《基于 LM5177 的双向 DC-DC 电源设计》，原图 2-2；DOCX 内嵌原图，未重绘）。

图中可见 VIN/VOUT 端子分布在 PCB 两侧，上方区域包含开关和功率部分，中部及下方区域集中控制器与信号电路。此图仅供概览，编辑源文件和 Altium 检查结果仍是权威依据；图片来源、处理记录和许可证说明见 [`../images/README.md`](../images/README.md)。

项目使用相对文档路径，并引用随附的集成库。输出作业同样使用相对于项目的输出路径。[`SANITIZATION.md`](../../hardware/altium/SANITIZATION.md) 记录了保持字节数不变的 PCB 路径替换、输出作业变更、哈希值和排除的材料。

本次发布未记录源文件所对应的确切 Altium Designer 版本，这是可复现性方面的限制。

制造输出、项目日志、缓存、预览和其他生成的输出文件均有意未包含。不要将本次源文件发布视为制造资料包。

## 必需检查

使用设计前，请在受支持的 Altium Designer 安装中编译项目、检查项目设置和设计规则、运行 ERC 和 DRC，并检查生成的输出文件。WSL 准备期间未在 Altium 中打开本次发布内容，也未执行 DRC 检查。

SPDX-License-Identifier: CC-BY-SA-4.0

SPDX-FileCopyrightText: 2026 LM5177 Power Converter Project Contributors

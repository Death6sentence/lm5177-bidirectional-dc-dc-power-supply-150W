# 测试数据

工作簿为 [`test-data/LM5177_measurements.xlsx`](../../test-data/LM5177_measurements.xlsx)，包含以下 9 个工作表：

1. `README`
2. `Test_Conditions`
3. `Steady_State`
4. `Voltage_Feedback`
5. `Output_Ripple`
6. `CC_Error`
7. `Regulation`
8. `Data_Dictionary`
9. `Provenance`

静态检查发现 103 个公式；全部 103 个公式单元格都有序列化缓存节点，其中 97 个非空，6 个有意表示空字符串结果。工作簿还包含 8 个 Excel Tables，且有 0 个外部链接。使用兼容的电子表格应用打开工作簿时，公式应自动、完整并强制重新计算。这些缓存节点不能独立证明公式或测量结果有效。

工作簿是记录和计算辅助工具，不是认证文件。请根据你自己的台架设置、接线、仪器、工作限值和测试流程，逐项检查每个数值、公式、单位及空行假设。本仓库不声称该工作簿能够证明另一套构建的通过或失败结果。

该工作簿采用 CC-BY-SA-4.0 许可。请参阅 [`LICENSE`](../../LICENSE)。

SPDX-License-Identifier: CC-BY-SA-4.0

SPDX-FileCopyrightText: 2026 LM5177 Power Converter Project Contributors

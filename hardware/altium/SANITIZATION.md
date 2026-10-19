# Altium 源文件发布脱敏记录

## 包含的源文件

从 `Altium_LM5177_2026-09-28` 复制，未重命名：

- `BMC096C.schdoc`
- `Copy of ASCIIPCB2.pcbdoc`
- `PCB_Project1.IntLib`
- `PCB_Project1.OutJob`
- `PCB_Project1.PrjPCB`
- `PCB_Project1.PrjPCBStructure`
- `libraries/Copy of ASCIIPCB2.PcbLib`
- `libraries/PCB_Project1.SCHLIB`

`PCB_Project1.PrjPCB` 保持不变。其文档路径为相对路径，并继续引用所包含的集成库。

## 排除的内容

历史记录、日志、调试文件、生成的输出、预览文件、缓存及运行时元数据、`.codegraph`、`.omo`、`PCB_Project1.Dat`、源 `PCB_Project11` 目录本身、`Project Logs for PCB_Project1`、`Project Outputs for PCB_Project1` 和 `Status Report.Txt` 均未复制。

## 脱敏变更

仅在复制的 PCB 文档中，将原始 26 字节 Windows 桌面绝对路径的两处出现替换为 26 字节相对路径 `.\Project Outputs\PCBData\`。替换保持字节数不变：PCB 大小仍为 `6,774,272` 字节。

复制的输出作业仅将以下三个设置改为相对于项目的值：

```text
OutputFilePath2=.\Project Outputs\BMC096C11.pdf
OutputBasePath2=.\Project Outputs\
RelativeOutputPath2=.\Project Outputs\
```

未有意更改原理图、PCB 设计内容、项目 GUID、库内容或文件名。

## PCB 验证记录

| 项目 | 值 |
|---|---|
| 原始 PCB SHA-256 | `cb99d6c01263aa1404500be1ededbf51a38ab38b2ab0029dffeeba7a7720ba84` |
| 脱敏后 PCB SHA-256 | `fc46f65e41133e62724bb86858b6ca806d46edee4dbe683a268dab25de293801` |
| 原始字节大小 | `6,774,272` |
| 脱敏后字节大小 | `6,774,272` |
| 二进制差异汇总 | `48` 个差异字节位置，仅位于两处路径区域 |

源文件按不可变文件处理。已针对发布内容执行 `file`、ASCII 和 UTF-16 字符串扫描、字节大小检查、SHA-256 检查以及二进制差异汇总。

由于无法在 WSL 中打开 Altium，仍建议在受支持的 Windows 安装环境中使用 Altium 完成最终的打开、保存和 DRC 检查。

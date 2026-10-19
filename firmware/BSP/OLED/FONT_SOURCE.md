# OLED 字体来源

`BSP/OLED/font.c` 包含 Daniel Hepper 公共领域字体源的确定性 6x8 转换版本：

- 仓库：<https://github.com/dhepper/font8x8>
- 提交：[`8e279d2d864e79128e96188a6b9526cfa3fbfef9`](https://github.com/dhepper/font8x8/tree/8e279d2d864e79128e96188a6b9526cfa3fbfef9)
- 源文件：[`font8x8_basic.h`](https://github.com/dhepper/font8x8/blob/8e279d2d864e79128e96188a6b9526cfa3fbfef9/font8x8_basic.h)
- 上游 README 和文件头声明：`License: Public Domain`。

仅包含可打印 ASCII U+0020 至 U+007E，并按代码点顺序排列。每个源字形由八个按行排列的字节组成。对于每个源行 `y`，六个输出列按如下方式构成：

1. `source[0] | source[1]`
2. `source[2]`
3. `source[3] | source[4]`
4. `source[5]`
5. `source[6] | source[7]`
6. `0x00` 间隔列

生成的字节仍按列排列，其中位 `y` 表示源行 `y`，并保留现有的 `afont8x6` 和 `font8x6` API 及描述符。

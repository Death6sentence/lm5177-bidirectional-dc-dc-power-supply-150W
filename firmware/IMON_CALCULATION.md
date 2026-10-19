# IMON 电流采样与 CC Mode

## 1. 硬件模型与标定

当前 LM5177 功率级分流器为 `4mOhm`，IMON 典型跨导为 `1mS`。IMONOUT 接到 PA1 的 `OPAMP3_VINP`，OPAMP3 配置为电压跟随器，输出通过 MCU 内部路径送入 ADC2 的 `VOPAMP3_ADC2`，不占用外部运放输出引脚。IMON 偏置网络为 `23.2k` 上拉到 `3.3V`、`23.2k` 下拉到 AGND，因此：

```text
Rth = 23.2k || 23.2k = 11.6kOhm
Vbias = 3.3V / 2 = 1.65V
VIMON = 1.65V + I(A) * 4mOhm * 1mS * 11.6kOhm
       = 1.65V + 0.0464V/A * I(A)
```

当前尚无该 `4mOhm` 与 `23.2k` 网络的板上多点拟合数据。上述 `1.65V` 与 `46.4mV/A` 是硬件理论计算；当前固件实现不直接使用这些理论默认值，而使用 `1.590V` 零点和 `50.0mV/A` 灵敏度。仓库没有记录这些实现默认值的推导或板上多点拟合来源，用户必须针对自己的硬件进行核验或校准。运行时可通过 `BSP_ADC_ImonCalibrateOffset()` 按外部参考电流更新本次上电期间的零点，但该单点校准不改变 `50.0mV/A` 灵敏度。ADC 参考电压为 `3.3V`，12 位满量程为 `4095`，因此固件实现默认模型为：

```text
ADC = 4095 * VIMON / 3.3V
    ≈ 1973.05 + 62.05 * I(A)
```

因此按固件实现默认模型，每个 ADC count 约对应 `16.12mA`，正电流会使 IMON 电压升高。负向电流仍属于实现默认直线的外推，后续若要求双向精度，应补充正负电流标定点。分流器、LM5177 跨导、电阻、ADC 参考和零点误差均有容差，不能把理论计算或实现默认值当作保证精度。

从 IMON 差分量程 `±50mV` 和 `4mOhm` 分流器估算，差分输入对应的物理测量幅度约为 `50mV / 4mOhm = 12.5A`。CC 目标范围和软件过流阈值由应用层独立配置，本次硬件模型修改不自动改变这些保护参数。

## 2. ADC 配置与整数换算

ADC1 的 Rank 1 为 TEM（PA0/`ADC1_IN1`），Rank 2 为 VIN（PA3/`ADC1_IN4`）。ADC2 的 Rank 1 为 VOUT（OPAMP2 内部输出/`ADC_CHANNEL_VOPAMP2`），Rank 2 为 IMON（OPAMP3 内部输出/`ADC_CHANNEL_VOPAMP3_ADC2`）。两个 ADC 均为 12 位、16 倍过采样、右移 4 位，应用使用右移后的 `0` 到 `4095` 原始值。`BSP_ADC_Init()` 在校准和首次采样前启动 OPAMP2 与 OPAMP3；每次读取完整 ADC1、ADC2 两个 Rank，再按 Rank 提取 VIN、VOUT 和 IMON，避免扫描序列错位。

IMON 结果保留原始 ADC 值和有符号毫安值：

```c
typedef struct
{
  uint16_t raw;
  int32_t milliamps;
} BSP_ADC_ImonSample;
```

固件在 `BSP/ADC/bsp_adc.c` 中用十分之一微伏定点数保存参数：

```c
#define BSP_ADC_IMON_DEFAULT_BIAS_UV_X10 15900000LL
#define BSP_ADC_IMON_SENSITIVITY_UV_X10_PER_MA 500LL
```

`15900000` 表示 `1.590000V`，`500` 表示 `50.0uV/mA`。`1A` 对应约 `62.05` 个 ADC counts。

正向换算可统一写成：

```text
numerator = raw * 3300mV * 1000 * 10 - 15900000 * 4095
denominator = 500 * 4095
I_mA = signed_round(numerator / denominator)
```

实际实现使用有符号 64 位中间量，并对正、负数采用对称的 round-to-nearest。反向换算先按相同定点尺度计算 `1.590V + I(mA) * 50.0uV/mA`，再将结果饱和到 `0..4095`。`BSP_ADC_ImonCalibrateOffset()` 只更新 RAM 中的零点，仍使用固定的 `50.0uV/mA` 灵敏度。CC 闭环的目标、测量值和误差都使用 ADC counts，反馈直接使用 `imon_sample.raw`；显示和保护使用换算后的 `milliamps`。

主菜单的 `AUTO CAL IMON` 会把 EN 拉低，等待 `100ms` 后连续采集 `16` 次 IMON ADC 并取四舍五入平均值，再以 `0mA` 更新本次上电期间的 RAM 零点。结果页保持 EN 低电平，按键返回主菜单时恢复 `POWER SWITCH` 中保存的 EN 状态。该功能不操作 DAC，只校准零点，不修改 `50.0uV/mA` 灵敏度，也不写入非易失存储器。

独立的 `DAC CAL IMON` 保持 EN 状态不变，将 DAC 输出置为 `0mV` 后复用相同的 `100ms` 等待和 `16` 次平均采样流程。结果页保持 DAC 为 `0mV`，按键返回主菜单时恢复默认 `1100mV`。该模式同样只更新 RAM 零点，不改变 `50.0uV/mA` 灵敏度或写入非易失存储器。

## 3. CC Mode 控制行为

菜单中的 CC 目标为 `0..2800mA`，每次调整 `50mA`。进入 CC Mode 时，目标电流按同一模型反算为目标 ADC counts，DTRK 从进入时的当前值开始调节。控制器不是 CMSIS PID，而是分段可变步进的 DTRK 控制器：

- 前台循环每 `10ms` 读取 IMON，并依次执行保护检查和 CC 控制。若循环错过多个周期，只处理当前到期的一次，错过的周期不补执行。
- CC 死区为 `±2` ADC counts。对于误差幅值 `3..10` counts，输出步长等于误差幅值；误差幅值大于 `10` counts 时，最大步长为 `31` 个软件 DTRK level。
- raw 误差定义为 `target_raw - measured_raw`；正误差表示实测电流低于目标，DTRK 增大。
- 软件输出范围为 `0..1000`。
- DTRK 软件 level 为 `0..1000`，映射到 TIM3 compare `0..32`。
- 显示和 UART 遥测仍按 `100ms` 更新，复用最近一次 CC 控制采样，不另行改变 10ms 控制周期。

CC 控制每次执行前检查 IMON 过流和电压保护条件。触发过流时停止 CC，关闭功率级，使 DTRK 回到空闲值并锁存故障；首次故障采样保留用于状态页显示。保护锁存后，重新启动仍需经过保护条件检查。当前电压保护开关保持现有实现状态，相关 VIN/VOUT 阈值和时钟配置不因本说明改变。

TI 文档 `ZHCACA5/SNVAA51` 中的 10kHz 二阶差分方程，是针对另一种 ATRK 被控对象和参数识别的参考设计。当前项目使用 DTRK，PWM 为 32 个 compare counts，采样与执行器量化方式也不同，因此不能直接复制其系数。这里采用当前硬件的 counts 死区和分段可变 DTRK 步进，参数仍应通过本机台架测试和容差分析整定。

## 4. 相关配置

当前 CubeMX 配置已满足固件要求，当前实现无需额外修改：

```text
PA7 = OPAMP2_VINP (IO0)
OPAMP2 = Follower mode, internally connected
OPAMP2 Internal Output = Enabled
ADC2 Rank 1 = VOPAMP2
PA1 = OPAMP3_VINP (IO2)
OPAMP3 = Follower mode, internally connected
OPAMP3 Internal Output = Enabled
ADC2 Rank 2 = VOPAMP3_ADC2
```

当前系统时钟源为内部 `16MHz HSI`，外部 HSE 不参与当前 PLL 和系统时钟计算：

```text
16MHz HSI
  -> PLLM /5 = 3.2MHz
  -> PLLN ×30 = 96MHz VCO
  -> PLLR /6 = 16MHz SYSCLK
  -> AHB /2 = 8MHz HCLK
  -> APB1 /1 = 8MHz PCLK1
  -> APB2 /1 = 8MHz PCLK2
```

ADC 使用 `ADC_CLOCK_SYNC_PCLK_DIV2`，实际同步 ADC 转换时钟为 `8MHz HCLK / 2 = 4MHz`。ADC12 内核时钟源为 `16MHz SYSCLK`。I2C2 使用 `8MHz PCLK1` 和 CubeMX 生成的 `0x00201D2B Timing`，USART1 使用 `8MHz PCLK2`，保持 `115200 8N1`。SysTick 由 HAL 按 `8MHz SystemCoreClock` 重装，应用的毫秒延时和周期任务保持原时长。

TIM1 和 TIM16 保持原有计数配置。TIM3 计数时钟为 `8MHz`，预分频为 0、周期为 `31`，因此 DTRK PWM 为 `250kHz`，每周期 `32` 个 compare counts。DTRK compare 范围为 `0..32`；TIM1/SYNC 和 TIM16/FAN 不随本功能改变。

## 5. 相关文件

- `BSP/ADC/bsp_adc.c`：ADC 读取、IMON 理想模型、整数正反向换算和饱和处理。
- `BSP/ADC/bsp_adc.h`：IMON 原始值和有符号计算电流数据结构及接口。
- `Core/Src/adc.c`：ADC1/ADC2 通道、Rank、过采样和采样时间配置。
- `Core/Src/opamp.c`：OPAMP2/OPAMP3 跟随器、PA7/PA1 输入和内部输出配置。
- `App/app.c`：测量调度、IMON 自动零点校准、EN 电源开关、CC 控制和保护判定。
- `App/cc_pid.c`：基于 ADC counts 的死区与 DTRK 分段可变步进控制器。
- `App/app_menu.c`：电源开关、IMON 校准、CC 目标及菜单状态转换。
- `App/app_menu.h`：菜单页面、动作、CC 目标范围 `0..2800mA` 及相关常量。
- `BSP/PWM/bsp_pwm.c`：DTRK level 到 TIM3 compare `0..32` 的映射。
- `App/app_status.c`：IMON 电压与计算电流的显示格式。
- `App/app_telemetry.c`：IMON 原始值、计算电流和 CC counts 遥测。
- `Core/Src/main.c`：16MHz HSI、PLL、AHB 和 APB 时钟配置。
- `MDK-ARM/LM5177MCU.uvprojx`：Keil 目标 CPU 时钟为 8MHz；保留的 `HSE_VALUE=25000000U` 不参与当前 HSI PLL 计算。

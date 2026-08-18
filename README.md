# Vehicle Sensor Lab

基于 i.MX6ULL 的 Linux 车载传感器数据采集实验平台。

项目围绕“内核驱动—用户态服务—交互界面”构建完整数据链路，用于实践 Linux 字符设备、设备树、I2C、事件驱动和嵌入式应用集成。前期通过软件事件逐步完善驱动接口，后期接入真实 IMU，采集振动、姿态与冲击数据。

> 本项目是学习与工程验证原型，不属于汽车功能安全系统，也不用于真实车辆安全决策。

## 项目目标

- 在 i.MX6ULL 上建立可复现的外部内核模块开发环境；
- 实现 `/dev/vehicle_sensor` 字符设备及稳定的用户态接口；
- 支持阻塞读取、非阻塞访问、`poll` 和 `ioctl`；
- 使用等待队列、环形缓冲区和锁管理连续传感器事件；
- 通过设备树和 platform driver 描述、管理设备资源；
- 接入 I2C IMU，采集真实运动、振动和冲击数据；
- 使用 C/C++ 服务完成采集、诊断和状态管理；
- 后续连接 CLI、Qt 界面和智能网关。

## 系统架构

```text
                 CLI / Qt / SmartEdgeGateway
                            ↕
                  用户态传感器服务
              采集 · 校验 · 诊断 · 状态管理
                            ↕
                  read / poll / ioctl
                            ↕
                  /dev/vehicle_sensor
                            ↕
             Linux 字符设备 / platform driver
             等待队列 · 环形缓冲区 · 并发保护
                            ↕
              软件事件 → I2C IMU / 硬件中断
```

驱动只负责采集、缓存和传递底层事件；车辆状态判断、日志记录、界面显示和网络通信由用户态完成，避免把业务逻辑放入内核。

## 计划能力

### 内核驱动

- 内核模块加载、卸载与版本匹配；
- 字符设备和 `file_operations`；
- 阻塞/非阻塞读取及 `poll` 事件通知；
- UAPI 数据结构与 `ioctl` 控制接口；
- 有界环形缓冲区、事件序列号和溢出统计；
- wait queue、锁和多进程读取行为；
- platform driver、设备树、I2C 与中断。

### 用户态

- C/C++ 数据采集服务；
- 传感器配置、状态管理和异常恢复；
- 事件日志、回放及运行诊断；
- CLI 测试工具；
- Qt 实时状态与曲线显示；
- 后续向智能网关发布设备状态。

### 测试与诊断

- 重复加载、卸载测试；
- 阻塞、非阻塞和多读取者测试；
- 缓冲区边界及非法参数测试；
- 慢消费者、队列溢出和故障注入；
- 事件延迟、丢失数量和长时间运行统计。

## 硬件与开发环境

| 项目 | 当前环境 |
| --- | --- |
| 开发板 | 野火 EBF6ULL S1 Pro |
| SoC | NXP i.MX6ULL，ARM Cortex-A7 |
| 板端系统 | Debian GNU/Linux 10 |
| 板端内核 | Linux `4.19.35-imx6` |
| 内核源码 | Embedfire `ebf_4.19.35_imx6ul` |
| 开发主机 | Windows + WSL |
| 编译方式 | ARM 交叉编译 |
| 联调方式 | SSH / SCP / 串口 |
| 真实传感器 | 后续阶段确定 I2C IMU 型号 |

## 当前进度

项目已经完成 **阶段 0：内核模块构建环境与最小模块闭环**，即将进入阶段 1 的字符设备实现。

已经完成：

- 确认开发板内核版本、架构和模块配置；
- 导出并验证运行内核 `.config`；
- 获取匹配的厂商内核源码；
- 建立独立内核构建目录并完成 `olddefconfig`；
- 验证生成的 `kernelrelease` 为 `4.19.35-imx6`；
- 定位并修复旧版 DTC 与新版主机 GCC 的 `yylloc` 兼容问题；
- 生成完整 `Module.symvers` 并成功交叉编译 ARM 32 位模块；
- 确认模块 `vermagic` 与开发板运行内核匹配；
- 在 i.MX6ULL 上完成 `insmod`、`lsmod`、`dmesg` 和 `rmmod` 闭环；
- 验证初始化函数与退出函数均被内核实际执行。

下一阶段：

- 创建 `/dev/vehicle_sensor`；
- 实现第一版固定事件读取；
- 编写最小用户态 C 测试程序。

详细记录见 [实施进度](./docs/progress.md) 和 [阶段 0 验收记录](./docs/stage0_validation.md)。

## 开发路线

| 阶段 | 交付结果 |
| ---: | --- |
| 0 | 匹配开发板的内核构建环境，最小模块可加载、卸载 |
| 1 | `/dev/vehicle_sensor` 可以返回固定事件 |
| 2 | 支持阻塞读取、`O_NONBLOCK` 和 `poll` |
| 3 | 支持 UAPI 与 `ioctl` 配置、事件注入 |
| 4 | 使用有界环形缓冲区并报告丢失、溢出统计 |
| 5 | 改造为 platform driver，并通过设备树匹配 |
| 6 | 接入 I2C IMU，采集真实运动与冲击数据 |
| 7 | 完成 C++ 采集服务及 CLI/Qt 客户端 |
| 8 | 完成故障注入、长稳测试、性能记录和项目演示 |

每个阶段以可运行结果和可复现记录为验收标准。

## 仓库结构

```text
.
├── driver/                 # Linux 内核模块
├── include/                # 驱动与用户态共享的 UAPI
├── app/                    # 用户态测试程序和采集服务
├── tests/                  # 测试说明与故障注入
├── patches/                # 厂商内核兼容补丁
└── docs/
    ├── architecture.md     # 架构与接口设计
    ├── prerequisite_guide.md
    ├── day55_start.md      # 阶段 0 原始实施记录
    ├── stage0_validation.md # 阶段 0 实机验收结果
    └── progress.md         # 已验证进度和当前断点
```

编译产物、编辑器配置、日志和本地环境文件不会提交到仓库。

## 文档入口

- [实施进度](./docs/progress.md)
- [架构设计](./docs/architecture.md)
- [前置知识](./docs/prerequisite_guide.md)
- [阶段 0 实施记录](./docs/day55_start.md)
- [阶段 0 验收结果](./docs/stage0_validation.md)

## 后续衔接

该项目完成后，驱动数据可以作为后续车载系统的统一输入源：

```text
Vehicle Sensor Lab
        ├── Qt 车载状态界面
        ├── SmartEdgeGateway
        └── 倒车影像与事件记录项目
```

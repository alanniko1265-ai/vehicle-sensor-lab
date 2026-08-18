# VehicleSensorLab

基于 i.MX6ULL 的 Linux 车载数据采集驱动与交互实验台。

## 项目定位

这个项目用于完成从 Linux 应用开发到驱动/BSP 开发的第一次完整跨越：

```text
Qt/CLI 控制台
      ↕ TCP（后期）
用户态传感器服务
      ↕ read / poll / ioctl
/dev/vehicle_sensor
      ↕
Linux 字符设备驱动
      ↕
软件事件 → 设备树/I2C → 后期真实 IMU
```

你负责亲自实现核心代码。仓库先提供架构边界、分阶段任务和验收标准，不提供完整答案。

## 最终互动效果

- 调整速度、温度和采样周期；
- 真实 IMU 采集振动、姿态和冲击事件；
- LED 显示正常、告警和故障状态；
- CLI/Qt 实时查看传感器事件；
- 同时启动多个读取者，观察事件分发行为；
- 主动制造缓冲区溢出、慢消费者和设备断开；
- 查看丢失事件、队列深度和驱动统计；
- 后期作为智能倒车影像项目的车辆事件输入源。
- 最后作为 `SmartEdgeGateway` 的真实 Linux 驱动数据源，由 Qt 控制中心配置和展示。

## 技术目标

- 内核模块的构建、加载和卸载；
- 字符设备与 `file_operations`；
- 用户态/内核态数据边界；
- 阻塞 `read`、非阻塞访问和 `poll`；
- `ioctl` 控制接口；
- wait queue、环形缓冲区和并发保护；
- platform driver、设备树与 `probe/remove`；
- 用户态 C++ 服务和 Qt/CLI 客户端；
- 故障注入、统计、文档和可复现测试。

## 当前硬件与环境

- 开发板：野火 EBF6ULL S1 Pro；
- SoC：NXP i.MX6ULL / ARMv7 Cortex-A7；
- 板端内核：`4.19.35-imx6`；
- 系统：Debian 10；
- 已验证：SSH、交叉编译、板载 LED、板载按键、RS485；
- 当前阶段不需要购买新硬件。

## 分阶段路线

| 阶段 | 结果 | 新知识 | 风险 |
|---:|---|---|---|
| 0 | 内核模块构建环境验证 | Kbuild、内核版本、交叉编译 | 低 |
| 1 | `/dev/vehicle_sensor` 可读取固定事件 | module、miscdevice、read | 低 |
| 2 | 没有数据时阻塞，有事件时被唤醒 | wait queue、poll、O_NONBLOCK | 中 |
| 3 | 可调整采样周期和注入事件 | ioctl、UAPI、输入校验 | 中 |
| 4 | 连续事件不丢失，能报告溢出 | ring buffer、锁、并发 | 中高 |
| 5 | 使用 platform driver 和设备树匹配 | device tree、probe/remove | 中高 |
| 6 | 接入 I2C IMU 并采集真实运动数据 | I2C、设备树、IIO/中断 | 中高 |
| 7 | C++服务和Qt/CLI交互控制台 | 设备抽象、线程边界、UI | 中 |
| 8 | 故障注入、长稳测试和简历整理 | 测试、诊断、工程文档 | 中 |

每个阶段完成验收后再进入下一阶段。不要在第 1 阶段同时加入设备树、真实硬件和 Qt。

## 与后续项目的关系

```text
当前：VehicleSensorLab（驱动与用户态设备服务）
          ↓
后续：智能倒车影像与事件记录（音视频）
          ↓
最后：SmartEdgeGateway（Linux服务 + Qt综合整合）
```

智能网关最后实施，当前项目不提前承担网关界面、远程管理和摄像头功能。

## 目录约定

```text
vehicle_sensor_lab/
├── driver/                 # 内核模块，由你逐阶段实现
├── include/                # 驱动与用户态共享的 UAPI
├── app/                    # 用户态测试程序和后续 C++ 服务
├── patches/                # 构建旧版厂商内核所需的可追踪兼容补丁
├── tests/                  # 可复现测试与故障注入脚本说明
└── docs/
    ├── architecture.md     # 架构与关键设计决定
    ├── progress.md         # 已验证进度、当前断点和下一步
    └── day55_start.md      # 当前第一天任务
```

## 工程习惯：第一阶段只要求这些

- 每个阶段单独提交一次 Git；
- 编译无新增警告；
- 所有错误路径都释放已经取得的资源；
- `insmod`/`rmmod` 可以重复执行；
- README 中记录实际命令和结果；
- 不复制整份教程代码，先解释每个回调为什么存在。

暂时不要求复杂 CI、100% 覆盖率、完整内核编码规范检查或大型框架。

## 当前入口

先查看 [当前实施进度](./docs/progress.md)，再阅读 [前置知识](./docs/prerequisite_guide.md) 和 [阶段 0 记录](./docs/day55_start.md)。阶段 0 只完成环境核验和第一个模块闭环，不修改设备树，也不接入真实传感器。

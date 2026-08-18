# 实施进度

本文件只记录已经验证的结果、当前断点和紧接着要做的事情。计划中的功能不会提前标记为完成。

## 2026-08-18：阶段 0——驱动构建环境（已完成）

### 已完成

- 确认开发板运行 Debian 10，架构为 ARMv7；
- 确认开发板运行内核为 `4.19.35-imx6`；
- 从开发板导出并验证 `/proc/config.gz`；
- 下载野火 `ebf_4.19.35_imx6ul` 内核分支；
- 建立 `build_image/build` 独立输出目录；
- 使用板端配置完成 `olddefconfig`；
- 验证 `ARCH=arm LOCALVERSION=-imx6` 时 `kernelrelease` 为 `4.19.35-imx6`；
- 定位旧版 DTC 与新版主机 GCC 的 `yylloc` 重复定义问题；
- 应用 `patches/0001-dtc-remove-redundant-yylloc.patch` 并通过 `git diff --check`；
- 完成补丁后的内核增量构建，生成 641276 字节的 `Module.symvers`；
- 编译 `vehicle_sensor_module.ko`，确认其为 ARM 32 位 EABI5 模块；
- 确认模块 `vermagic` 为 `4.19.35-imx6`；
- 通过 SCP 将模块上传至 i.MX6ULL；
- 使用 `insmod` 加载模块并在 `lsmod` 中确认驻留；
- 在 `dmesg` 中确认 `vehicle_sensor_init()` 输出 loaded 日志；
- 使用 `rmmod` 卸载模块并确认 `vehicle_sensor_exit()` 输出 unloaded 日志。

详细证据见 [阶段 0 验收记录](./stage0_validation.md)。

## 当前断点：阶段 1——第一个字符设备

- 阶段 0 已经闭环，不再重复准备内核环境；
- 下一目标是创建 `/dev/vehicle_sensor`；
- 第一版设备只返回一条固定事件，暂不加入定时器、`poll`、`ioctl` 或真实传感器。

### 阶段 1 下一步验收

1. 理解设备号、设备节点和 `file_operations` 的关系；
2. 注册最小字符设备；
3. 加载后生成 `/dev/vehicle_sensor`；
4. 编写用户态 C 程序打开设备；
5. 从设备读取一条固定事件并正确处理返回值。

## 已确认的项目边界

- 第一阶段不要求 STM32；
- 前期使用软件事件理解驱动接口，后期再接入真实 I2C IMU；
- 到真实传感器阶段前再确认模块型号、电压和接线，不提前购买；
- 倒车影像、超声波和蜂鸣器属于后续独立项目，不塞进当前驱动入门阶段。

## 学习方式

每次只引入一个主要概念：先说明目标和运行位置，再执行一个小操作，根据真实输出决定下一步。

# 实施进度

本文件只记录已经验证的结果、当前断点和紧接着要做的事情。计划中的功能不会提前标记为完成。

## 2026-08-18：阶段 0——驱动构建环境

### 已完成

- 确认开发板运行 Debian 10，架构为 ARMv7；
- 确认开发板运行内核为 `4.19.35-imx6`；
- 从开发板导出并验证 `/proc/config.gz`；
- 下载野火 `ebf_4.19.35_imx6ul` 内核分支；
- 建立 `build_image/build` 独立输出目录；
- 使用板端配置完成 `olddefconfig`；
- 验证 `ARCH=arm LOCALVERSION=-imx6` 时 `kernelrelease` 为 `4.19.35-imx6`；
- 定位旧版 DTC 与新版主机 GCC 的 `yylloc` 重复定义问题；
- 应用 `patches/0001-dtc-remove-redundant-yylloc.patch` 并通过 `git diff --check`。

### 当前断点

- 需要确认补丁后的完整内核增量构建成功；
- 需要确认内核构建目录中的 `Module.symvers` 已生成；
- 最小模块源码已经具备加载和卸载日志，但尚未完成板端闭环验证。

### 下一步验收

1. 编译 `driver/vehicle_sensor_module.c`，生成 ARM 32 位 `.ko`；
2. 用 `modinfo` 检查 `vermagic=4.19.35-imx6`；
3. 上传至 i.MX6ULL；
4. 使用 `insmod` 和 `rmmod` 完成一次加载、卸载；
5. 在 `dmesg` 中看到对应的 loaded/unloaded 日志。

## 已确认的项目边界

- 第一阶段不要求 STM32；
- 前期使用软件事件理解驱动接口，后期再接入真实 I2C IMU；
- 到真实传感器阶段前再确认模块型号、电压和接线，不提前购买；
- 倒车影像、超声波和蜂鸣器属于后续独立项目，不塞进当前驱动入门阶段。

## 学习方式

每次只引入一个主要概念：先说明目标和运行位置，再执行一个小操作，根据真实输出决定下一步。

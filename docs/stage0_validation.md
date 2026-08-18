# 阶段 0 验收记录

日期：2026-08-18

## 验收目标

验证从主机源码到 i.MX6ULL 运行内核的最小外部模块开发链路：

```text
C 源码
  → 匹配内核的 Kbuild
  → ARM 交叉编译
  → SCP 上传
  → insmod 加载
  → init 执行
  → rmmod 卸载
  → exit 执行
```

## 内核构建环境

匹配内核构建生成的符号文件：

```text
/home/anllenge/work/imx6ull-kernel/ebf_linux_kernel/build_image/build/Module.symvers
641276 bytes
```

模块成功完成 `CC [M]`、`MODPOST` 和 `LD [M]`，生成：

```text
driver/vehicle_sensor_module.ko
```

## 模块元数据

主机和开发板上的 `modinfo` 验证结果：

```text
version:        0.1.0
description:    Vehicle sensor lab minimal kernel module
author:         alanniko1265-ai
license:        GPL
name:           vehicle_sensor_module
vermagic:       4.19.35-imx6 SMP preempt mod_unload modversions ARMv7 p2v8
```

主机 `file` 验证模块为：

```text
ELF 32-bit LSB relocatable, ARM, EABI5
```

## 板端加载

模块通过 SCP 上传至：

```text
/home/debian/vehicle_sensor_module.ko
```

加载后 `lsmod`：

```text
vehicle_sensor_module    16384  0
```

内核日志：

```text
[  875.402152] vehicle_sensor_module: loading out-of-tree module taints kernel.
[  875.412628] vehicle_sensor v0.1.0 loaded
```

`out-of-tree module taints kernel` 是内核对外部模块的状态标记，不是加载错误。

## 板端卸载

执行 `rmmod vehicle_sensor_module` 后，`lsmod` 不再返回该模块。

退出日志：

```text
[ 1487.989097] vehicle_sensor unloaded
```

## 验收结论

阶段 0 通过：

- 模块架构与 i.MX6ULL 匹配；
- `vermagic` 与运行内核匹配；
- 初始化函数成功执行；
- 模块成功驻留内核；
- 退出函数成功执行；
- 模块能够安全卸载。

下一阶段开始实现 `/dev/vehicle_sensor`，使用户态程序能够读取第一条固定事件。

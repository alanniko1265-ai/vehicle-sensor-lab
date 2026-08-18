# Day55：建立内核模块最小闭环

如果下面的术语还不熟悉，先完整阅读 [prerequisite_guide.md](./prerequisite_guide.md)。`driver/` 中已经提供代码和 Makefile 骨架，你只需完成两个日志 TODO，并填入自己的作者信息。

## 今日成果

今天不写字符设备。只完成以下闭环：

```text
确认板端内核版本
  → 准备匹配的内核构建目录
  → 自己编写最小模块
  → 交叉编译为 .ko
  → insmod/rmmod
  → 从 dmesg 验证生命周期
```

这不是纯环境配置。`.ko` 能在真实 i.MX6ULL 上重复加载和卸载，就是今天的可运行结果。

## 先理解三个概念

### 1. 应用交叉编译器不等于驱动构建环境

普通应用只依赖编译器和用户态库。内核模块还依赖正在运行内核对应的：

- 配置；
- 生成头文件；
- 符号版本；
- Kbuild 输出。

所以仅有 `arm-linux-gnueabihf-gcc` 还不够。

### 2. 模块是运行中内核的一部分

模块被加载后拥有内核权限。空指针、越界和错误资源释放可能导致整个系统异常，因此先从只有 init/exit 的模块开始。

### 3. Kbuild 负责模块的真实构建过程

外部模块的 Makefile 通常只是告诉内核构建系统：

- 目标架构是 ARM；
- 交叉编译器前缀是什么；
- 匹配的内核构建目录在哪里；
- 哪个对象需要构建成模块。

## 任务 A：在开发板记录事实

在板端执行并保存输出：

```bash
uname -a
uname -r
cat /etc/os-release
cat /proc/version
ls -ld /lib/modules/$(uname -r)
ls -ld /lib/modules/$(uname -r)/build
zcat /proc/config.gz | head
```

如果 `/proc/config.gz` 不存在，记录错误即可，不要为了完成命令随意修改系统。

验收：能明确写出正在运行的内核 release；知道板上是否自带可用的 `build` 链接。

## 任务 B：检查主机端环境

在 WSL 执行：

```bash
arm-linux-gnueabihf-gcc --version
find ~/ -maxdepth 4 -type d -name 'ebf_linux_kernel' 2>/dev/null
```

若尚无匹配内核源码，按野火官方驱动环境文档准备 `ebf_4.19.35_imx6ul` 分支，并先完成一次官方要求的内核构建。源码较大，本步骤不要盲目重复下载。

参考：<https://doc.embedfire.com/linux/imx6/driver/zh/latest/linux_driver/exper_env.html>

验收：得到实际的 `KERNEL_DIR`，其中是已经准备好的内核构建输出，而不只是一个刚解压的源码目录。

## 任务 C：你来写第一个模块

在 `driver/` 中已经提供：

```text
vehicle_sensor_module.c
Makefile
```

源文件只包含：

- 一个模块加载函数；
- 一个模块卸载函数；
- 加载时打印模块名和版本；
- 卸载时打印退出信息；
- GPL license、author、description、version 元数据。

骨架已经标出这些入口，请结合前置知识说明它们的作用：

```text
module_init
module_exit
pr_info
MODULE_LICENSE
```

本阶段禁止加入字符设备、定时器、线程和设备树代码。

Makefile需要表达：

```text
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
obj-m := vehicle_sensor_module.o
make -C <KERNEL_DIR> M=<当前目录> modules
```

Makefile 已允许从命令行传入 `KERNEL_DIR`，并会在目录缺失时给出说明。先运行 `make help` 阅读用法，不要直接把别人的固定路径写进仓库。

## 任务 D：部署和验证

将 `.ko` 上传到开发板后执行：

```bash
sudo insmod vehicle_sensor_module.ko
lsmod | grep vehicle_sensor
modinfo vehicle_sensor_module.ko
dmesg | tail -n 20
sudo rmmod vehicle_sensor_module
dmesg | tail -n 20
```

再连续执行 10 次加载/卸载测试。每次都应成功，且 `dmesg` 中不能出现 warning、oops 或资源泄漏相关信息。

## 今日提交内容

- `driver/vehicle_sensor_module.c`；
- `driver/Makefile`；
- 在本文件末尾追加“实际环境记录”；
- 一次 Git 提交，建议信息：`feat: add minimal vehicle sensor kernel module`。

## 请不要做

- 不替换内核镜像；
- 不修改设备树；
- 不从网上复制完整字符驱动；
- 不用 `insmod -f` 强制加载版本不匹配的模块；
- 不在确认绝对路径前清理内核构建目录。

## 完成后带回来的信息

1. 任务 A 的输出；
2. 实际 `KERNEL_DIR`；
3. `make` 的完整输出；
4. `insmod`、`rmmod` 和 `dmesg` 的结果；
5. 你对 init/exit 以及 Kbuild 流程的解释。

拿到这些信息后，Day56 再进入 `miscdevice + read`，创建真正的 `/dev/vehicle_sensor`。

## 实际环境记录

### 已由环境预检查确认

```text
WSL 用户：anllenge
系统工具链：/usr/bin/arm-linux-gnueabihf-gcc，GCC 11.4.0
已有旧项目工具链：/home/anllenge/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc，GCC 8.3.0
匹配内核目录：在 /home/anllenge 下暂未找到 ebf_linux_kernel/build_image
```

当前真正缺少的是**已完成准备的匹配内核构建目录**，不是交叉编译器。下一步先完成任务 A 的板端信息采集，再决定使用哪个编译器准备官方内核树。

### 待你填写

- 板端 `uname -a`：`Linux npi 4.19.35-imx6 #1.2508stable SMP PREEMPT Sat Aug 23 03:32:38 UTC 2025 armv7l GNU/Linux`
- 板端 `uname -r`：`4.19.35-imx6`
- 板端 `/proc/version`：内核由 `Ubuntu/Linaro GCC 7.5.0` 构建
- `/proc/config.gz`：存在，可用于取得运行内核配置；显示 `CONFIG_GCC_VERSION=70500`
- 模块配置：`CONFIG_MODULES=y`、`CONFIG_MODULE_UNLOAD=y`、`CONFIG_MODVERSIONS=y`
- `/lib/modules/4.19.35-imx6`：存在，包含当前内核模块
- `/lib/modules/$(uname -r)/build`：不存在，因此不在板端原地构建模块
- 配置副本：已复制到 WSL `/home/anllenge/work/imx6ull-kernel/npi-config.gz`，大小约 32 KiB，`gzip -t` 返回 0
- WSL 空间：约 950 GiB 可用，足够保存并完整构建内核
- 内核源码：`ebf_4.19.35_imx6ul` 分支，浅克隆提交 `927f64b95`
- 野火构建脚本：通过 `LOCALVERSION=-imx6` 添加运行内核所需后缀
- 已验证：使用 `ARCH=arm LOCALVERSION=-imx6` 时，`kernelrelease` 精确输出 `4.19.35-imx6`
- 首次完整构建：主机 GCC 11 默认 `-fno-common`，触发 DTC 的 `yylloc` 重复定义；采用上游提交 `e33a814e...` 的一行兼容补丁后继续增量构建
- 最终 `KERNEL_DIR`：
- 模块 `vermagic`：
- 10 次加载/卸载结果：

### 本次命令纠错

- `whoiam` 是拼写错误，正确命令是 `whoami`。
- `ls *ld ...` 中的 `*` 是通配符，不是选项前缀；正确命令是 `ls -ld ...`。这次命令把 `*ld` 当成了一个待查看的文件名，因此先报找不到 `*ld`，随后仍列出了第二个参数指向的模块目录。
- `~work/...` 不等于 `~/work/...`：`~` 只有单独出现或紧跟 `/` 时才展开为当前用户主目录。
- `imx6ull` 末尾是两个小写字母 `l`，不是两个数字 `1`；Linux 路径必须逐字符匹配。
- `ARCH=aem` 是拼写错误，Kbuild 因而寻找不存在的 `arch/aem/Makefile`；正确值是 `ARCH=arm`。
- `ARCG=arm` 只是设置了一个 Kbuild 不认识的变量，不能替代 `ARCH=arm`。
- 只执行 `make O=...` 会选择默认的完整构建目标；配置尚未同步时会进入交互提问。Day55 应先执行带完整参数的 `olddefconfig`。

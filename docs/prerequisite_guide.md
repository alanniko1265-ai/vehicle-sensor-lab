# 前置知识：第一次写 Linux 内核模块

这份说明只覆盖 Day55 所需知识，不要求先学完整的 Linux 驱动课程。

## 1. 你正在写的东西是什么

过去的程序运行在用户态：

```text
你的应用程序
    ↓ 系统调用
Linux 内核
    ↓
硬件
```

现在要写的 `.ko` 是可动态装入内核的代码：

```text
vehicle_sensor_module.ko
        ↓ insmod
正在运行的 Linux 内核
        ↓ rmmod
卸载并释放资源
```

因此它与普通应用有三个关键区别：

1. 没有普通应用的 `main()`；
2. 不能直接使用 `printf`、glibc 和普通用户态头文件；
3. 错误可能影响整个系统，所以资源取得和释放必须成对。

## 2. 最小模块的生命周期

```text
sudo insmod vehicle_sensor_module.ko
                  ↓
         vehicle_sensor_init()
                  ↓
             模块已加载
                  ↓
sudo rmmod vehicle_sensor_module
                  ↓
         vehicle_sensor_exit()
```

- `module_init()`：告诉内核哪个函数是加载入口；
- `module_exit()`：告诉内核哪个函数是卸载入口；
- `__init`：加载完成后，入口函数占用的初始化内存可以被回收；
- `__exit`：标记只在卸载阶段使用的代码；
- `pr_info()`：向内核日志写一条 info 级别消息；
- `MODULE_LICENSE("GPL")`：声明模块许可证，避免内核被标记为专有模块污染。

第一版模块不创建设备节点。它只证明“主机能够构建，开发板能够加载，生命周期能够正确执行”。

## 3. 为什么普通 gcc 命令不能直接编译 `.ko`

内核模块需要知道当前内核的：

- 配置选项；
- 生成头文件；
- 架构；
- 导出符号；
- 模块版本信息。

因此构建入口不是：

```bash
arm-linux-gnueabihf-gcc vehicle_sensor_module.c
```

而是让目标内核的 Kbuild 系统调用交叉编译器：

```bash
make -C <KERNEL_DIR> M=<模块源码目录> modules
```

其中：

- `-C <KERNEL_DIR>`：先进入匹配的内核构建目录；
- `M=<模块源码目录>`：再回来构建这个外部模块；
- `obj-m`：告诉 Kbuild 哪个对象要生成 `.ko`；
- `ARCH=arm`：目标是 ARM；
- `CROSS_COMPILE=arm-linux-gnueabihf-`：工具链命令前缀。
- `LOCALVERSION=-imx6`：加入野火运行内核使用的 release 后缀。

## 4. 三个版本信息必须能对应

板端：

```bash
uname -r
```

模块构建完成后，在主机或板端：

```bash
modinfo vehicle_sensor_module.ko | grep vermagic
```

加载后：

```bash
dmesg | tail -n 20
```

你需要建立的判断是：

```text
运行内核 release
≈ 模块 vermagic
← 来自匹配内核源码和构建配置
```

版本不匹配时不要使用 `insmod -f` 强行加载。

## 5. 代码骨架中每一块的职责

代码位于 `../driver/vehicle_sensor_module.c`。

```c
#include <linux/init.h>      // __init、__exit
#include <linux/module.h>    // module_init、MODULE_LICENSE 等
#include <linux/printk.h>    // pr_info
```

加载函数返回 `int`：

- `0` 表示初始化成功；
- 负的 Linux errno 表示失败；
- 如果加载函数失败，内核不会把模块保留为已加载状态。

卸载函数返回 `void`：所有初始化阶段取得的资源，都应该在这里按相反顺序释放。

Day55还没有资源，因此exit函数只打印日志。

## 6. 两个终端分别做什么

### WSL终端：编写和交叉编译

```bash
cd /mnt/c/Users/20858/Documents/实习/linux_projects/vehicle_sensor_lab/driver
make KERNEL_DIR=/你的实际内核构建目录
```

### 开发板终端：加载和验证

可以从WSL连接：

```bash
ssh debian@192.168.7.2
```

上传模块：

```bash
scp vehicle_sensor_module.ko debian@192.168.7.2:/home/debian/
```

然后在开发板上执行 `insmod`、`rmmod`、`lsmod` 和 `dmesg`。

## 7. Day55只需要会回答的问题

完成后你应该能用自己的话回答：

1. 内核模块为什么没有 `main()`？
2. `insmod` 和 `rmmod` 分别触发哪个函数？
3. `pr_info` 的内容为什么不一定直接显示在当前终端？
4. 为什么应用交叉编译器不能单独完成内核模块构建？
5. `KERNEL_DIR` 应指向什么，而不应该随便指向什么？

暂时不需要理解字符设备号、VFS、`file_operations`、设备树和并发。它们从 Day56 开始逐项加入。

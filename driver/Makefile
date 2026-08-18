ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-
LOCALVERSION ?= -imx6
KERNEL_DIR ?=

export ARCH CROSS_COMPILE LOCALVERSION

obj-m := vehicle_sensor_module.o

.PHONY: all clean help check-kernel-dir

all: check-kernel-dir
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules

clean: check-kernel-dir
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean

check-kernel-dir:
	@test -n "$(KERNEL_DIR)" || (echo "usage: make KERNEL_DIR=/absolute/path/to/kernel/build"; exit 1)
	@test -f "$(KERNEL_DIR)/Makefile" || (echo "KERNEL_DIR does not contain a kernel Makefile: $(KERNEL_DIR)"; exit 1)

help:
	@echo "build: make KERNEL_DIR=/absolute/path/to/kernel/build"
	@echo "clean: make KERNEL_DIR=/absolute/path/to/kernel/build clean"
	@echo "override toolchain if needed: CROSS_COMPILE=/path/to/arm-linux-gnueabihf-"
	@echo "board kernel suffix: LOCALVERSION=$(LOCALVERSION)"

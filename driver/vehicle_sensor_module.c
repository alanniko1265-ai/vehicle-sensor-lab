// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>

#define VEHICLE_SENSOR_NAME "vehicle_sensor"
#define VEHICLE_SENSOR_VERSION "0.1.0"

static int __init vehicle_sensor_init(void)
{
    pr_info("%s v%s loaded\n",
            VEHICLE_SENSOR_NAME,
            VEHICLE_SENSOR_VERSION);

    return 0;
}

static void __exit vehicle_sensor_exit(void)
{
    pr_info("%s unloaded\n", VEHICLE_SENSOR_NAME);
}

module_init(vehicle_sensor_init);
module_exit(vehicle_sensor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("alanniko1265-ai");
MODULE_DESCRIPTION("Vehicle sensor lab minimal kernel module");
MODULE_VERSION(VEHICLE_SENSOR_VERSION);

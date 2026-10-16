#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include "our_driver.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#define SENSOR_NODE DT_NODELABEL(our_driver0)
#define MIN_DELAY_MS 100
#define MAX_DELAY_MS 5000

static const struct device *const sensor_dev = DEVICE_DT_GET(SENSOR_NODE);

static int cmd_sensor_fetch (const struct shell *sh, size_t argc, char **argv){
    if (!device_is_ready(sensor_dev)) {
        shell_error(sh, "Sensor is not ready");
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(sensor_dev);

    if (ret < 0) {
        shell_error(sh, "Sample fetch failed: %d", ret);
        return ret;
    }

    shell_print(sh, "Sample fetched successfully (LED ON)");
    return 0;

};

static int cmd_sensor_read (const struct shell *sh, size_t argc, char **argv){
     if (!device_is_ready(sensor_dev)) {
        shell_error(sh, "Sensor is not ready");
        return -ENODEV;
    }

    struct sensor_value val = {0};

    int ret = sensor_channel_get(
        sensor_dev,
        SENSOR_CHAN_AMBIENT_TEMP,
        &val
    );

    if (ret < 0) {
        shell_error(sh, "Channel get failed: %d", ret);
        return ret;
    }

    shell_print(
        sh,
        "Sensor value: %d.%06d",
        val.val1,
        val.val2
    );

    shell_print(sh, "LED OFF");

    return 0; 
};

static int cmd_sensor_info (const struct shell *sh, size_t argc, char **argv){
    shell_print(sh, "Device name: %s", sensor_dev->name);

    shell_print(
        sh,
        "Ready state: %s",
        device_is_ready(sensor_dev) ? "YES" : "NO"
    );
    return 0;
};

static int cmd_sensor_set (const struct shell *sh, size_t argc, char **argv){
    if (argc != 2) {
        shell_error(sh, "Missing value. Usage: sensor set <delay_ms>");
        return -EINVAL;
    }

    char *endptr;
    errno = 0;

    if (argv[1][0] < '0' || argv[1][0] > '9') {
        shell_error(sh, "Invalid value: %s", argv[1]);
        return -EINVAL;
    }

    unsigned long value = strtoul(argv[1], &endptr, 10);

    if (errno == ERANGE || endptr == argv[1] || *endptr != '\0') {
        shell_error(sh, "Invalid numeric value: %s", argv[1]);
        return -EINVAL;
    }

    if (value < MIN_DELAY_MS || value > MAX_DELAY_MS) {
        shell_error(
            sh,
            "Delay must be between %d and %d ms",
            MIN_DELAY_MS,
            MAX_DELAY_MS
        );
        return -ERANGE;
    }

    if (!device_is_ready(sensor_dev)) {
        shell_error(sh, "Sensor device is not ready");
        return -ENODEV;
    }

    int ret = our_driver_set_delay(sensor_dev, (uint32_t)value);

    if (ret < 0) {
        shell_error(sh, "Failed to set delay: %d", ret);
        return ret;
    }

    shell_print(sh, "Blink delay set to %lu ms", value);
    return 0;
};

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor, 
SHELL_CMD(fetch, NULL, "Calls sensor_sample_fetch", cmd_sensor_fetch),
SHELL_CMD(read, NULL, "Calls sensor_channel_get", cmd_sensor_read),
SHELL_CMD(info, NULL, "Print the device name and ready state", cmd_sensor_info),
SHELL_CMD_ARG(set, NULL, "Set the blink delay in milliseconds", cmd_sensor_set, 2, 0),
SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(sensor, &sub_sensor, "LED Controller", NULL);    
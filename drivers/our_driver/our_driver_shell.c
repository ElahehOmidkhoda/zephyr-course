#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>

#define SENSOR_NODE DT_NODELABEL(our_driver0)

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

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor, 
SHELL_CMD(fetch, NULL, "Calls sensor_sample_fetch", cmd_sensor_fetch),
SHELL_CMD(read, NULL, "Calls sensor_channel_get", cmd_sensor_read),
SHELL_CMD(info, NULL, "Print the device name and ready state", cmd_sensor_info),
SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(sensor, &sub_sensor, "LED Controller", NULL);    
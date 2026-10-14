#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_INF);

#define MY_SENSOR_NODE DT_NODELABEL(our_driver0)

int main(void)
{
    const struct device *driver =
        DEVICE_DT_GET(MY_SENSOR_NODE);

    if (!device_is_ready(driver)) {
        LOG_ERR("Driver not ready");
        return 0;
    }

    struct sensor_value val = {};

    while (true) {

        // Turn LED ON
        int ret = sensor_sample_fetch(driver);

        if (ret < 0) {
            LOG_ERR("Sample fetch failed: %d", ret);
        }

        k_msleep(1000);

        // Turn LED OFF
        ret = sensor_channel_get(
            driver,
            SENSOR_CHAN_AMBIENT_TEMP,
            &val
        );

        if (ret < 0) {
            LOG_ERR("Channel get failed: %d", ret);
        }

        k_msleep(1000);
    }

    return 0;
}
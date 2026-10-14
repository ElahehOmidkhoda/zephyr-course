#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define DT_DRV_COMPAT our_driver

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

struct our_driver_data {
    uint32_t blink_delay_ms;
};

static struct our_driver_data driver_data = {
    .blink_delay_ms = 1000,
};

static const struct gpio_dt_spec led = GPIO_DT_SPEC_INST_GET(0, led_gpios);

static int channel_get_my_imp(const struct device *dev,
				    enum sensor_channel chan,
				    struct sensor_value *val) {
                        int ret = gpio_pin_set_dt(&led, 0);
                        if (ret < 0) {
                            LOG_ERR("Failed to set LED pin: %d", ret);
                            return ret;
                        }
                        LOG_INF("LED OFF, Channel Get");
                        return 0;
                    }

static int sample_fetch_my_imp(const struct device *dev,
				    enum sensor_channel chan){
                        int ret = gpio_pin_set_dt(&led, 1);
                        if (ret < 0) {
                            LOG_ERR("Failed to set LED pin: %d", ret);
                            return ret;
                        }
                        LOG_INF("LED ON, Sample Fetch");
                        return 0;
                    }


int our_driver_set_delay(const struct device *dev, uint32_t delay_ms)
{
    struct our_driver_data *data = dev->data;

    data->blink_delay_ms = delay_ms;

    LOG_INF("Blink delay changed to %u ms",
            (unsigned int)data->blink_delay_ms);

    return 0;
}

static DEVICE_API(sensor, api_iomico_lecture) = {
    .channel_get = channel_get_my_imp,
    .sample_fetch = sample_fetch_my_imp,

};

static int init(const struct device *dev)
{
    struct our_driver_data *data = dev->data;

     int ret = gpio_pin_configure_dt(
        &led,
        GPIO_OUTPUT_INACTIVE
    );
    if (ret < 0) {
        LOG_ERR("GPIO configuration failed: %d", ret);
        return ret;
    }
    LOG_INF("Device Initialized");
    LOG_INF("Initial blink delay: %u ms",
            (unsigned int)data->blink_delay_ms);
    return 0;
};

DEVICE_DT_INST_DEFINE(0, init, NULL, &driver_data, NULL, POST_KERNEL, 80, &api_iomico_lecture)
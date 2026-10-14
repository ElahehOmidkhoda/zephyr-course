#ifndef OUR_DRIVER_H
#define OUR_DRIVER_H

#include <stdint.h>
#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

int our_driver_set_delay(const struct device *dev, uint32_t delay_ms);

#ifdef __cplusplus
}
#endif

#endif
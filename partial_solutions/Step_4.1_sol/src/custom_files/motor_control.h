#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
/* Step 3.9 - Include pwm drivers */
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>


int motor_init(void);
int set_motor_angle(int32_t pwm_duty_cycle_ns);
#include "motor_control.h"

#define LOG_MODULE_NAME motor_control
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

// #define PWM_LED0	DT_ALIAS(pwm_led0)
#define PWM_LED0	DT_NODELABEL(pwm_led1) 
static const struct pwm_dt_spec pwm_led1 = PWM_DT_SPEC_GET(PWM_LED0);

#define PWM_PERIOD_NS 20000000
#define PWM_PULSE_NS 1500000
int motor_init(void)
{
    int err = 0;
    LOG_INF("Initializing Motor Control");

    if (!pwm_is_ready_dt(&pwm_led1)) {
        LOG_ERR("Error: PWM device %s is not ready", pwm_led1.dev->name);
        return 0;
    }

    err = pwm_set_dt(&pwm_led1, PWM_PERIOD_NS, PWM_PULSE_NS);
    if (err) {
        LOG_ERR("pwm_set_dt returned %d", err);
    }

    return err;
}
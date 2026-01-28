#include "motor_control.h"

#define LOG_MODULE_NAME motor_control
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

// #define PWM_LED0	DT_ALIAS(pwm_led0)
// #define PWM_LED0	DT_NODELABEL(pwm_led1) 
// static const struct pwm_dt_spec pwm_led1 = PWM_DT_SPEC_GET(PWM_LED0);

#define SERVO_MOTOR    DT_NODELABEL(servo)
#define PWM_SERVO_MIN_DUTY_CYCLE  DT_PROP(SERVO_MOTOR, min_pulse)
#define PWM_SERVO_MAX_DUTY_CYCLE  DT_PROP(SERVO_MOTOR, max_pulse)

static const struct pwm_dt_spec pwm_servo = PWM_DT_SPEC_GET(SERVO_MOTOR);


#define PWM_PERIOD_NS 20000000
#define PWM_PULSE_WIDTH_NS 1500000
int motor_init(void)
{
    int err = 0;
    LOG_INF("Initializing Motor Control");

    if (!pwm_is_ready_dt(&pwm_servo)) {
        LOG_ERR("Error: PWM device %s is not ready", pwm_servo.dev->name);
        return 0;
    }

    err = pwm_set_dt(&pwm_servo, PWM_PERIOD_NS, PWM_PULSE_WIDTH_NS);
    if (err) {
        LOG_ERR("pwm_set_dt returned %d", err);
    }

    return err;
}

int set_motor_angle(int32_t pwm_duty_cycle_ns){
    int err;
    err = pwm_set_dt(&pwm_servo, PWM_PERIOD_NS, pwm_duty_cycle_ns);
    if (err) {
        LOG_ERR("pwm_set_dt_returned %d", err);
    }
 
    return err;
}
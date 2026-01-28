/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <zephyr/logging/log.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/kernel.h>
#include "motor_control.h"

#define LOG_MODULE_NAME app
LOG_MODULE_REGISTER(LOG_MODULE_NAME);


#define RUN_STATUS_LED DK_LED1
#define RUN_LED_BLINK_INTERVAL 1000

#define SERVO_MOTOR    DT_NODELABEL(servo)
#define PWM_SERVO_MIN_DUTY_CYCLE  DT_PROP(SERVO_MOTOR, min_pulse)
#define PWM_SERVO_MAX_DUTY_CYCLE  DT_PROP(SERVO_MOTOR, max_pulse)


void button_handler(uint32_t button_state, uint32_t has_changed)
{
	int button_pressed = 0;
    int err = 0;
	if (has_changed & button_state)
	{
		switch (has_changed)
		{
			case DK_BTN1_MSK:
				button_pressed = 1;
				err = set_motor_angle(PWM_SERVO_MIN_DUTY_CYCLE);
				break;
			case DK_BTN2_MSK:
				button_pressed = 2;
				err = set_motor_angle(PWM_SERVO_MAX_DUTY_CYCLE);
				break;
			case DK_BTN3_MSK:
				button_pressed = 3;
				break;
			case DK_BTN4_MSK:
				button_pressed = 4;
				break;
			default:
				break;
		}
        if (err) {
            LOG_ERR("Error: couldn't set duty cycle, err %d", err);
        }
	}
}
static void configure_dk_buttons_and_leds(void){
	int err;

	err = dk_leds_init();
	if(err){
		LOG_ERR("Couldn't init LEDS (err %d)", err);
	}
	
    err = dk_buttons_init(button_handler);
    if (err) {
        LOG_ERR("Couldn't init buttons (err %d)", err);
    }
}

int main(void)
{

	int err = 0;
	bool blink_status = false;

	printf("Hello World! %s\n", CONFIG_BOARD);

	configure_dk_buttons_and_leds();

	err = motor_init();
    if (err) {
        LOG_ERR("motor_init() failed. (err %d)", err);
    }
	
	for (;;){
		dk_set_led(RUN_STATUS_LED, (blink_status));
		blink_status = !blink_status;
		k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));


	}

	return 0;
}
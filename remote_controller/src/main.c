#include <stdio.h>
#include <zephyr/logging/log.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/kernel.h>

#include "motor_control.h"

#include "remote.h"

#define LOG_MODULE_NAME app
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#define RUN_STATUS_LED DK_LED1

#define RUN_LED_BLINK_INTERVAL 1000


#define CONN_STATUS_LED DK_LED3


#define SERVO_MOTOR    DT_NODELABEL(servo)
#define PWM_SERVO_MIN_DUTY_CYCLE  DT_PROP(SERVO_MOTOR, min_pulse)
#define PWM_SERVO_MAX_DUTY_CYCLE  DT_PROP(SERVO_MOTOR, max_pulse)


void on_connected(struct bt_conn *conn, uint8_t err);
void on_disconnected(struct bt_conn *conn, uint8_t reason);
void on_notif_changed(enum bt_button_notifications_enabled status);
void on_data_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len);

struct bt_conn_cb bluetooth_callbacks = {
	.connected 	= on_connected,
	.disconnected 	= on_disconnected,
};
struct bt_remote_service_cb remote_callbacks = {
	.notif_changed = on_notif_changed,
	.data_received = on_data_received,
};

void on_data_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{	
	int button_pressed=0;
    uint8_t temp_str[len+1];
    memcpy(temp_str, data, len);
    temp_str[len] = 0x00;

    LOG_INF("Received data on conn %p. Len: %d", (void *)conn, len);
    LOG_INF("Data: %s", temp_str);

	if(temp_str[0]==0x00 ){
		button_pressed=1;
		set_motor_angle(PWM_SERVO_MIN_DUTY_CYCLE);
	}
	else if (temp_str[0]==0x01)
	{
		button_pressed=2;
		set_motor_angle(PWM_SERVO_MAX_DUTY_CYCLE);
	}
	LOG_INF("Remote button %d pressed.", button_pressed);
	set_button_press(button_pressed);	
}

static struct bt_conn *current_conn;    // Used to keep track of current connection status.

/* Callbacks, also in main.c: */

void on_connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        LOG_ERR("connection failed, err %d", err);
    }
    current_conn = bt_conn_ref(conn);
    dk_set_led_on(CONN_STATUS_LED);
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("Disconnected (reason: %d)", reason);
	dk_set_led_off(CONN_STATUS_LED);
	if(current_conn) {
		bt_conn_unref(current_conn);
		current_conn = NULL;
	}
}

void on_notif_changed(enum bt_button_notifications_enabled status)
{
    if (status == BT_BUTTON_NOTIFICATIONS_ENABLED) {
        LOG_INF("Notifications enabled");
    } else {
        LOG_INF("Notifications disabled");
    }
}


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
		LOG_INF("Button %d pressed.", button_pressed);
	
		set_button_press(button_pressed);
		err = send_button_notification(current_conn, button_pressed);
		if (err){
			LOG_ERR ("Couldn't send notification (err: %d)", err);
		}
	}
}

static void configure_dk_buttons_and_leds(void){
	
	int err;
	err = dk_leds_init();
	if (err){
		LOG_ERR("Couldn't init LEDS (err % d)", err);
	}

	
	err = dk_buttons_init(button_handler);
	if (err){
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
	err = bluetooth_init(&bluetooth_callbacks, &remote_callbacks);
    if (err) {
        LOG_ERR("Bluetooth_init() failed. (err %d)", err);
    }
	
	for (;;){
		dk_set_led(RUN_STATUS_LED, (blink_status));
		blink_status = !blink_status;
		k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));

	}

	return 0;
}
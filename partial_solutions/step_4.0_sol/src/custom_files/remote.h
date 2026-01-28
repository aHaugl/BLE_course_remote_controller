#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

//Bluetooth configs
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>

void bt_ready_callback(int err);
int bluetooth_init();
/* Step 4.0.4 - Add kernel and logging */
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* Step 4.0.6 - Add BLE headers */
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>

/* Step 4.1.2 - Add Remote Service UUID */
#define BT_UUID_REMOTE_SERV_VAL \
BT_UUID_128_ENCODE(0x020f0001, 0xbc61, 0x4d87, 0x9319, 0x9aff590fd64c)
/* Step 4.1.3 - define the service*/
#define BT_UUID_REMOTE_SERVICE  BT_UUID_DECLARE_128(BT_UUID_REMOTE_SERV_VAL)


/* Step 4.1.1 - Declare callback */
void bt_ready_callback(int err);

/* Step 4.0.5 - Declare init function*/
int bluetooth_init(struct bt_conn_cb * bt_cb);

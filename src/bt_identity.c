/*
 * Give the keyboard a Bluetooth identity address chosen by firmware.
 *
 * Zephyr only falls back to the chip's factory address when no identity
 * exists at the time Bluetooth is enabled, so creating one here, before
 * ZMK's own BLE init, makes it the address the keyboard advertises with.
 * The address is a static random address (top two bits set) hashed from
 * CONFIG_EQ60_BT_IDENTITY_SEED and the chip's factory id, so two boards
 * running the same config still get different addresses.
 */

#include <zephyr/init.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/logging/log.h>
#include <soc.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int eq60_bt_identity_init(void) {
    const uint32_t words[3] = {
        (uint32_t)CONFIG_EQ60_BT_IDENTITY_SEED,
        NRF_FICR->DEVICEADDR[0],
        NRF_FICR->DEVICEADDR[1],
    };
    const uint8_t *p = (const uint8_t *)words;

    /* FNV-1a, 64-bit */
    uint64_t h = 0xcbf29ce484222325ULL;
    for (size_t i = 0; i < sizeof(words); i++) {
        h ^= p[i];
        h *= 0x100000001b3ULL;
    }

    bt_addr_le_t addr = {.type = BT_ADDR_LE_RANDOM};
    for (int i = 0; i < 6; i++) {
        addr.a.val[i] = (uint8_t)(h >> (8 * i));
    }
    BT_ADDR_SET_STATIC(&addr.a);

    int ret = bt_id_create(&addr, NULL);
    if (ret < 0) {
        LOG_ERR("Failed to create BT identity (seed %d): %d", CONFIG_EQ60_BT_IDENTITY_SEED, ret);
        return ret;
    }

    LOG_INF("BT identity seed %d -> %02X:%02X:%02X:%02X:%02X:%02X", CONFIG_EQ60_BT_IDENTITY_SEED,
            addr.a.val[5], addr.a.val[4], addr.a.val[3], addr.a.val[2], addr.a.val[1],
            addr.a.val[0]);
    return 0;
}

SYS_INIT(eq60_bt_identity_init, APPLICATION, CONFIG_EQ60_BT_IDENTITY_INIT_PRIORITY);

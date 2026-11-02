# Equals 60 (nice!nano v2, wireless, no screen)

ZMK config for the Boardsource Equals 60, built with the `boardsource_equals60_diy`
shield defined in `boards/shields/` (the DIY PCB has its top two rows swapped
compared to ZMK's stock 5x12 shield) on a `nice_nano_v2`. ZMK is pinned to `v0.3` in `config/west.yml`.

Firmware is built by GitHub Actions on every push. Download the artifact,
unzip it, and you get two files:

| File | Purpose |
|------|---------|
| `boardsource_equals60_diy-nice_nano_v2-zmk.uf2` | the keyboard firmware |
| `settings_reset-nice_nano_v2-zmk.uf2` | wipes all saved settings (bonds, name, slot) |

## Flashing

1. Double-tap the reset button. A `NICENANO` drive appears.
2. Copy the `.uf2` onto it. The drive disappears when done.

## First flash after this change: do a full reset

The keyboard name changed and the old pairings on the keyboard side are
probably the source of the trouble, so start clean once:

1. Flash `settings_reset-...uf2`, wait for the drive to vanish.
2. Flash `boardsource5x12-...uf2`.
3. On every computer that ever paired with this keyboard, open Bluetooth
   settings and **forget / remove** "Boardsource 5x12" and "eq60".
4. Pair again as described below.

## Bluetooth address and name

The keyboard's Bluetooth address is chosen by the firmware
(`src/bt_identity.c`) from `CONFIG_EQ60_BT_IDENTITY_SEED` in the `.conf`
file, instead of the fixed address burned into the chip. Hosts identify a
keyboard by its address, so:

- To make every computer that ever paired with this keyboard treat it as a
  brand-new device, bump the seed (1 -> 2), rebuild, flash `settings_reset`,
  then flash the firmware. The old entry stays in each computer's Bluetooth
  list until you remove it, but it is inert and never conflicts.
- To rename the keyboard, change `CONFIG_ZMK_KEYBOARD_NAME` (16 characters
  max). Computers cache the name per address, so bump the seed at the same
  time or they keep showing the old name.

## How Bluetooth slots work

The keyboard has 5 slots. Each slot remembers one computer. Only the active
slot sends keystrokes, even if other computers show the keyboard as
"connected". The keyboard advertises for pairing **only when the active slot
is empty**. Selecting a slot that already belongs to another computer and
trying to pair a new one does nothing, which looks like "pairing silently
fails".

All Bluetooth keys are on the Adjust layer: hold the bottom-left key.

| Keys | Action |
|------|--------|
| Adjust + 1 … 5 | switch to slot 1 … 5 |
| Adjust + Backspace | clear the active slot (it now advertises for pairing) |
| Adjust + B | force output over Bluetooth |
| Adjust + U | force output over USB |
| Adjust + Space | bootloader |

### Pair a computer

1. Hold Adjust, press the slot number you want to give this computer.
2. Hold Adjust, press Backspace. The slot is now empty and the keyboard is
   advertising. Nothing visible happens on the keyboard; that is normal.
3. If this computer was ever paired with the keyboard before, forget it in
   the computer's Bluetooth settings first. Otherwise it keeps an old key and
   the connection fails.
4. Pair "eq60" from the computer. Type something to confirm.

### Switch between computers

Hold Adjust and press the slot number. The other computer may still show the
keyboard as connected; only the selected slot gets keystrokes.

### Keystrokes stop while charging

ZMK prefers USB output whenever a USB cable is plugged in, even into a
charger. Press Adjust + B to force Bluetooth, or Adjust + U to go back.

### Nothing works and you are lost

Flash the `settings_reset` firmware, then the normal firmware, forget the
keyboard on all computers, and pair again. That is the only way to clear all
five slots at once.

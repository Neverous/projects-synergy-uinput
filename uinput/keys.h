/* 2013
 * Maciej Szeptuch (Neverous) <neverous@neverous.info>
 *
 * Synergy protocol 1.4 keys/buttons mapping for use with uinput
 */
#ifndef KEYS_H
#define KEYS_H

#include <stdint.h>
#include <linux/input.h>

// Mouse button mapping
static const uint16_t BTN_MAP[] =
{
    [1]                         = BTN_LEFT,
    [2]                         = BTN_MIDDLE,
    [3]                         = BTN_RIGHT,
    [4]                         = BTN_BACK,         // FIXME: MOUSE WHEEL NOT WORKING!
    [5]                         = BTN_FORWARD,      // ...
    [6]                         = BTN_SIDE,
};

#endif

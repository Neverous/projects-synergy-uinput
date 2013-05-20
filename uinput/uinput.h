/* 2013
 * Maciej Szeptuch (Neverous) <neverous@neverous.info>
 *
 * Uinput access functions.
 */
#ifndef UINPUT_DEVICES_H
#define UINPUT_DEVICES_H

#include <stdint.h>

int32_t uInitializeMouse(uint16_t width, uint16_t height);
int32_t uInitializeKeyboard(void);
void uClose(int32_t device);

void uMouseMotion(int32_t device, const uint16_t x, const uint16_t y);
void uMouseRelativeMotion(int32_t device, const int16_t dx, const int16_t dy);
void uMouseWheel(int32_t device, const int16_t dx, const int16_t dy);
void uMouseButton(int32_t device, const uint8_t button, const uint8_t state);

void uKey(int32_t device, const uint16_t key, const uint8_t state);

#endif

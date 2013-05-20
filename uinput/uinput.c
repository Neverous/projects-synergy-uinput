/* 2013
 * Maciej Szeptuch (Neverous) <neverous@neverous.info>
 *
 * Uinput access functions.
 *
 * TODO:
 *  - Mouse wheel (REL_WHEEL are not forwarded now??, what am i missing)
 *  - Mouse relative movement(probably need separate devices since xorg support only abs or rel per device not both)
 *  - Gamepad support?
 */

#include <linux/uinput.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "uinput.h"
#include "keys.h"

int32_t uInitializeMouse(uint16_t width, uint16_t height)
{
    int32_t device = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(device < 0)
    {
        perror("Cannot open");
        return -1;
    }

    if(ioctl(device, UI_SET_EVBIT, EV_SYN) < 0) {perror("IOCTL error"); close(device); return -1;}
    if(ioctl(device, UI_SET_EVBIT, EV_KEY) < 0) {perror("IOCTL error"); close(device); return -1;}
    if(ioctl(device, UI_SET_EVBIT, EV_ABS) < 0) {perror("IOCTL error"); close(device); return -1;}
    if(ioctl(device, UI_SET_EVBIT, EV_REL) < 0) {perror("IOCTL error"); close(device); return -1;}

    // MOUSE SUPPORT
    if(ioctl(device, UI_SET_KEYBIT, BTN_MOUSE) < 0) {perror("IOCTL error"); close(device); return -1;}
    if(ioctl(device, UI_SET_RELBIT, REL_WHEEL) < 0) {perror("IOCTL error"); close(device); return -1;}
    if(ioctl(device, UI_SET_RELBIT, REL_HWHEEL) < 0) {perror("IOCTL error"); close(device); return -1;}

    for(uint32_t b = 0, buttons = sizeof(BTN_MAP) / sizeof(BTN_MAP[0]); b < buttons; ++ b)
        if(ioctl(device, UI_SET_KEYBIT, BTN_MAP[b]) < 0) {perror("IOCTL error"); close(device); return -1;}

    if(ioctl(device, UI_SET_ABSBIT, ABS_X) < 0) {perror("IOCTL error"); close(device); return -1;}
    if(ioctl(device, UI_SET_ABSBIT, ABS_Y) < 0) {perror("IOCTL error"); close(device); return -1;}

    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "synergyMouse");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    uidev.absmax[ABS_X] = width;
    uidev.absmax[ABS_Y] = height;

    if(write(device, &uidev, sizeof(uidev)) < 0) {perror("Write error"); close(device); return -1;}
    if(ioctl(device, UI_DEV_CREATE) < 0) {perror("IOCTL error"); close(device); return -1;}

    sleep(2);
    return device;
}

int32_t uInitializeKeyboard(void)
{
    int32_t device = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(device < 0)
    {
        perror("Cannot open");
        return -1;
    }

    if(ioctl(device, UI_SET_EVBIT, EV_SYN) < 0) {perror("IOCTL error"); close(device); return -1;}
    if(ioctl(device, UI_SET_EVBIT, EV_KEY) < 0) {perror("IOCTL error"); close(device); return -1;}
    if(ioctl(device, UI_SET_EVBIT, EV_MSC) < 0) {perror("IOCTL error"); close(device); return -1;}

    // KEYBOARD SUPPORT
    for(uint32_t k = 0; k < 256; ++ k)
        if(ioctl(device, UI_SET_KEYBIT, KEY_RESERVED + k) < 0) {perror("IOCTL error"); close(device); return -1;}

    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "synergyKeyboard");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1;
    uidev.id.product = 0x2;
    uidev.id.version = 1;

    if(write(device, &uidev, sizeof(uidev)) < 0) {perror("Write error"); close(device); return -1;}
    if(ioctl(device, UI_DEV_CREATE) < 0) {perror("IOCTL error"); close(device); return -1;}

    sleep(2);
    return device;
}

void uClose(int32_t device)
{
    if(device < 0)
        return;

    ioctl(device, UI_DEV_DESTROY);
    close(device);
}

void uSync(int32_t device)
{
    if(device < 0)
        return;

    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    if(write(device, &ev, sizeof(ev)) < 0)
        perror("Write error");
}

void uMouseMotion(int32_t device, const uint16_t x, const uint16_t y)
{
    if(device < 0)
        return;

    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_ABS;
    ev.code = ABS_X;
    ev.value = x;
    if(write(device, &ev, sizeof(ev)) < 0)
        perror("Write error");

    ev.code = ABS_Y;
    ev.value = y;
    if(write(device, &ev, sizeof(ev)) < 0)
        perror("Write error");

    uSync(device);
}

void uMouseRelativeMotion(int32_t device, const int16_t dx, const int16_t dy)
{
    if(device < 0)
        return;

    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_REL;
    if(dx)
    {
        ev.code = REL_X;
        ev.value = dx;
        if(write(device, &ev, sizeof(ev)) < 0)
            perror("Write error");

        uSync(device);
    }
    
    if(dy)
    {
        ev.code = REL_Y;
        ev.value = dy;
        if(write(device, &ev, sizeof(ev)) < 0)
            perror("Write error");

        uSync(device);
    }
}

void uMouseWheel(int32_t device, const int16_t dx, const int16_t dy)
{
    if(device < 0)
        return;

    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_REL;
    if(dx)
    {
        ev.code = REL_HWHEEL;
        ev.value = dx < 0xFF ? 1 : -1;
        if(write(device, &ev, sizeof(ev)) < 0)
            perror("Write error");

        uSync(device);
    }

    if(dy)
    {
        ev.code = REL_WHEEL;
        ev.value = dy < 0xFF ? 1 : -1;
        if(write(device, &ev, sizeof(ev)) < 0)
            perror("Write error");

        uSync(device);
    }
}

void uMouseButton(int32_t device, const uint8_t button, const uint8_t state)
{
    if(device < 0)
        return;

    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_KEY;
    ev.code = BTN_MAP[button];
    ev.value = state;
    if(write(device, &ev, sizeof(ev)) < 0)
        perror("Write error");

    uSync(device);
}

void uKey(int32_t device, const uint16_t key, const uint8_t state)
{
    if(device < 0)
        return;

    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_KEY;
    ev.code = key;
    ev.value = state;
    if(write(device, &ev, sizeof(ev)) < 0)
        perror("Write error");

    uSync(device);
}

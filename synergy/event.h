/* 2013
 * Maciej Szeptuch (Neverous) <neverous@neverous.info>
 *
 * Event handlers.
 */
#ifndef EVENTS_H
#define EVENTS_H

#include <stdint.h>
#include "client.h"

void eventConnected(SynergyClient *client, const char *host, const uint16_t port);
void eventDisconnected(SynergyClient *client, const char *host, const uint16_t port, const char *reason);
void eventOptionsReset(SynergyClient *client);
void eventOptionsSet(SynergyClient *client, const uint32_t *options);
void eventFocusIn(SynergyClient *client, const uint16_t x, const uint16_t y, const uint16_t mask, const uint16_t seq);
void eventFocusOut(SynergyClient *client);
void eventMouseMotion(SynergyClient *client, const uint16_t x, const uint16_t y);
void eventMouseRelativeMotion(SynergyClient *client, const int16_t dx, const int16_t dy);
void eventMouseWheel(SynergyClient *client, const uint16_t dx, const uint16_t dy);
void eventMouseButtonDown(SynergyClient *client, const uint16_t button);
void eventMouseButtonUp(SynergyClient *client, const uint16_t button);
void eventKeyDown(SynergyClient *client, const uint16_t key, const uint16_t mask, const uint16_t button);
void eventKeyRepeat(SynergyClient *client, const uint16_t key, const uint16_t mask, const uint16_t count, const uint16_t button);
void eventKeyUp(SynergyClient *client, const uint16_t key, const uint16_t mask, const uint16_t button);

#endif

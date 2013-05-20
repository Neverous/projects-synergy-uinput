/* 2013
 * Maciej Szeptuch (Neverous) <neverous@neverous.info>
 *
 * Synergy client.
 */
#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>

#include "../log/log.h"

typedef struct _client
{
    char *name;
    char *host;
    int32_t socket;
    uint16_t port;
    uint16_t width, height;
    Logger log;
} SynergyClient;

uint8_t sConnect(SynergyClient *client);
void sDisconnect(SynergyClient *client, const char *reason);
uint32_t sRead(SynergyClient *client, char *message, uint32_t bufsize);
uint8_t sWrite(SynergyClient *client, const char *data);
void sProcess(SynergyClient *client);

#endif

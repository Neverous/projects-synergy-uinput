/* 2013
 * Maciej Szeptuch (Neverous) <neverous@neverous.info>
 *
 * Synergy client
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include "client.h"
#include "event.h"
#include "protocol.h"
#include "../log/log.h"

uint8_t sConnect(SynergyClient *client)
{
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int32_t opt = 0;

    NOTICE(&client->log, "Connecting to %s:%u.", client->host, client->port);
    client->socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client->socket < 0)
    {
        ERROR(&client->log, "Error opening socket: %s!", strerror(errno));
        close(client->socket);
        client->socket = 0;
        return 0;
    }

    if(setsockopt(client->socket, IPPROTO_TCP, TCP_NODELAY, (char *) &opt, sizeof(opt)) < 0)
        WARNING(&client->log, "Cannot set TCP_NODELAY: %s!", strerror(errno));
    
    NOTICE(&client->log, "Looking for %s address.", client->host);
    server = gethostbyname(client->host);
    if(!server)
    {
        ERROR(&client->log, "Cannot get host address: %s!", strerror(errno));
        close(client->socket);
        client->socket = 0;
        return 0;
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(client->port);
    NOTICE(&client->log, "Connecting...");
    if(connect(client->socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        ERROR(&client->log, "Cannot connect: %s!", strerror(errno));
        close(client->socket);
        client->socket = 0;
        return 0;
    }

    INFO(&client->log, "Connected to %s:%u.", client->host, client->port);
    eventConnected(client, client->host, client->port);
    return 1;
}

void sDisconnect(SynergyClient *client, const char *reason)
{
    NOTICE(&client->log, "Disconnecting from %s:%u.", client->host, client->port);
    shutdown(client->socket, SHUT_RDWR);
    close(client->socket);
    client->socket = 0;
    INFO(&client->log, "Disconnected from %s:%u, reason: %s.", client->host, client->port, reason);
    eventDisconnected(client, client->host, client->port, reason);
}

uint32_t sRead(SynergyClient *client, char *message, uint32_t bufsize)
{
    char ssize[4];
    uint32_t size;

    NOTICE(&client->log, "Reading data.");
    if(read(client->socket, ssize, 4) != 4)
    {
        ERROR(&client->log, "Read error: %s!", strerror(errno));
        return 0;
    }

    if(!pUnpack(ssize, "%4i", &size))
    {
        ERROR(&client->log, "Read error: cannot unpack message size!");
        return 0;
    }

    if(size + 1 > bufsize)
    {
        FATAL(&client->log, "Read error: message bigger than buffer!");
        return 0;
    }

    if(read(client->socket, message, size) != size)
    {
        ERROR(&client->log, "Read error: %s!", strerror(errno));
        return 0;
    }

    message[size] = 0;
    DEBUG(&client->log, "READ: %s", message);
    NOTICE(&client->log, "Data read.");
    return size;
}

uint8_t sWrite(SynergyClient *client, const char *data)
{
    uint32_t size;
    NOTICE(&client->log, "Writing data.");
    if(!pUnpack(data, "%4i", &size))
    {
        ERROR(&client->log, "Write error: no message size in data!");
        return 0;
    }

    if(write(client->socket, data, size + 4) != size + 4)
    {
        ERROR(&client->log, "Write error: %s!", strerror(errno));
        return 0;
    }

    DEBUG(&client->log, "WRITE: %s", data + 4);
    NOTICE(&client->log, "Data written.");
    return 1;
}

void sProcess(SynergyClient *client)
{
    uint8_t times = 0;
    uint32_t size = 0;
    char message[8192];
    NOTICE(&client->log, "Started processing.");
    while(1)
    {
        while(!client->socket)
        {
            NOTICE(&client->log, "Retry in %us!", 2 << times);
            sleep(2 << times);
            times = (times + 1) & 7;
            sConnect(client);
        }

        size = sRead(client, message, 8192);
        if(!message)
        {
            sDisconnect(client, "read error");
            continue;
        }

        NOTICE(&client->log, "Processing: %u %4s.", size, message);
        #define CMP(msg) if(!memcmp(message, msg, 4))
        CMP(MSG_HANDSHAKEREQUEST)
        {
            uint32_t major = 0, minor = 0;
            pUnpack(message, MSG_HANDSHAKEREQUEST, &major, &minor);
            INFO(&client->log, "Connected to server protocol version %u.%u!", major, minor);
            char reply[4096];
            pPack(reply, MSG_HANDSHAKEREPLY, PROTOCOL_MAJOR, PROTOCOL_MINOR, client->name);
            if(!sWrite(client, reply))
                sDisconnect(client, "write error");
        }

        else CMP(MSG_INFOREQUEST)
        {
            NOTICE(&client->log, "Screen info request");
            char reply[4096];
            pPack(reply, MSG_INFOREPLY, 0, 0, client->width, client->height, 0, client->width / 2, client->height / 2);
            if(!sWrite(client, reply))
                sDisconnect(client, "write error");
        }

        else CMP(MSG_INFOACKNOWLEDGED)
        {
            INFO(&client->log, "Client recognized!");
        }

        else CMP(MSG_OPTIONSSET)
        {
            uint32_t options[1048576];
            pUnpack(message, MSG_OPTIONSSET, options);
            NOTICE(&client->log, "Server set options.");
            eventOptionsSet(client, options);
        }

        else CMP(MSG_OPTIONSRESET)
        {
            NOTICE(&client->log, "Server requested options reset!");
            eventOptionsReset(client);
        }

        else CMP(MSG_NOOP)
        {
            DEBUG(&client->log, "NOOP");
        }

        else CMP(MSG_CLOSE)
        {
            INFO(&client->log, "Server disconnected!");
            sDisconnect(client, "server disconnected");
        }

        else CMP(MSG_ENTER)
        {
            uint32_t x = 0, y = 0, seq = 0, mask = 0;
            pUnpack(message, MSG_ENTER, &x, &y, &seq, &mask);
            INFO(&client->log, "Client in focus: %u %u %u %u.", x, y, seq, mask);
            eventFocusIn(client, x, y, seq, mask);
        }

        else CMP(MSG_LEAVE)
        {
            INFO(&client->log, "Client out of focus.");
            eventFocusOut(client);
        }

        else CMP(MSG_INCOMPATIBLECLIENT)
        {
            uint32_t major = 0, minor = 0;
            pUnpack(message, MSG_INCOMPATIBLECLIENT, &major, &minor);
            ERROR(&client->log, "Incompatible server protocol %u.%u vs %u.%u", major, minor, PROTOCOL_MAJOR, PROTOCOL_MINOR);
        }

        else CMP(MSG_NAMEINUSE)
        {
            WARNING(&client->log, "Name already in use!");
            sDisconnect(client, "name in use");
        }

        else CMP(MSG_UNKNOWNNAME)
        {
            ERROR(&client->log, "Name not recognized by server!");
            sDisconnect(client, "unknown name");
        }

        else CMP(MSG_PROTOCOLERROR)
        {
            ERROR(&client->log, "Protocol error!");
            sDisconnect(client, "protocol error!");
        }

        else CMP(MSG_KEEPALIVE)
        {
            NOTICE(&client->log, "Keep alive!");
            char reply[4096];
            pPack(reply, MSG_KEEPALIVE);
            if(!sWrite(client, reply))
                sDisconnect(client, "write error");
        }

        else CMP(MSG_KEYDOWN)
        {
            uint32_t id = 0, mask = 0, button = 0;
            pUnpack(message, MSG_KEYDOWN, &id, &mask, &button);
            NOTICE(&client->log, "Key down %u %u %u!", id, mask, button);
            eventKeyDown(client, id, mask, button);
        }

        else CMP(MSG_KEYUP)
        {
            uint32_t id = 0, mask = 0, button = 0;
            pUnpack(message, MSG_KEYUP, &id, &mask, &button);
            NOTICE(&client->log, "Key up %u %u %u!", id, mask, button);
            eventKeyUp(client, id, mask, button);
        }
        else CMP(MSG_KEYREPEAT)
        {
            uint32_t id = 0, mask = 0, count = 0, button = 0;
            pUnpack(message, MSG_KEYREPEAT, &id, &mask, &count, &button);
            NOTICE(&client->log, "Key repeat %u %u %u!", id, mask, count, button);
            eventKeyRepeat(client, id, mask, count, button);
        }

        else CMP(MSG_MOUSEDOWN)
        {
            uint32_t button = 0;
            pUnpack(message, MSG_MOUSEDOWN, &button);
            NOTICE(&client->log, "Mouse button down %u!", button);
            eventMouseButtonDown(client, button);
        }

        else CMP(MSG_MOUSEUP)
        {
            uint32_t button = 0;
            pUnpack(message, MSG_MOUSEUP, &button);
            NOTICE(&client->log, "Mouse button up %u!", button);
            eventMouseButtonUp(client, button);
        }

        else CMP(MSG_MOUSEMOTION)
        {
            uint32_t x = 0, y = 0;
            pUnpack(message, MSG_MOUSEMOTION, &x, &y);
            NOTICE(&client->log, "Mouse motion (%u, %u)", x, y);
            eventMouseMotion(client, x, y);
        }

        else CMP(MSG_MOUSERELATIVEMOTION)
        {
            uint32_t dx = 0, dy = 0;
            pUnpack(message, MSG_MOUSERELATIVEMOTION, &dx, &dy);
            NOTICE(&client->log, "Mouse relative motion (%u, %u)", dx, dy);
            eventMouseRelativeMotion(client, dx, dy);
        }

        else CMP(MSG_MOUSEWHEEL)
        {
            uint32_t dx = 0, dy = 0;
            pUnpack(message, MSG_MOUSEWHEEL, &dx, &dy);
            NOTICE(&client->log, "Mouse wheel (%u, %u)", dx, dy);
            eventMouseWheel(client, dx, dy);
        }

        else
        {
            WARNING(&client->log, "Invalid message from server: %s!", message);
        }

        #undef CMP
    }

    NOTICE(&client->log, "Processing finished.");
}

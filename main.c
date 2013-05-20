/* 2013
 * Maciej Szeptuch (Neverous) <neverous@neverous.info>
 *
 * Synergy-uinput.
 * ----------
 *  Synergy client for use with uinput.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#include "log/log.h"
#include "synergy/event.h"
#include "synergy/client.h"
#include "uinput/uinput.h"

// Usage options and info
const char *VERSION = "0.2.1";
const char *HELP = "Usage: synergy-uinput [options]\n\n\
    -h --help                       Display this usage information.\n\
    -V --version                    Display program version.\n\
    -v --verbose                    Increase verbosity level.\n\
    -d --debug LEVEL[=info]         Set verbosity level to LEVEL [error, warning, info, notice, debug].\n\
    -l --log FILE[=stderr]          Set log file\n\
    -c --host HOST[=localhost]      Address of synergy server.\n\
    -p --port PORT[=24800]          Port of synergy server.\n\
    -W --width WIDTH[=1]            Screen width for mouse.\n\
    -H --height HEIGHT[=1]          Screen height for mouse.\n\
    -N --name NAME[=synergy-uinput] SynergyClient name.";

const char *SHORT_OPTIONS = "hVvd:c:p:N:W:H:l:";
const struct option LONG_OPTIONS[] = 
{
    {"help",    no_argument,        0, 'h'}, // display help and usage information
    {"version", no_argument,        0, 'V'}, // display version
    {"verbose", no_argument,        0, 'v'}, // set log level to notice [error, warning, info, notice, debug]
    {"debug",   required_argument,  0, 'd'}, // manually set log level
    {"log",     required_argument,  0, 'l'}, // set log file
    {"host",    required_argument,  0, 'c'}, // connection host
    {"port",    required_argument,  0, 'p'}, // connection port
    {"name",    required_argument,  0, 'N'}, // client name
    {"width",   required_argument,  0, 'W'}, // screen width
    {"height",  required_argument,  0, 'H'}, // screen height
    {0, 0, 0, 0},
};

SynergyClient client = {
    "synergy-uinput",
    "localhost",
    0,
    24800,
    1, 1, 
    {LOG_INFO, 0},
};

uint32_t loglevel;
char *logfile;

int32_t mouse,
        keyboard;

void sigbreak(int signal)
{
    INFO(&client.log, "Catched signal. Closing.");
    sDisconnect(&client, "catched signal");
    uClose(mouse);
    uClose(keyboard);
    lClose(&client.log);
    exit(0);
}

int32_t main(int32_t argc, char **argv)
{
    signal(SIGINT, sigbreak);
    signal(SIGKILL, sigbreak);
    int32_t o;

    while((o = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS, 0)) != -1) switch(o)
    {
        case 'h': puts(HELP);
            return 0;

        case 'V': printf("synergy-uinput %s\n", VERSION);
            return 0;

        case 'v': loglevel = LOG_NOTICE;
            break;

        case 'd':
            if(memcmp(optarg, "error", 5) == 0)         loglevel = LOG_ERROR;
            else if(memcmp(optarg, "warning", 7) == 0)  loglevel = LOG_WARNING;
            else if(memcmp(optarg, "info", 4) == 0)     loglevel = LOG_INFO;
            else if(memcmp(optarg, "notice", 6) == 0)   loglevel = LOG_NOTICE;
            else if(memcmp(optarg, "debug", 5) == 0)    loglevel = LOG_DEBUG;
            else                                        {fputs(HELP, stderr); return 1;}
            break;

        case 'c': client.host = optarg;
            break;

        case 'p': client.port = atoi(optarg);
            break;

        case 'N': client.name = optarg;
            break;

        case 'W': client.width = atoi(optarg);
            break;

        case 'H': client.height = atoi(optarg);
            break;

        case 'l': logfile = optarg;
            break;

        case '?': fputs(HELP, stderr);
            return 1;
    }

    lOpen(&client.log, logfile, loglevel);
    NOTICE(&client.log, "Synergy-uinput configured with %s name, %dx%d resolution and %s:%d address.", client.name, client.width, client.height, client.host, client.port);
    keyboard = uInitializeKeyboard();
    mouse = uInitializeMouse(client.width, client.height);
    if(keyboard < 0 || mouse < 0)
    {
        ERROR(&client.log, "Cannot create uinput devices.");
        return 1;
    }

    sConnect(&client);
    sProcess(&client);
    sDisconnect(&client, "client exit");
    uClose(mouse);
    uClose(keyboard);
    lClose(&client.log);
    return 0;
}

/* Event handlers */
void eventConnected(SynergyClient *client, const char *host, const uint16_t port){}
void eventDisconnected(SynergyClient *client, const char *host, const uint16_t port, const char *reason){}
void eventOptionsReset(SynergyClient *client){}
void eventOptionsSet(SynergyClient *client, const uint32_t *options){}
void eventFocusOut(SynergyClient *client){}

void eventFocusIn(SynergyClient *client, const uint16_t x, const uint16_t y, const uint16_t mask, const uint16_t seq)
{
    uMouseMotion(mouse, x, y);
    return;
}


void eventMouseMotion(SynergyClient *client, const uint16_t x, const uint16_t y)
{
    uMouseMotion(mouse, x, y);
    return;
}

void eventMouseRelativeMotion(SynergyClient *client, const int16_t dx, const int16_t dy)
{
    uMouseRelativeMotion(mouse, dx, dy);
    return;
}

void eventMouseWheel(SynergyClient *client, const uint16_t dx, const uint16_t dy)
{
    uMouseWheel(mouse, dx, dy);
    return;
}

void eventMouseButtonDown(SynergyClient *client, const uint16_t button)
{
    uMouseButton(mouse, button, 1);
    return;
}

void eventMouseButtonUp(SynergyClient *client, const uint16_t button)
{
    uMouseButton(mouse, button, 0);
    return;
}

void eventKeyDown(SynergyClient *client, const uint16_t key, const uint16_t mask, const uint16_t button)
{
    uKey(keyboard, button - 8, 1);
    return;
}

void eventKeyUp(SynergyClient *client, const uint16_t key, const uint16_t mask, const uint16_t button)
{
    uKey(keyboard, button - 8, 0);
    return;
}

void eventKeyRepeat(SynergyClient *client, const uint16_t key, const uint16_t mask, const uint16_t count, const uint16_t button)
{
    uKey(keyboard, button - 8, 2);
    return;
}

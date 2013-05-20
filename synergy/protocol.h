/* 2013
 * Maciej Szeptuch (Neverous) <neverous@neverous.info>
 *
 * Synergy protocol 1.4 implementation for use with uinput
 */
#ifndef SYNERGY_PROTOCOL_H
#define SYNERGY_PROTOCOL_H

#include <stdarg.h>
#include <stdint.h>

// Protocol version
#define PROTOCOL_MAJOR  1
#define PROTOCOL_MINOR  4

// Available messages
#define MSG_HANDSHAKEREQUEST        "Synergy%2i%2i"             // major version, minor version
#define MSG_HANDSHAKEREPLY          "Synergy%2i%2i%s"           // major version, minor version, client name
#define MSG_INFOREQUEST             "QINF"
#define MSG_INFOREPLY               "DINF%2i%2i%2i%2i%2i%2i%2i" // [screen] x, y, width, height, wrap?, [mouse] pos x, pos y
#define MSG_INFOACKNOWLEDGED        "CIAK"
#define MSG_OPTIONSSET              "DSOP%4I"                   // options
#define MSG_OPTIONSRESET            "CROP"
#define MSG_NOOP                    "CNOP"
#define MSG_CLOSE                   "CBYE"
#define MSG_ENTER                   "CINN%2i%2i%4i%2i"          // pos x, pos y, sequence number, mask
#define MSG_LEAVE                   "COUT"
#define MSG_INCOMPATIBLECLIENT      "EICV%2i%2i"                // major version, minor version
#define MSG_NAMEINUSE               "EBSY"
#define MSG_UNKNOWNNAME             "EUNK"
#define MSG_PROTOCOLERROR           "EBAD"
#define MSG_CLIPBOARDREQUEST        "CCLP%1i%4i"                // clipboard id, sequence number
#define MSG_CLIPBOARDREPLY          "DCLP%1i%4i%s"              // clipboard id, sequence number, data
#define MSG_SCREENSAVER             "CSEC%1i"                   // state 1/0
#define MSG_KEEPALIVE               "CALV"
#define MSG_KEYDOWN                 "DKDN%2i%2i%2i"             // key id, mask, button
#define MSG_KEYUP                   "DKUP%2i%2i%2i"             // key id, mask, button
#define MSG_KEYREPEAT               "DKRP%2i%2i%2i%2i"          // key id, mask, count, button
#define MSG_MOUSEDOWN               "DMDN%1i"                   // button id
#define MSG_MOUSEUP                 "DMUP%1i"                   // button id
#define MSG_MOUSEMOTION             "DMMV%2i%2i"                // x, y
#define MSG_MOUSERELATIVEMOTION     "DMRM%2i%2i"                // dx, dy
#define MSG_MOUSEWHEEL              "DMWM%2i%2i"                // dx, dy
#define MSG_GAMEPADTIMINGREQUEST    "CGRQ"
#define MSG_GAMEPADTIMINGREPLY      "CGRS%2i"                   // frequency
#define MSG_GAMEPADBUTTONS          "DGBT%1i%2i"                // gamepad id, buttons
#define MSG_GAMEPADSTICKS           "DGST%1i%2i%2i%2i%2i"       // gamepad id, [stick1] x, y, [stick2] x, y
#define MSG_GAMEPADTRIGGERS         "DGTR%1i%1i%1i"             // gamepad id, trigger1, trigger2
#define MSG_GAMEPADFEEDBACK         "DGFB%1i%2i%2i"             // gamepad id, m1?, m2?

// Synergy protocol 1.4 formatting implementation
/*
 * Packs data into buffer.
 */
uint32_t pPack(char *buffer, const char *fmt, ...);

/*
 * Unpacks data from buffer.
 */
uint8_t pUnpack(const char *buffer, const char *fmt, ...);

#endif

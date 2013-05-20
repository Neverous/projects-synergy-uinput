/* 2013
 * Maciej Szeptuch (Neverous) <neverous@neverous.info>
 *
 * Synergy protocol 1.4 implementation for use with uinput
 */
#include <string.h>
#include "protocol.h"

/* Helper functions */
uint32_t packInt(char *buffer, const uint32_t size, const uint32_t arg);
uint32_t packInts(char *buffer, const uint32_t size, const uint32_t *arg);
uint32_t packString(char *buffer, const char* arg);

uint32_t unpackInt(const char *buffer, const uint32_t size, uint32_t *arg);
uint32_t unpackInts(const char *buffer, const uint32_t size, uint32_t *arg);
uint32_t unpackString(const char *buffer, char *arg);

/*
 * Packs data into buffer.
 */
uint32_t pPack(char *buffer, const char *fmt, ...)
{
    uint32_t b = 4,     // buffer pos
             size = 0;  // element size
    va_list args; va_start(args, fmt);

    for(uint32_t f = 0; fmt[f]; ++ f)
    {
        if(fmt[f] != '%')
        {
            buffer[b ++] = fmt[f];
            continue;
        }

        ++ f;
        if('0' <= fmt[f] && fmt[f] <= '9')
        {
            size = fmt[f] - '0';
            ++ f;
        }

        switch(fmt[f])
        {
            case 'i':
                b += packInt(buffer + b, size, va_arg(args, uint32_t));
                break;

            case 'I':
                b += packInts(buffer + b, size, va_arg(args, uint32_t *));
                break;

            case 's':
                b += packString(buffer + b, va_arg(args, char *));
                break;
        }
    }

    packInt(buffer, 4, b - 4);
    return b;
}

/*
 * Unpacks data from buffer.
 */
uint8_t pUnpack(const char *buffer, const char *fmt, ...)
{
    uint32_t b = 0,     // buffer pos
             size = 0;  // element size
    va_list args; va_start(args, fmt);

    for(uint32_t f = 0; fmt[f]; ++ f)
    {
        if(fmt[f] != '%')
        {
            if(fmt[f] != buffer[b])
                return 0;

            ++ b;
            continue;
        }

        ++ f;
        if('0' <= fmt[f] && fmt[f] <= '9')
        {
            size = fmt[f] - '0';
            ++ f;
        }

        switch(fmt[f])
        {
            case 'i':
                b += unpackInt(buffer + b, size, va_arg(args, uint32_t *));
                break;

            case 'I':
                b += unpackInts(buffer + b, size, va_arg(args, uint32_t *));
                break;

            case 's':
                b += unpackString(buffer + b, va_arg(args, char *));
                break;
        }
    }

    return 1;
}

uint32_t packInt(char *buffer, const uint32_t size, const uint32_t arg)
{
    for(uint32_t s = 0; s < size; ++ s)
        buffer[s] = (arg >> (8 * (size - s - 1))) & 0xFF;

    return size;
}

uint32_t unpackInt(const char *buffer, const uint32_t size, uint32_t *arg)
{
    *arg = 0;
    for(uint32_t s = 0; s < size; ++ s)
        *arg |= (buffer[s] & 0xFF) << (8 * (size - s - 1));

    return size;
}

uint32_t packInts(char *buffer, const uint32_t size, const uint32_t *arg)
{
    buffer += packInt(buffer, 4, arg[0]);
    for(uint32_t a = 1; a <= arg[0]; ++ a)
        buffer += packInt(buffer, size, arg[a]);

    return arg[0] * size + 4;
}

uint32_t unpackInts(const char *buffer, const uint32_t size, uint32_t *arg)
{
    buffer += unpackInt(buffer, 4, &arg[0]);
    for(uint32_t a = 1; a <= arg[0]; ++ a)
        buffer += unpackInt(buffer, size, &arg[a]);

    return arg[0] * size + 4;
}

uint32_t packString(char *buffer, const char *arg)
{
    uint32_t size = 0;
    buffer += 4;

    while(arg[size] != 0)
    {
        buffer[size] = arg[size];
        ++ size;
    }

    packInt(buffer - 4, 4, size);
    return size + 4;
}

uint32_t unpackString(const char *buffer, char *arg)
{
    uint32_t size = 0;
    buffer += unpackInt(buffer, 4, &size);
    memcpy(arg, buffer, size);
    return size + 4;
}

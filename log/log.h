/* 2013
 * Maciej Szeptuch (Neverous) <neverous@neverous.info>
 *
 * Simple logging utility.
 */
#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

// LOG LEVELS
#define LOG_FATAL 7
#define LOG_CRITICAL 6
#define LOG_ERROR 5
#define LOG_WARNING 4
#define LOG_INFO 3
#define LOG_NOTICE 2
#define LOG_DEBUG 1
#define LOG_NOLOG 0

// MESSAGE TYPES MACROS
#define FATAL(log, ...)      lMessage(log, LOG_FATAL, __VA_ARGS__)
#define CRITICAL(log, ...)   lMessage(log, LOG_CRITICAL, __VA_ARGS__)
#define ERROR(log, ...)      lMessage(log, LOG_ERROR, __VA_ARGS__)
#define WARNING(log, ...)    lMessage(log, LOG_WARNING, __VA_ARGS__)
#define INFO(log, ...)       lMessage(log, LOG_INFO, __VA_ARGS__)
#define NOTICE(log, ...)     lMessage(log, LOG_NOTICE, __VA_ARGS__)
#define DEBUG(log, ...)      lMessage(log, LOG_DEBUG, __VA_ARGS__)

typedef struct _logger
{
    uint8_t loglevel;
    FILE *logfile;
} Logger;

// Open log file(/stderr) and set proper loglevel
void lOpen(Logger *log, const char *filename, const uint8_t loglevel);

// Close log file
void lClose(Logger *log);

// Send message(printf style) with given loglevel
void lMessage(Logger *log, const uint8_t loglevel, const char *fmt, ...);

#endif

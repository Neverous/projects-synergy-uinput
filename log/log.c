/* 2013
 * Maciej Szeptuch (Neverous) <neverous@neverous.info>
 *
 * Simple logging utility.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "log.h"

// Name mapping for log levels
const char *map[8] =
{
    "NOLOG",
    "DEBUG",
    "NOTICE",
    "INFO",
    "WARNING",
    "ERROR",
    "CRITICAL",
    "FATAL"
};

void lOpen(Logger *log, const char *filename, const uint8_t loglevel)
{
    log->loglevel = loglevel;
    if(!filename)
        log->logfile = stderr;

    else
        log->logfile = fopen(filename, "ab");

    if(!log->logfile)
    {
        perror("Cannot open log file!");
        exit(3);
    }

    NOTICE(log, "---------- Log opened! ----------");
}

void lClose(Logger *log)
{
    NOTICE(log, "---------- Log closed! ----------");
    if(log->logfile != stderr)
        fclose(log->logfile);

    log->logfile = 0;
}

void lMessage(Logger *log, uint8_t loglevel, const char *fmt, ...)
{
    if(loglevel < log->loglevel)
        return;

    char buffer[32];
    time_t raw;
    struct tm *local;
    va_list args;

    time(&raw);
    local = localtime(&raw);
    strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", local);
    fprintf(log->logfile, "[%s|%s] ", buffer, map[loglevel]);
    va_start(args, fmt);
    vfprintf(log->logfile, fmt, args);
    va_end(args);
    fputs("\n", log->logfile);
    fflush(log->logfile);
}

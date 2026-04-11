#include "trace_parser.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define LINE_BUF 256
#define EIP_ADDR_OFFSET 10
#define DST_ADDR_OFFSET 6
#define DST_DATA_OFFSET 15
#define SRC_ADDR_OFFSET 27
#define SRC_DATA_OFFSET 36

static TraceEntry pending[2];
static int pendingCount = 0;
static int pendingIndex = 0;

static int isInvalid(const char *dataField) {
    return strncmp(dataField, "--------", 8) == 0;
}

static int readLine(FILE *f, char *buf, int size) {
    while (fgets(buf, size, f) != NULL) {
        int len = (int)strlen(buf);

        while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
            buf[--len] = '\0';
        }

        if (len > 0) {
            return 1;
        }
    }

    return 0;
}

int getNextTraceEntry(FILE *file, TraceEntry *entry) {
    char eipLine[LINE_BUF];
    char dstLine[LINE_BUF];

    if (pendingIndex < pendingCount) {
        *entry = pending[pendingIndex++];
        return 1;
    }

    pendingCount = 0;
    pendingIndex = 0;

    while (1) {
        if (!readLine(file, eipLine, LINE_BUF)) {
            return 0;
        }

        if (strncmp(eipLine, "EIP", 3) == 0) {
            break;
        }
    }

    {
        unsigned int instrAddr = 0;

        if (sscanf(eipLine + EIP_ADDR_OFFSET, "%x", &instrAddr) != 1) {
            return 0;
        }

        entry->operation = 'R';
        entry->virAddr = (uint32_t)instrAddr;
        entry->instructionComplete = 1; /* changed below if dst/src exist */
    }

    if (!readLine(file, dstLine, LINE_BUF)) {
        return 1;
    }

    if (strncmp(dstLine, "dstM", 4) == 0) {
        unsigned int dstAddr = 0;
        unsigned int srcAddr = 0;

        if (sscanf(dstLine + DST_ADDR_OFFSET, "%x", &dstAddr) == 1) {
            if (dstAddr != 0 && !isInvalid(dstLine + DST_DATA_OFFSET)) {
                pending[pendingCount].operation = 'W';
                pending[pendingCount].virAddr = (uint32_t)dstAddr;
                pending[pendingCount].instructionComplete = 0;
                pendingCount++;
            }
        }

        if (sscanf(dstLine + SRC_ADDR_OFFSET, "%x", &srcAddr) == 1) {
            if (srcAddr != 0 && !isInvalid(dstLine + SRC_DATA_OFFSET)) {
                pending[pendingCount].operation = 'R';
                pending[pendingCount].virAddr = (uint32_t)srcAddr;
                pending[pendingCount].instructionComplete = 0;
                pendingCount++;
            }
        }

        if (pendingCount > 0) {
            entry->instructionComplete = 0;
            pending[pendingCount - 1].instructionComplete = 1;
        }
    }

    return 1;
}

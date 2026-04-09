#ifndef TRACE_PARSER_H
#define TRACE_PARSER_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
    char operation;      
    uint32_t virAddr;    
} TraceEntry;


int getNextTraceEntry(FILE *file, TraceEntry *entry);

#endif
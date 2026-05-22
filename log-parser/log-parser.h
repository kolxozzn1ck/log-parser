#pragma once

#define MAX 1024

typedef struct {
    int inf;
    int warn;
    int err;
    int dbg;
    int unk;
} LogStats;

const char* getType(const char* line);
void updStats(LogStats* s, const char* t);
void printStats(LogStats* s);
int filterType(const char* type, const char* filter);
int filterText(const char* line, const char* search);
void help();

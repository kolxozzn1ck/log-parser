#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "log-parser.h"

const char* getType(const char* line) {
    char buf[MAX];
    strncpy(buf, line, MAX - 1);
    buf[MAX - 1] = '\0';

    int i = 0;
    while (buf[i]) {
        buf[i] = tolower(buf[i]);
        i++;
    }

    if (strstr(buf, "[info]") != NULL || strstr(buf, "info:") != NULL) return "INFO";
    if (strstr(buf, "[warning]") != NULL || strstr(buf, "warning:") != NULL) return "WARNING";
    if (strstr(buf, "[error]") != NULL || strstr(buf, "error:") != NULL) return "ERROR";
    if (strstr(buf, "[debug]") != NULL || strstr(buf, "debug:") != NULL) return "DEBUG";

    return "UNKNOWN";
}

void updStats(LogStats* s, const char* t) {
    if (!strcmp(t, "INFO")) s->inf++;
    else if (!strcmp(t, "WARNING")) s->warn++;
    else if (!strcmp(t, "ERROR")) s->err++;
    else if (!strcmp(t, "DEBUG")) s->dbg++;
    else s->unk++;
}

void printStats(LogStats* s) {
    int total = s->inf + s->warn + s->err + s->dbg + s->unk;
    printf("\n=== STATISTICS ===\n");
    printf("INFO: %d\nWARNING: %d\nERROR: %d\nDEBUG: %d\nUNKNOWN: %d\nTOTAL: %d\n",
        s->inf, s->warn, s->err, s->dbg, s->unk, total);
}

int filterType(const char* type, const char* filter) {
    if (filter == NULL || !strcmp(filter, "all")) return 1;

    if (!strcmp(filter, "error") && !strcmp(type, "ERROR")) return 1;
    if (!strcmp(filter, "warning") && !strcmp(type, "WARNING")) return 1;
    if (!strcmp(filter, "info") && !strcmp(type, "INFO")) return 1;
    if (!strcmp(filter, "debug") && !strcmp(type, "DEBUG")) return 1;

    return 0;
}

int filterText(const char* line, const char* search) {
    if (search == NULL || strlen(search) == 0) return 1;

    char l1[MAX], l2[MAX];
    strncpy(l1, line, MAX - 1);
    l1[MAX - 1] = '\0';
    strncpy(l2, search, MAX - 1);
    l2[MAX - 1] = '\0';

    for (int i = 0; l1[i]; i++) l1[i] = tolower(l1[i]);
    for (int i = 0; l2[i]; i++) l2[i] = tolower(l2[i]);

    return (strstr(l1, l2) != NULL);
}

void help() {
    printf("\n=== LOG PARSER ===\n\n");
    printf("Usage:\n");
    printf("  log_parser.exe - run with default file stat.log\n");
    printf("  log_parser.exe [file] - analyze specified file\n");
    printf("  log_parser.exe [file] [type] - filter by type\n");
    printf("  log_parser.exe [file] [type] [text] - filter by type and text\n\n");
    printf("Types: all, error, warning, info, debug\n");
    printf("Examples:\n");
    printf("  log_parser.exe app.log error\n");
    printf("  log_parser.exe app.log error timeout\n");
    printf("  log_parser.exe app.log all database\n");
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "Russian");
    char fname[256] = "stat.log";
    const char* ftype = "all";
    const char* ftext = NULL;

    if (argc > 1) {
        if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            help();
            return 0;
        }
        strncpy(fname, argv[1], sizeof(fname) - 1);
        fname[sizeof(fname) - 1] = '\0';
    }
    if (argc > 2) ftype = argv[2];
    if (argc > 3) ftext = argv[3];

    printf("\n=== LOG PARSER ===\n");
    printf("File: %s\nFilter: %s\n", fname, ftype);
    if (ftext != NULL) printf("Search: %s\n", ftext);

    FILE* f = fopen(fname, "r");
    if (f == NULL) {
        printf("\nERROR: File '%s' not found!\n", fname);
        printf("Check if '%s' exists in the current directory!\n", fname);
        return 1;
    }

    LogStats st = { 0, 0, 0, 0, 0 };
    char buf[MAX];
    int num = 0, shown = 0;

    printf("\n--- RESULTS ---\n");

    while (fgets(buf, sizeof(buf), f)) {
        int l = strlen(buf);
        if (l > 0 && buf[l - 1] == '\n') buf[l - 1] = '\0';

        num++;
        const char* tp = getType(buf);
        updStats(&st, tp);

        if (filterType(tp, ftype) && filterText(buf, ftext)) {
            shown++;
            printf("[%s] %s\n", tp, buf);
        }
    }

    fclose(f);

    if (shown == 0) printf("(nothing found)\n");

    printf("\nTotal lines: %d, shown: %d\n", num, shown);
    printStats(&st);

    return 0;
}

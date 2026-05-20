#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#define MAX 1024

typedef struct {
	int inf;
	int warn;
	int err;
	int dbg;
	int unk;
} LogStats;

const char* getType(const char* line) {
	char buf[MAX];
	strcpy(buf, line);
	int i = 0;
	while (buf[i]) {
		buf[i] = tolower(buf[i]);
		i++;
	}

	if (strstr(buf, "[info]") != 0 || strstr(buf, "info:")) return "INFO";
	if (strstr(buf, "[warning]") || strstr(buf, "warning:")) return "WARNING";
	if (strstr(buf, "[error]") || strstr(buf, "error:")) return "ERROR";
	if (strstr(buf, "[debug]") || strstr(buf, "debug:")) return "DEBUG";

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
	printf("\n=== СТАТИСТИКА ===\n");
	printf("INFO(ИНФО): %d\nWARNING(ВНИМАНИЕ): %d\nERROR(ОШИБКА): %d\nDEBUG(ОТЛАДКА): %d\nUNKNOWN(НЕИЗВЕСТНО): %d\nВСЕГО: %d\n",
		s->inf, s->warn, s->err, s->dbg, s->unk, total);
}

int filterType(const char* type, const char* filter) {
	if (filter == 0 || !strcmp(filter, "all")) return 1;

	if (!strcmp(filter, "error") && !strcmp(type, "ERROR")) return 1;
	if (!strcmp(filter, "warning") && !strcmp(type, "WARNING")) return 1;
	if (!strcmp(filter, "info") && !strcmp(type, "INFO")) return 1;
	if (!strcmp(filter, "debug") && !strcmp(type, "DEBUG")) return 1;

	return 0;
}

int filterText(const char* line, const char* search) {
	if (search == 0 || strlen(search) == 0) return 1;

	char l1[MAX], l2[MAX];
	strcpy(l1, line);
	strcpy(l2, search);

	for (int i = 0; l1[i]; i++) l1[i] = tolower(l1[i]);
	for (int i = 0; l2[i]; i++) l2[i] = tolower(l2[i]);

	return (strstr(l1, l2) != 0);
}

void help() {
	printf("\n=== LOG PARSER (Анализатор логов) ===\n\n");
	printf("Использование:\n");
	printf("  log_parser.exe - запуск с файлом test.log\n");
	printf("  log_parser.exe [файл] - анализ указанного файла\n");
	printf("  log_parser.exe [файл] [тип] - фильтр по типу\n");
	printf("  log_parser.exe [файл] [тип] [текст] - фильтр по типу и тексту\n\n");
	printf("Типы: all, error, warning, info, debug\n");
	printf("Примеры:\n");
	printf("  log_parser.exe app.log error\n");
	printf("  log_parser.exe app.log error timeout\n");
	printf("  log_parser.exe app.log all database\n");
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, "Russian");

	char fname[256] = "stat.log";
	const char* ftype = "all";
	const char* ftext = 0;

	if (argc > 1) {
		if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
			help();
			return 0;
		}
		strcpy(fname, argv[1]);
	}
	if (argc > 2) ftype = argv[2];
	if (argc > 3) ftext = argv[3];

	printf("\n=== LOG PARSER ===\n");
	printf("Файл: %s\nФильтр: %s\n", fname, ftype);
	if (ftext != 0) printf("Поиск: %s\n", ftext);

	FILE* f = fopen(fname, "r");
	if (f == 0) {
		printf("\nОШИБКА: Файл '%s' не найден!\n", fname);
		printf("Проверьте, находится ли %s в папке дериктории!\n", fname);
		return 1;
	}

	LogStats st = { 0,0,0,0,0 };
	char buf[MAX];
	int num = 0, shown = 0;

	printf("\n--- РЕЗУЛЬТАТЫ ---\n");

	while (fgets(buf, sizeof(buf), f)) {
		int l = strlen(buf);
		if (l > 0 && buf[l - 1] == '\n') buf[l - 1] = 0;

		num++;
		const char* tp = getType(buf);
		updStats(&st, tp);

		if (filterType(tp, ftype) && filterText(buf, ftext)) {
			shown++;
			printf("[%s] %s\n", tp, buf);
		}
	}

	fclose(f);

	if (shown == 0) printf("(ничего не найдено)\n");

	printf("\nВсего строк: %d, показано: %d\n", num, shown);
	printStats(&st);

	return 0;
}

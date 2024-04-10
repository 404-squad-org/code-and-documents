#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"

static void run(const char* source) {
	initScanner(source);
	int line = -1;
	for (;;) {
		Token token = scanToken();
		if (token.line != line) {
			printf("%4d ", token.line);
			line = token.line;
		}
		else {
			printf("   | ");
		}
		printf("%2d  %.*s \n", token.type, token.length, token.start);
		if (token.type == TOKEN_EOF) break;
	}
}

static void repl(void) {
	for (;;) {
		char string[1024];
		gets(string);
		run(string);
	}
}

static char* readFile(const char* path) {
	FILE* fp = fopen(path, "rb");
	if (!fp) exit(1);
	fseek(fp, 0L, SEEK_END);
	size_t length = ftell(fp);
	if (length <= 0) exit(1);
	rewind(fp);
	char* string = calloc(length + 1, 1);
	if (!string) exit(1);
	fread(string, 1, length, fp);
	fclose(fp);
	return string;
}

static void runFile(const char* path) {
	char* source = readFile(path);
	run(source);
	free(source);
}

int main(int argc, const char* argv[]) {
	if (argc == 1) {
		repl();
	}
	else if (argc == 2) {
		runFile(argv[1]);
	}
	else {
		fprintf(stderr, "Usage: scanner [path]\n");
		exit(1);
	}
	return 0;
}
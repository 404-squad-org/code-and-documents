#include "scanner.h"
#include<stdio.h>
#include<stdlib.h>
static void run(const char* source) {
	initScanner(source);
	int line = -1;
	// ��ӡToken,����TOKEN_EOFΪֹ
	for (;;) {
		Token token = scanToken();
		if (token.line != line) {
			printf("%4d ", token.line);
			line = token.line;
		}
		else {
			printf("   | ");
		}
		printf("%2d '%.*s'\n", token.type, token.length, token.start);
		if (token.type == TOKEN_EOF) {
			break;
		}
	}
}

	static void repl(){
	// ���û��������û�ÿ����һ�д��룬����һ�д��룬����������
	// repl��"read evaluate print loop"����д
	char line[1024];
	while (1) {
		if (fgets(line, sizeof(line), stdin) == NULL) {
			printf("\n");
			break;
		}
		run(line);
	}
}

static char* readFile(const char* path) {
	// �û������ļ������������ļ������ݶ����ڴ棬����ĩβ���'\0'
	// ע��:����Ӧ��ʹ�ö�̬�ڴ���䣬���Ӧ������ȷ���ļ��Ĵ�С��
	FILE* fp = fopen(path, "r");
	if (errno != 0) {
		perror("not_exist.txt");
	}

	fseek(fp, 0, SEEK_END);
	long int bytes = ftell(fp);
	rewind(fp);

	char* content = malloc(bytes + 1);

	int n = fgets(content,bytes, fp);
	content[n] = '\0';
}

static void runFile(const char* path) {
	// ����'.c'�ļ�:�û������ļ��������������ļ�������������
	char* source = readFile(path);
	run(source);
	free(source);
}
int main(int argc, const char* argv[]) {
	if (argc == 1){
	// ./scanner û�в���,����뽻��ʽ����
		repl();
	}
    else if (argc == 2) {
	 // ./scanner file �����һ������,����������ļ�
		 runFile(argv[1]);
	}
    else {
		fprintf(stderr, "Usage: scanner [path]\n");
		exit(1);
	}

	return 0;
}
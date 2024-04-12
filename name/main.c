#define _CRT_SECURE_NO_WARNINGS

#include "scanner.h"
#include <stdio.h>
#include<stdlib.h>

static void run(const char* source) {
    initScanner(source);
    int line = -1;
    // ��ӡToken, ����TOKEN_EOFΪֹ
    for (;;) {
        // ������һ��Token
        Token token = scanToken();
        // ��ӡ���Token����Ϣ
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        }
        else {
            printf("   |");
        }
        printf("% 2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF) break;
    }
}

static void repl() {
    // ���û��������û�ÿ����һ�д��룬����һ�д��룬����������
    // repl��"read evaluate print loop"����д
    char line[1024];
    for (;;) {
        printf("> ");

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }
        run(line);
    }

}

static char* readFile(const char* path) {
    // �û������ļ������������ļ������ݶ����ڴ棬����ĩβ���'\0'
    // ע��: ����Ӧ��ʹ�ö�̬�ڴ���䣬���Ӧ������ȷ���ļ��Ĵ�С��
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open this file %s.\n", path);
        exit(1);
    }
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enought \
            memory to alloc to read %s\n",path);
        exit(1);
    }
    //������˵���Ѿ�����ã����濪ʼ��ȡ���ݰ�
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "read error :less read %s.\n",path);
        exit(1);
    }
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void runFile(const char* path) {
    // ����'.c'�ļ�:�û������ļ��������������ļ�������������
    char* source = readFile(path);
    run(source);
    free(source);
}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
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
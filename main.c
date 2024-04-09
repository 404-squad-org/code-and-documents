// main.c
#define _CRT_SECURE_NO_WARNINGS
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#define MAX_CODE_PER_LINE 4096

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
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF) break;
    }
}

static void repl() {
    // ���û��������û�ÿ����һ�д��룬����һ�д��룬����������
    // repl��"read evaluate print loop"����д
    char* user_input = (char)calloc(1, MAX_CODE_PER_LINE);
    while (gets(user_input))
    {
        run(user_input);
    }
}

static char* readFile(const char* path) {
    // �û������ļ������������ļ������ݶ����ڴ棬����ĩβ���'\0'
    // ע��: ����Ӧ��ʹ�ö�̬�ڴ���䣬���Ӧ������ȷ���ļ��Ĵ�С��
    //���ļ�
    FILE* file_stream = fopen(path, "r");
    if (!file_stream) {
        fprintf(stderr, "Error: %s open failed.\n", path);
        exit(1);
    }
    //���ļ���С
    fseek(file_stream, 0, SEEK_END);
    long int bytes = ftell(file_stream);
    if (bytes == -1) {
        //�ļ����ܲ����ڻ�������Ϊ��
        fclose(file_stream);
        fprintf(stderr, "Error: ftell failed.\n");
        exit(1);
    }
    //�����ڴ�
    char* buffer = (char*)calloc(bytes + 1, sizeof(char));
    if (!buffer) {
        fprintf(stderr, "Error: calloc failed.\n");
        exit(1);
    }
    //�����ļ�����ӿ��ַ�
    rewind(file_stream);
    size_t n = fread(buffer, 1, bytes, file_stream);
    if (n > bytes) {
        //���ļ���ȡ���󣬹ر��ļ����ͷŷ�����ڴ�
        fclose(file_stream);
        free(buffer);
        fprintf(stderr, "Error: fread failed.\n");
        exit(1);
    }
    buffer[bytes] = '\0';
    //�ر��ļ�
    fclose(file_stream);
    return buffer;
}


static void runFile(const char* path) {
    // ����'.c'�ļ�:�û������ļ��������������ļ�������������
    char* source  = readFile(path);
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
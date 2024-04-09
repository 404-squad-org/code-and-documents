// main.c
#define _CRT_SECURE_NO_WARNINGS
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#define MAX_CODE_PER_LINE 4096

static void run(const char* source) {
    initScanner(source);
    int line = -1;
    // 打印Token, 遇到TOKEN_EOF为止
    for (;;) {
        // 解析下一个Token
        Token token = scanToken();
        // 打印这个Token的信息
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
    // 与用户交互，用户每输入一行代码，分析一行代码，并将结果输出
    // repl是"read evaluate print loop"的缩写
    char* user_input = (char)calloc(1, MAX_CODE_PER_LINE);
    while (gets(user_input))
    {
        run(user_input);
    }
}

static char* readFile(const char* path) {
    // 用户输入文件名，将整个文件的内容读入内存，并在末尾添加'\0'
    // 注意: 这里应该使用动态内存分配，因此应该事先确定文件的大小。
    //打开文件
    FILE* file_stream = fopen(path, "r");
    if (!file_stream) {
        fprintf(stderr, "Error: %s open failed.\n", path);
        exit(1);
    }
    //得文件大小
    fseek(file_stream, 0, SEEK_END);
    long int bytes = ftell(file_stream);
    if (bytes == -1) {
        //文件可能不存在或者内容为空
        fclose(file_stream);
        fprintf(stderr, "Error: ftell failed.\n");
        exit(1);
    }
    //申请内存
    char* buffer = (char*)calloc(bytes + 1, sizeof(char));
    if (!buffer) {
        fprintf(stderr, "Error: calloc failed.\n");
        exit(1);
    }
    //读入文件并添加空字符
    rewind(file_stream);
    size_t n = fread(buffer, 1, bytes, file_stream);
    if (n > bytes) {
        //若文件读取错误，关闭文件，释放分配的内存
        fclose(file_stream);
        free(buffer);
        fprintf(stderr, "Error: fread failed.\n");
        exit(1);
    }
    buffer[bytes] = '\0';
    //关闭文件
    fclose(file_stream);
    return buffer;
}


static void runFile(const char* path) {
    // 处理'.c'文件:用户输入文件名，分析整个文件，并将结果输出
    char* source  = readFile(path);
    run(source);
    free(source);
}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        // ./scanner 没有参数,则进入交互式界面
        repl();
    }
    else if (argc == 2) {
        // ./scanner file 后面跟一个参数,则分析整个文件
        runFile(argv[1]);
    }
    else {
        fprintf(stderr, "Usage: scanner [path]\n");
        exit(1);
    }

    return 0;
}
#define _CRT_SECURE_NO_WARNINGS

#include "scanner.h"
#include <stdio.h>
#include<stdlib.h>

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
            printf("   |");
        }
        printf("% 2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF) break;
    }
}

static void repl() {
    // 与用户交互，用户每输入一行代码，分析一行代码，并将结果输出
    // repl是"read evaluate print loop"的缩写
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
    // 用户输入文件名，将整个文件的内容读入内存，并在末尾添加'\0'
    // 注意: 这里应该使用动态内存分配，因此应该事先确定文件的大小。
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
    //到这里说明已经申请好，下面开始读取数据吧
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
    // 处理'.c'文件:用户输入文件名，分析整个文件，并将结果输出
    char* source = readFile(path);
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

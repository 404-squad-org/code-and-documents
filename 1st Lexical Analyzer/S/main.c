// main.c
#include "Scanner.h"  // 包含自定义的Scanner头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void run(const char* source) {
    initScanner(source);  // 初始化词法分析器
    int line = -1;
    // 打印Token, 遇到TOKEN_EOF为止
    for (;;) {
        Token token = scanToken();  // 扫描当前Token
        if (token.line != line) {
            printf("%4d ", token.line);  // 打印行号
            line = token.line;
        }
        else {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);  // 打印Token信息

        if (token.type == TOKEN_EOF) break;  // 遇到EOF退出循环
    }
}
static void repl() {
    char line[256];
    int exit_flag = 0;

    printf("欢迎进入交互模式，请输入代码（输入exit退出, 输入\\n换行）：\n");

    while (!exit_flag) {
        printf("> ");

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        if (strcmp(line, "\\n\n") == 0) {
            printf("\n");  // 输入\\n回车换行
            continue;
        }

        // 识别无内容直接输入回车的情况并跳过
        if (strcmp(line, "\n") == 0) {
            continue;
        }

        if (strcmp(line, "exit\n") == 0) {
            break;
        }

        run(line);  // 运行输入的代码
    }
}

static char* readFile(const char* path) {
    // 用户输入文件名，将整个文件的内容读入内存，并在末尾添加'\0'
    // 注意: 这里应该使用动态内存分配，因此应该事先确定文件的大小。
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "错误：无法打开文件\n");
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    rewind(fp);

    char* content = malloc(size + 1);
    if (!content) {
        fprintf(stderr, "错误：内存分配失败\n");
        exit(1);
    }

    content[fread(content, 1, size, fp)] = '\0';  // 读取文件内容

    return content;
}

static void runFile(const char* path) {
    // 处理'.c'文件:用户输入文件名，分析整个文件，并将结果输出
    char* content = readFile(path);
    run(content);  // 运行文件内容的词法分析
    free(content);  // 释放内存
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
        fprintf(stderr, "用法: scanner [路径]\n");
        exit(1);
    }

    return 0;
}


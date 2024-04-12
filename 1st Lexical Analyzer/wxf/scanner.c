// scanner.c
#include "scanner.h"
#include <stdbool.h>
#include <string.h>


typedef struct {
    const char* start;
    const char* current;    // 下一个要读的字符
    int line;
} Scanner;

// 全局变量
Scanner scanner;

void initScanner(const char* source) {
    // 初始化scanner
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

/***************************************************************************************
 *                                   辅助方法											*
 ***************************************************************************************/
static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isAtEnd() {
    return *scanner.current == '\0';
}

static char advance() {
    return *scanner.current++;
}

static char peek() {
    return *scanner.current;
}

static char peekNext() {
    if (isAtEnd()) return '\0';
    return *(scanner.current + 1);
}

// 有条件的移动current指针
static bool match(char expected) {
     if (isAtEnd()) return false;
    if (peek() != expected) return false;
    scanner.current++;
    return true;
}

// 传入TokenType, 创建对应类型的Token，并返回。
static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

// 遇到不能解析的情况时，我们创建一个ERROR Token. 比如：遇到@，$等符号时，比如字符串，字符没有对应的右引号时。
static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

static void skipWhitespace() {
    // 跳过空白字符: ' ', '\r', '\t', '\n'和注释
    // 注释以'//'开头, 一直到行尾
    // 注意更新scanner.line！
    while (1) {
        if (peek() == ' ' ||
            peek() == '\r' ||
            peek() == '\t') {
            advance();
        }
        if (peek() == '\n') {
            advance();
            scanner.line++;
        }
        if (peek() == '/' && peekNext() == '/') {
            while (peek() != '\n') {
                advance();
            }
            scanner.line++;
        }
        if (peek() != ' ' || peek() != '\r' || peek() != '\t' || peek() != '\n' ) {
            break;
        }
    }
}

// 参数说明：
// start: 从scanner.start位置往后偏移start开始比较
// length: 要比较字符的长度
// rest: 要比较的内容
// type: 如果完全匹配，则说明是type类型的关键字
static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    // Token的长度
    int len = (int)(scanner.current - scanner.start);
    // start + length: 关键字的长度
    if (start + length == len && memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

// 判断当前Token到底是标识符还是关键字
static TokenType identifierType() {
    // 确定identifier类型主要有两种方式：
    // 1. 将所有的关键字放入哈希表中，然后查表确认
    // 2. 将所有的关键字放入Trie树中，然后查表确认
    // Trie树的方式不管是空间上还是时间上都优于哈希表的方式

    char c = scanner.start[0];
    // 用switch语句实现Trie树
    switch (c) {
    case 'b': return checkKeyword(1, 4, "reak", TOKEN_BREAK);
    case 'c': {
        int len = (int)(scanner.current - scanner.start);
        if (len > 1) {
            switch (scanner.start[1])
            {
            case'a':return checkKeyword(2, 2, "se", TOKEN_CASE);
            case'h':return checkKeyword(2, 2, "ar", TOKEN_CHAR);
            case'o':
                if (len > 2) {
                    switch (scanner.start[3])
                    {
                        //这里的带三位scanner.start[2]都是n，没办法区分，
                        //所以用第四位scanner.start[3]
                    case's':return checkKeyword(2, 3, "nst", TOKEN_CONST);
                    case't':return checkKeyword(2, 6, "ntinue", TOKEN_CONTINUE);
                    }
                }
            }
        }
    }break;
    case 'd': {
        int len = (int)(scanner.current - scanner.start);
        if (len > 1) {
            switch (scanner.start[1])
            {
            case'e':return checkKeyword(2, 5, "fault", TOKEN_DEFAULT);
            case'o':
                if (len == 2) {
                    return TOKEN_DO;
                }
                if (len > 2) {
                    switch (scanner.start[2])
                    {
                    case'u':return checkKeyword(3, 3, "ble", TOKEN_DOUBLE);
                    }
                }


            }
        }
        break;
    }
    case 'f': {
        int len = (int)(scanner.current - scanner.start);
        if (len > 1) {
            switch (scanner.start[1])
            {
            case'l':return checkKeyword(2, 3, "oat", TOKEN_FLOAT);
            case'o':return checkKeyword(2, 1, "r", TOKEN_FOR);
            }
        }
        break;
    }
    case 'g': {
        return checkKeyword(1, 3, "oto", TOKEN_GOTO);
    }

    case 'i': {
        int len = (int)(scanner.current - scanner.start);
        if (len > 1) {
            switch (scanner.start[1])
            {
            case'f':return TOKEN_IF;
            case'n':return checkKeyword(2, 1, "t", TOKEN_INT);
            }
        }
        break;
    }
    case 'l': {
        return checkKeyword(1, 3, "ong", TOKEN_LONG);
    }
    case 'r': {
        return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    }


    case 's': {
        int len = (int)(scanner.current - scanner.start);
        if (len > 1) {
            switch (scanner.start[1])
            {
            case'h':return checkKeyword(2, 3, "ort", TOKEN_SHORT);
            case'i':
                if (len > 2) {
                    switch (scanner.start[2])
                    {
                    case'g':return checkKeyword(3, 3, "ned", TOKEN_SIGNED);
                    case'z':return checkKeyword(3, 3, "eof", TOKEN_SIZEOF);
                    }

                }
                break;
            case't':return checkKeyword(2, 4, "ruct", TOKEN_STRUCT);
            case'w':return checkKeyword(2, 4, "itch", TOKEN_SWITCH);
            }
        }
        break;
    }
    case 't':return checkKeyword(1, 6, "ypedef", TOKEN_TYPEDEF);

    case 'u': {
        int len = (int)(scanner.current - scanner.start);
        if (len > 2) {
            switch (scanner.start[2])
            {
            case'i':return checkKeyword(1, 4, "nion", TOKEN_UNION);
            case's':return checkKeyword(1, 7, "nsigned", TOKEN_UNSIGNED);
            }
        }
        break;
    }
    case 'v':return checkKeyword(1, 3, "oid", TOKEN_VOID);
    case 'w':return checkKeyword(1, 4, "hile", TOKEN_WHILE);

        // identifier
        return TOKEN_IDENTIFIER;
        //识别关键字
    }
}

//判断为标识符。
static Token identifier() {
 //IDENTFIER包含：字母，数字和下划线
  while (isAlpha(peek()) || isDigit(peek())) {
       advance();
    }
  // 这样的Token可能是标识符, 也可能是关键字, 
  //identifierType()是用来确定Token类型的
    return makeToken(identifierType());
}

static Token number() {
    // 简单起见，我们将NUMBER的规则定义如下:
    // 1. NUMBER可以包含数字和最多一个'.'号
    // 2. '.'号前面要有数字
    // 3. '.'号后面也要有数字
    // 这些都是合法的NUMBER: 123, 3.14
    // 这些都是不合法的NUMBER: 123., .14
    while (isDigit(peek())) {
        advance();
    }
    //如果发现小数点并且下一个是数字
    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        //只有满足12.333这种情况才继续往后移动
        while (isDigit(peek())) {
            advance();
        }
    }
    return makeToken(TOKEN_NUMBER);

}

static Token string() {
    // 字符串以"开头，以"结尾，而且不能跨行
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') {
            return errorToken("no support multi line");
        }
        advance();
    }//出来就是到达结尾或者等于 "
    if (isAtEnd()) {
        return errorToken("arrive end ");//字符串结束没有  "   错误
    }
    //往后走一步，current指向空白字符
    advance();
    return makeToken(TOKEN_STRING);
}

static Token character() {
    // 字符'开头，以'结尾，而且不能跨行  
    while (!isAtEnd() && peek() != '\'') {
        if (peek() == '\n') {
            return errorToken("no support multi line");
        }
        advance;
    }//出来就是到达结尾或者等于 '
    if (isAtEnd()) {
        return errorToken("arrive end ");//字符串结束没有  '   错误
    }
    //往后走一步，current指向空白字符
    advance();
    return makeToken(TOKEN_CHARACTER);
}

/***************************************************************************************
 *                                   	分词											  *
 ***************************************************************************************/
Token scanToken() {
    // 跳过前置空白字符和注释
    skipWhitespace();
    // 记录下一个Token的起始位置
    scanner.start = scanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    // 获取Token的第一个字符
    char c = advance();
    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
        // single-character tokens
    case '(': return makeToken(TOKEN_LEFT_PAREN);
    case ')': return makeToken(TOKEN_RIGHT_PAREN);
    case '[': return makeToken(TOKEN_LEFT_BRACKET);
    case ']': return makeToken(TOKEN_RIGHT_BRACKET);
    case '{': return makeToken(TOKEN_LEFT_BRACE);
    case '}': return makeToken(TOKEN_RIGHT_BRACE);
    case ',': return makeToken(TOKEN_COMMA);
    case '.': return makeToken(TOKEN_DOT);
    case ';': return makeToken(TOKEN_SEMICOLON);
    case '~': return makeToken(TOKEN_TILDE);
        // one or two characters tokens
    case '+':
        if (match('+')) return makeToken(TOKEN_PLUS_PLUS);
        else if (match('=')) return makeToken(TOKEN_PLUS_EQUAL);
        else return makeToken(TOKEN_PLUS);
    case '-':
        if (match('-')) return makeToken(TOKEN_MINUS_MINUS);
        else if (match('=')) return makeToken(TOKEN_MINUS_EQUAL);
        else if (match('>'))return makeToken(TOKEN_MINUS_GREATER);
        else return makeToken(TOKEN_MINUS);
    case '*':
        if (match('=')) return makeToken(TOKEN_STAR_EQUAL);
        else return makeToken(TOKEN_STAR);
    case '/':
        if (match('=')) return makeToken(TOKEN_SLASH_EQUAL);
        else return makeToken(TOKEN_SLASH);
    case '%':
        if (match('=')) return makeToken(TOKEN_PERCENT_EQUAL);
        else return makeToken(TOKEN_PERCENT);
    case '&':
        if (match('=')) return makeToken(TOKEN_AMPER_EQUAL);
        else if (match('&')) return makeToken(TOKEN_AMPER_AMPER);
        else return makeToken(TOKEN_AMPER);
    case '|':
        if (match('=')) return makeToken(TOKEN_PIPE_EQUAL);
        else if (match('|')) return makeToken(TOKEN_PIPE_PIPE);
        else return makeToken(TOKEN_PIPE);
    case '^':
        if (match('=')) return makeToken(TOKEN_HAT_EQUAL);
        else return makeToken(TOKEN_HAT);
    case '=':
        if (match('=')) return makeToken(TOKEN_EQUAL_EQUAL);
        else return makeToken(TOKEN_EQUAL);
    case '!':
        if (match('=')) return makeToken(TOKEN_BANG_EQUAL);
        else return makeToken(TOKEN_BANG);
    case '<':
        if (match('=')) return makeToken(TOKEN_LESS_EQUAL);
        else if (match('<')) return makeToken(TOKEN_LESS_LESS);
        else return makeToken(TOKEN_LESS);
    case '>':
        if (match('=')) return makeToken(TOKEN_GREATER_EQUAL);
        else if (match('<')) return makeToken(TOKEN_GREATER_GREATER);
        else return makeToken(TOKEN_GREATER);
       

        // three-characters tokens （附加题）
        // <, <=, <<, <<=
        // >, >=, >>, >>=

        // various-character tokens
    case '"': return string();
    case '\'': return character();
    }

    return errorToken("Unexpected character.");
}

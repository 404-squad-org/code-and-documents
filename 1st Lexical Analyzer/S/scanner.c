// scanner.c
#include "Scanner.h"

typedef struct {
    const char* start;      // 指向Token的起始位置
    const char* current;    // 下一个字符的位置
    int line;               // 源代码中哪一行
} Scanner;

// 全局变量
Scanner scanner;

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}
/***********************************************************************
****************
*                                   辅助方法
*
***********************************************************************
****************/
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
    if (token.type == 73) token.line++;
    return token;
}

// 遇到不能解析的情况时，我们创建一个ERROR Token. 比如：遇到@，$等符号时，比如字
//符串，字符没有对应的右引号时。
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
    //start:
    for (;;) {
        if (peek() == ' ' || peek() == '\n' || peek() == '\r' || peek() == '\t' || peek() == '/' && peekNext() == '/') {
            if (peek() == '\n' || peek() == '/' && peekNext() == '/') scanner.line++;
            scanner.current++;
        }
        else
            break;
    }
    //if (peek()== 47 || peekNext() == 47) {
    //    scanner.line++;
    //    while (advance() != '\n')
    //        ;
    //    goto start;
    //}
}

    

static TokenType checkKeyword(int start, int length, const char* rest,TokenType type) {
    int len = (int)(scanner.current - scanner.start);
    if (start + length == len && memcmp(scanner.start + start, rest,length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
    char c = scanner.start[0];
    // 用switch语句实现Trie树
    switch (c) {
    case 'b': return checkKeyword(1, 4, "reak", TOKEN_BREAK); break;
    case 'c': if (scanner.start[1] == 'a') return checkKeyword(2, 2, "se", TOKEN_CASE);
            else if (scanner.start[1] == 'h') return checkKeyword(2, 2, "ar", TOKEN_CHAR);
            else if (scanner.start[1] == 'o' && scanner.start[3] == 't') return checkKeyword(2, 6, "ntinue", TOKEN_CONTINUE);
            else if (scanner.start[1] == 'o' && scanner.start[3] == 's') return checkKeyword(2, 3, "nst", TOKEN_CONST);
            else return TOKEN_IDENTIFIER; break;
    case 'd': if (scanner.start[1] == 'e') return checkKeyword(2, 5, "fault", TOKEN_DEFAULT);
            else if (scanner.start[1] == 'o') return checkKeyword(2, 4, "uble", TOKEN_DOUBLE);
            else return TOKEN_IDENTIFIER; break;
    case 'e': if (scanner.start[1] == 'n') return checkKeyword(2, 2, "um", TOKEN_ENUM);
            else if (scanner.start[1] == 'l') return checkKeyword(2, 2, "se", TOKEN_ELSE);
            else return TOKEN_IDENTIFIER; break;
    case 'f': if (scanner.start[1] == 'l') return checkKeyword(2, 3, "oat", TOKEN_FLOAT);
            else if (scanner.start[1] == 'o') return checkKeyword(2, 1, "r", TOKEN_FOR);
            else return TOKEN_IDENTIFIER; break;
    case 'g': return checkKeyword(1, 3, "oto", TOKEN_GOTO);
    case 'i': if (scanner.start[1] == 'n') return checkKeyword(2, 1, "t", TOKEN_INT);
            else if (scanner.start[1] == 'f' && scanner.current - scanner.start == 2) return TOKEN_IF;
            else return TOKEN_IDENTIFIER;
    case 'l': return checkKeyword(1, 3, "ong", TOKEN_LONG);
    case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 's': if (scanner.start[1] == 'h') return checkKeyword(2, 3, "ort", TOKEN_SHORT);
            else if (scanner.start[1] == 'i' && scanner.start[2] == 'g') return checkKeyword(3, 3, "ned", TOKEN_SIGNED);
            else if (scanner.start[1] == 'i' && scanner.start[2] == 'z') return checkKeyword(3, 3, "eof", TOKEN_SIZEOF);
            else if (scanner.start[1] == 't') return checkKeyword(2, 4, "ruct", TOKEN_STRUCT);
            else if (scanner.start[1] == 'w') return checkKeyword(2, 4, "itch", TOKEN_SWITCH);
            else return TOKEN_IDENTIFIER;
    case 't': return checkKeyword(1, 6, "ypedef", TOKEN_TYPEDEF);
    case 'u': if (scanner.start[2] == 's') return checkKeyword(1, 7, "nsigned", TOKEN_UNSIGNED);
            else return checkKeyword(1, 4, "nion", TOKEN_UNION);
    case 'v': return checkKeyword(1, 3, "oid", TOKEN_VOID);
    case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE); break;
    }
    return TOKEN_IDENTIFIER;
}

static Token identifier() {
    // IDENTIFIER包含: 字母，数字和下划线
    while (isAlpha(peek()) || isDigit(peek())||peek()==95) {
        advance();
    }
    // 这样的Token可能是标识符, 也可能是关键字, identifierType()是用来确定Token类型的
        return makeToken(identifierType());
}

static Token number() {
    // 简单起见，我们将NUMBER的规则定义如下:
    // 1. NUMBER可以包含数字和最多一个'.'号
    // 2. '.'号前面要有数字
    // 3. '.'号后面也要有数字
    // 这些都是合法的NUMBER: 123, 3.14
    // 这些都是不合法的NUMBER: 123., .14
    int Decimal_point = 0;
    while (isDigit(peek())||peek()=='.') {
        if (peek() == '.') Decimal_point++;
        advance();
    }

    if (Decimal_point <= 1) return makeToken(TOKEN_NUMBER);
    else return makeToken(TOKEN_ERROR);
}

static Token string() {
    // 字符串以"开头，以"结尾，而且不能跨行
    do {
        advance();
    } while (peek() != '"' && peek() != '\n');
    advance();
    return makeToken(TOKEN_STRING);
}

static Token character() {
    // 字符'开头，以'结尾，而且不能跨行
    do {
        advance();
    } while (peek() != 39 && peek() != '\n');
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
        else if (match('>')) return makeToken(TOKEN_MINUS_GREATER);
        else   return makeToken(TOKEN_MINUS);
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
        else makeToken(TOKEN_AMPER);
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
        else if (match('>')) return makeToken(TOKEN_GREAER_GREATER);
        else return makeToken(TOKEN_GREATER);
        // various-character tokens
    case '"': return string();
    case 39: return character();
    }

    return errorToken("Unexpected character.");
}

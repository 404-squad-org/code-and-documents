// scanner.c
#include "scanner.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    const char* start;
    const char* current;    // ��һ��Ҫ�����ַ�
    int line;
} Scanner;

// ȫ�ֱ���
Scanner scanner;

void initScanner(const char* source) {
    // ��ʼ��scanner
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

/***************************************************************************************
 *                                   ��������											*
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

// ���������ƶ�currentָ��
static bool match(char expected) {
    if (isAtEnd()) return false;
    if (peek() != expected) return false;
    scanner.current++;
    return true;
}

// ����TokenType, ������Ӧ���͵�Token�������ء�
static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

// �������ܽ��������ʱ�����Ǵ���һ��ERROR Token. ���磺����@��$�ȷ���ʱ�������ַ������ַ�û�ж�Ӧ��������ʱ��
static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

static void skipWhitespace() {
    // �����հ��ַ�: ' ', '\r', '\t', '\n'��ע��
    // ע����'//'��ͷ, һֱ����β
    // ע�����scanner.line��
    if (peek(scanner) == '\n') {
        scanner.line++;
        scanner.current++;
    }else if (peek(scanner) == ' ' || peek(scanner) == '\t' || peek(scanner) == '\r') {
        scanner.current++;
    }
    if (peek(scanner) == '/') {
        if (advance(peek(scanner)) == '/') {
            while (peek(scanner) != '\n' && (peek(scanner) != '\0')) {
                scanner.current++;
            }
            scanner.current++;
            scanner.line++;
        }
        else if (advance(scanner) == '*') {
            do scanner.current++; while (peek(scanner) == '*' && advance(scanner) == '/');
            scanner.current++;
        }
    }

}

// ����˵����
// start: ���ĸ�����λ�ÿ�ʼ�Ƚ�
// length: Ҫ�Ƚ��ַ��ĳ���
// rest: Ҫ�Ƚϵ�����
// type: �����ȫƥ�䣬��˵����type���͵Ĺؼ���
static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    // Token�ĳ���
    int len = (int)(scanner.current - scanner.start);
    // start + length: �ؼ��ֵĳ���
    if (start + length == len && memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

// �жϵ�ǰToken�����Ǳ�ʶ�����ǹؼ���
static TokenType identifierType() {
    // ȷ��identifier������Ҫ�����ַ�ʽ��
    // 1. �����еĹؼ��ַ����ϣ���У�Ȼ����ȷ��
    // 2. �����еĹؼ��ַ���Trie���У�Ȼ����ȷ��
    // Trie���ķ�ʽ�����ǿռ��ϻ���ʱ���϶����ڹ�ϣ��ķ�ʽ

    char c = scanner.start[0];
    // ��switch���ʵ��Trie��
    switch (c) {
    case 'b': return checkKeyword(1, 4, "reak", TOKEN_BREAK);
    case 'c':
        if (scanner.start[1] == 'a') return checkKeyword(2, 2, "se", TOKEN_CASE);
        else if (scanner.start[1] == 'h') return checkKeyword(2, 2, "ar", TOKEN_CHAR);
        else if (scanner.start[1] == 'o' && scanner.start[2] == 'n')
        {
            if (scanner.start[3] == 't')
            {
                return checkKeyword(4, 4, "inue", TOKEN_CONTINUE);
            }
            else if (scanner.start[3] == 's')
            {
                return checkKeyword(4, 1, "t", TOKEN_CONST);
            }
            else {
                return TOKEN_IDENTIFIER;
            }
        }
        else return TOKEN_IDENTIFIER;
    case 'd':
        if (scanner.start[1] == 'e') return checkKeyword(2, 5, "fault", TOKEN_DEFAULT);
        else if (scanner.start[1] == 'o') return checkKeyword(2, 4, "uble", TOKEN_DOUBLE);
        else return TOKEN_IDENTIFIER;
    case 'e':
        if (scanner.start[1] == 'n') return checkKeyword(2, 2, "um", TOKEN_ENUM);
        else if (scanner.start[1] == 'l') return checkKeyword(2, 2, "se", TOKEN_ELSE);
        else return TOKEN_IDENTIFIER;
    case 'f':
        if (scanner.start[1] == 'l') return checkKeyword(2, 3, "oat", TOKEN_FLOAT);
        else if (scanner.start[1] == 'o') return checkKeyword(2, 1, "r", TOKEN_FOR);
        else return TOKEN_IDENTIFIER;
    case 'g':return checkKeyword(1, 3, "oto", TOKEN_GOTO);
    case 'i':
        if (scanner.start[1] == 'f') return TOKEN_IF;
        else if (scanner.start[1] == 'n') return checkKeyword(2, 1, "t", TOKEN_INT);
        else return TOKEN_IDENTIFIER;
    case 'l':return checkKeyword(1, 3, "ong", TOKEN_LONG);
    case 'r':return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 's':
        if (scanner.start[1] == 'h') return checkKeyword(2, 3, "ort", TOKEN_SHORT);
        else if (scanner.start[1] == 'i')
        {
            if (scanner.start[2] == 'g')
            {
                return checkKeyword(3, 3, "ned", TOKEN_SIGNED);
            }
            else if (scanner.start[2] == 'z')
            {
                return checkKeyword(3, 3, "eof", TOKEN_SIZEOF);
            }
            else {
                return TOKEN_IDENTIFIER;
            }
        }
        else if (scanner.start[1] == 't')return checkKeyword(2, 4, "ruct", TOKEN_STRUCT);
        else if (scanner.start[1] == 'w')return checkKeyword(2, 4, "itch", TOKEN_SWITCH);
        else return TOKEN_IDENTIFIER;
    case 't':return checkKeyword(1, 6, "ypedef", TOKEN_TYPEDEF);
    case'u':
        if (scanner.start[1] == 'n')
        {
            if (scanner.start[2] == 's')
            {
                return checkKeyword(3, 5, "igned", TOKEN_UNSIGNED);
            }
            else if (scanner.start[2] == 'i')
            {
                return checkKeyword(3, 2, "on", TOKEN_UNION);
            }
            else {
                return TOKEN_IDENTIFIER;
            }
        }
        else {
            return TOKEN_IDENTIFIER;
        }
    case 'v':return checkKeyword(1, 3, "oid", TOKEN_VOID);
    case 'w':return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }
    // identifier
    return TOKEN_IDENTIFIER;
}

static Token identifier() {
    // IDENTIFIER����: ��ĸ�����ֺ��»���
    while (isAlpha(peek()) || isDigit(peek())) {
        advance();
    }
    // ������Token�����Ǳ�ʶ��, Ҳ�����ǹؼ���, identifierType()������ȷ��Token���͵�
    return makeToken(identifierType());
}

static Token number() {
    // ����������ǽ�NUMBER�Ĺ���������:
    // 1. NUMBER���԰������ֺ����һ��'.'��
    // 2. '.'��ǰ��Ҫ������
    // 3. '.'�ź���ҲҪ������
    // ��Щ���ǺϷ���NUMBER: 123, 3.14
    // ��Щ���ǲ��Ϸ���NUMBER: 123., .14
    while (isDigit(*scanner.current))
    {
        scanner.current++;
    }
    if (*scanner.current == '.')
    {
        scanner.current++;
        if (isDigit(*scanner.current))
        {
            while (isDigit(*scanner.current))
            {
                scanner.current++;
            }
        }
        else {
            return makeToken(TOKEN_ERROR);
        }
        return makeToken(TOKEN_NUMBER);
    }
    return makeToken(TOKEN_NUMBER);
}


static Token string() {
    // �ַ�����"��ͷ����"��β�����Ҳ��ܿ���
    while (*scanner.current != '"')
    {
        scanner.current++;
    }
    scanner.current++;
    return makeToken(TOKEN_STRING);
}


static Token character() {
    // �ַ�'��ͷ����'��β�����Ҳ��ܿ��� 
    while (*scanner.current != '\'')
    {
        scanner.current++;
    }
    scanner.current++;
    return makeToken(TOKEN_CHARACTER);
}

/***************************************************************************************
 *                                   	�ִ�											  *
 ***************************************************************************************/
Token scanToken() {
    // ����ǰ�ÿհ��ַ���ע��
    skipWhitespace();
    // ��¼��һ��Token����ʼλ��
    scanner.start = scanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    // ��ȡToken�ĵ�һ���ַ�
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
        // one or two characters tokenscase 
    case '+':
        if (match('+')) return makeToken(TOKEN_PLUS_PLUS);
        else if (match('=')) return makeToken(TOKEN_PLUS_EQUAL);
        else return makeToken(TOKEN_PLUS);
    case '-':
        if (match('-')) return makeToken(TOKEN_MINUS_MINUS);
        else if (match('=')) return makeToken(TOKEN_MINUS_EQUAL);
        else if (match('>')) return makeToken(TOKEN_MINUS_GREATER);
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
        if (match('&')) return makeToken(TOKEN_AMPER_AMPER);
        else if (match('=')) return makeToken(TOKEN_AMPER_EQUAL);
        else return makeToken(TOKEN_AMPER);
    case '|':
        if (match('|')) return makeToken(TOKEN_PIPE_PIPE);
        else if (match('=')) return makeToken(TOKEN_PIPE_EQUAL);
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
        if (match('<')) return makeToken(TOKEN_LESS_LESS);
        else if (match('=')) return makeToken(TOKEN_LESS_EQUAL);
        else return makeToken(TOKEN_LESS);
    case '>':
        if (match('>')) return makeToken(TOKEN_GREAER_GREATER);
        else if (match('=')) return makeToken(TOKEN_GREATER_EQUAL);
        else return makeToken(TOKEN_GREATER);
        // three-characters tokens �������⣩
        // <, <=, <<, <<=
        
        // >, >=, >>, >>=

        // various-character tokens
    case '"': return string();
    case '\'': return character();
    }
    skipWhitespace();
    return errorToken("Unexpected character.");
}
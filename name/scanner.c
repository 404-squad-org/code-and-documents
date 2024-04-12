// scanner.c
#include "scanner.h"
#include <stdbool.h>
#include <string.h>


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

// ����˵����
// start: ��scanner.startλ������ƫ��start��ʼ�Ƚ�
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
                        //����Ĵ���λscanner.start[2]����n��û�취���֣�
                        //�����õ���λscanner.start[3]
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
        //ʶ��ؼ���
    }
}

//�ж�Ϊ��ʶ����
static Token identifier() {
 //IDENTFIER��������ĸ�����ֺ��»���
  while (isAlpha(peek()) || isDigit(peek())) {
       advance();
    }
  // ������Token�����Ǳ�ʶ��, Ҳ�����ǹؼ���, 
  //identifierType()������ȷ��Token���͵�
    return makeToken(identifierType());
}

static Token number() {
    // ����������ǽ�NUMBER�Ĺ���������:
    // 1. NUMBER���԰������ֺ����һ��'.'��
    // 2. '.'��ǰ��Ҫ������
    // 3. '.'�ź���ҲҪ������
    // ��Щ���ǺϷ���NUMBER: 123, 3.14
    // ��Щ���ǲ��Ϸ���NUMBER: 123., .14
    while (isDigit(peek())) {
        advance();
    }
    //�������С���㲢����һ��������
    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        //ֻ������12.333��������ż��������ƶ�
        while (isDigit(peek())) {
            advance();
        }
    }
    return makeToken(TOKEN_NUMBER);

}

static Token string() {
    // �ַ�����"��ͷ����"��β�����Ҳ��ܿ���
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') {
            return errorToken("no support multi line");
        }
        advance();
    }//�������ǵ����β���ߵ��� "
    if (isAtEnd()) {
        return errorToken("arrive end ");//�ַ�������û��  "   ����
    }
    //������һ����currentָ��հ��ַ�
    advance();
    return makeToken(TOKEN_STRING);
}

static Token character() {
    // �ַ�'��ͷ����'��β�����Ҳ��ܿ���  
    while (!isAtEnd() && peek() != '\'') {
        if (peek() == '\n') {
            return errorToken("no support multi line");
        }
        advance;
    }//�������ǵ����β���ߵ��� '
    if (isAtEnd()) {
        return errorToken("arrive end ");//�ַ�������û��  '   ����
    }
    //������һ����currentָ��հ��ַ�
    advance();
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
       

        // three-characters tokens �������⣩
        // <, <=, <<, <<=
        // >, >=, >>, >>=

        // various-character tokens
    case '"': return string();
    case '\'': return character();
    }

    return errorToken("Unexpected character.");
}
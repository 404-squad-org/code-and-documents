# include "scanner.h"
#include <stdbool.h>
#include <string.h>

typedef struct {
	const char* start;
	const char* current;
	int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

static bool isSpace(char c) {
	return c == ' ' || c == '\f' || c == '\n' ||
		   c == '\r' || c == '\t' || c == '\v';
}

static bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') ||
		   (c >= 'A' && c <= 'Z') ||
		    c == '_';
}

static bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

static bool isAtEnd(void) {
	return *scanner.current == '\0';
}

static char advance(void) {
	return *scanner.current++;
}

static char peek(void) {
	return *scanner.current;
}

static char peekNext(void) {
	if (isAtEnd()) return '\0';
	return *(scanner.current + 1);
}

static bool match(char expected) {
	if (isAtEnd()) return false;
	if (peek() != expected) return false;
	scanner.current++;
	return true;
}

static Token makeToken(TokenType type) {
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.length = (int)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

static Token errorToken(const char* message) {
	Token token;
	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int)strlen(message);
	token.line = scanner.line;
	return token;
}

static void skipWhitespace(void) {
	for (;;) {
		char c = peek();
		if (isSpace(c)) {
			if (c == '\n') scanner.line++;
			advance();
		}
		else if (c == '/') {
			if (peekNext() == '/') {
				while (!isAtEnd() && peek() != '\n') advance();
			}
			else if (peekNext() == '*') {
				advance();
				advance();
				while (!isAtEnd()) {
					if (peek() == '*' && peekNext() == '/') {
						advance();
						advance();
						break;
					}
					advance();
				}
			}
			else return;
		}
		else return;
	}
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
	int len = (int)(scanner.current - scanner.start);
	if (start + length == len && memcmp(scanner.start + start, rest, length) == 0) {
		return type;
	}
	return TOKEN_IDENTIFIER;
}

static TokenType identifierType(void) {
	char c = scanner.start[0];
	switch (c) {
	case 'b': return checkKeyword(1, 4, "reak", TOKEN_BREAK);
	case 'c':
		c = scanner.start[1];
		switch (c) {
		case 'a': return checkKeyword(2, 2, "se", TOKEN_CASE);
		case 'h': return checkKeyword(2, 2, "ar", TOKEN_CHAR);
		case 'o':
			c = scanner.start[2];
			if (c == 'n') {
				c = scanner.start[3];
				switch (c) {
				case 't': return checkKeyword(4, 4, "inue", TOKEN_CONTINUE);
				case 's': return checkKeyword(4, 1, "t", TOKEN_CONST);
				}
			}
			break;
		}
	case 'd':
		c = scanner.start[1];
		switch (c) {
		case 'e': return checkKeyword(2, 5, "fault", TOKEN_DEFAULT);
		case 'o': return checkKeyword(2, 4, "uble", TOKEN_DOUBLE);
		}
		break;
	case 'e':
		c = scanner.start[1];
		switch (c) {
		case 'n': return checkKeyword(2, 2, "um", TOKEN_ENUM);
		case 'l': return checkKeyword(2, 2, "se", TOKEN_ELSE);
		}
		break;
	case 'f':
		c = scanner.start[1];
		switch (c) {
		case 'l': return checkKeyword(2, 3, "oat", TOKEN_FLOAT);
		case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
		}
		break;
	case 'g': return checkKeyword(1, 3, "oto", TOKEN_GOTO);
	case 'i':
		c = scanner.start[1];
		switch (c) {
		case 'f': return checkKeyword(2, 0, "", TOKEN_IF);
		case 'n': return checkKeyword(2, 1, "t", TOKEN_INT);
		}
	case 'l': return checkKeyword(1, 3, "ong", TOKEN_LONG);
	case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
	case 's':
		c = scanner.start[1];
		switch (c) {
		case 'h': return checkKeyword(2, 3, "ort", TOKEN_SHORT);
		case 'i':
			c = scanner.start[2];
			switch (c) {
			case 'g': return checkKeyword(3, 3, "ned", TOKEN_SIGNED);
			case 'z': return checkKeyword(3, 3, "eof", TOKEN_SIZEOF);
			}
			break;
		case 't': return checkKeyword(2, 4, "ruct", TOKEN_STRUCT);
		case 'w': return checkKeyword(2, 4, "itch", TOKEN_SWITCH);
		}
		break;
	case 't': return checkKeyword(1, 6, "ypedef", TOKEN_TYPEDEF);
	case 'u':
		c = scanner.start[1];
		if (c == 'n') {
			c = scanner.start[2];
			switch (c) {
			case 's': return checkKeyword(3, 5, "igned", TOKEN_UNSIGNED);
			case 'i': return checkKeyword(3, 2, "on", TOKEN_UNION);
			}
		}
		break;
	case 'v': return checkKeyword(1, 3, "oid", TOKEN_VOID);
	case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
	}
	return TOKEN_IDENTIFIER;
}

static Token identifier(void) {
	while (isAlpha(peek()) || isDigit(peek())) {
		advance();
	}
	return makeToken(identifierType());
}

static Token number(void) {
	while (isDigit(peek())) advance();
	if (peek() == '.' && isDigit(peekNext())) {
		advance();
		while (isDigit(peek())) advance;
	}
	/*bool haveDot = false;
	if (peek() == '.') {
		advance();
		haveDot = true;
	}
	bool haveFraction = false;
	while (isDigit(peek())) {
		advance();
		haveFraction = true;
	}
	if (haveDot && !haveFraction) {
		scanner.current--;
	}*/
	return makeToken(TOKEN_NUMBER);
}

static Token string(void) {
	while (!isAtEnd() && peek() != '"') {
		if (peek() == '\n') {
			scanner.current = scanner.start + 1;
			return errorToken("Without string after '\"'.");
		}
		advance();
	}
	if (isAtEnd()) {
		scanner.current = scanner.start + 1;
		return errorToken("Without string after '\"'.");
	}
	advance();
	return makeToken(TOKEN_STRING);
}

static Token character(void) {
	if (!isAtEnd() && peek() != '\'') {
		if (peekNext() != '\'') {
			return errorToken("Without character after '\''.");
		}
		advance();
	}
	if (isAtEnd()) return errorToken("Without character after '\''.");
	advance();
	return makeToken(TOKEN_CHARACTER);
}

Token scanToken(void) {
	skipWhitespace();
	scanner.start = scanner.current;
	if (isAtEnd()) return makeToken(TOKEN_EOF);
	char c = advance();
	if (isAlpha(c)) return identifier();
	if (isDigit(c)) return number();
	switch (c) {
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
	case '#': return makeToken(TOKEN_POUND);
	case '\\': return makeToken(TOKEN_BACKSLASH);
	case '?': return makeToken(TOKEN_QUESTION);
	case ':': return makeToken(TOKEN_COLON);


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
		else if (match('<')) {
			if (match('=')) return makeToken(TOKEN_LESS_LESS_EQUAL);
			else return makeToken(TOKEN_LESS_LESS);
		}
		else return makeToken(TOKEN_LESS);
	case '>':
		if (match('=')) return makeToken(TOKEN_GREATER_EQUAL);
		else if (match('>')) {
			if (match('=')) return makeToken(TOKEN_GREAER_GREATER_EQUAL);
			else return makeToken(TOKEN_GREAER_GREATER);
		}
		else return makeToken(TOKEN_GREATER);
	

	case '"': return string();
	case '\'': return character();
	}
	return errorToken("Unexpected character.");
}
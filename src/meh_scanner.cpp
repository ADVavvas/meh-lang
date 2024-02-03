#include "meh_scanner.hpp"
#include "meh.hpp"
#include "meh_error.hpp"
#include "meh_token.hpp"
#include <_ctype.h>
#include <optional>
#include <string>

Scanner::Scanner(const std::string &source) : source(source) {}

const std::vector<Token> Scanner::scanTokens() {
  while (!isAtEnd()) {
    // We are at the beginning of the next lexeme.
    start = current;
    scanToken();
  }
  addToken(TokenType::FILE_END);
  return tokens;
}

void Scanner::scanToken() {
  char c = source[current++];
  switch (c) {
  case ';':
    addToken(TokenType::SEMICOLON);
    break;
  case '(':
    addToken(TokenType::PAREN_LEFT);
    break;
  case ')':
    addToken(TokenType::PAREN_RIGHT);
    break;
  case '{':
    addToken(TokenType::BRACE_LEFT);
    break;
  case '}':
    addToken(TokenType::BRACE_RIGHT);
    break;
  case ',':
    addToken(TokenType::COMMA);
    break;
  case '.':
    addToken(TokenType::DOT);
    break;
  case '-':
    addToken(TokenType::MINUS);
    break;
  case '+':
    addToken(TokenType::PLUS);
    break;
  case '*':
    addToken(TokenType::STAR);
    break;
  case '!':
    matches('=') ? addToken(TokenType::NOT_EQUAL) : addToken(TokenType::NOT);
    break;
  case '=':
    matches('=') ? addToken(TokenType::EQUAL_EQUAL)
                 : addToken(TokenType::EQUAL);
    break;
  case '<':
    matches('=') ? addToken(TokenType::LESS_EQUAL) : addToken(TokenType::LESS);
    break;
  case '>':
    matches('=') ? addToken(TokenType::GREATER_EQUAL)
                 : addToken(TokenType::GREATER);
    break;
  case '/':
    if (matches('/')) {
      while (peek() != '\n' && !isAtEnd()) {
        current++;
      }
    } else if (matches('*')) {
      //   while (!isAtEnd() && source[current - 1] != '*' &&
      //          source[current] != '/') {
      //     current++;
      //   }
    } else {
      addToken(TokenType::SLASH);
    }
    break;
  case '"':
    handleString();
    break;
  case ' ':
  case '\r':
  case '\t':
    // Whitespace.
    break;
  case '\n':
    line++;
    break;
  default:
    if (isdigit(c)) {
      handleNumber();
    } else if (isalpha(c)) {
      handleIdentifier();
    } else {
      Meh::error(Error(0, "Unexpected character."));
    }
    break;
  }
}

const void Scanner::addToken(const TokenType type) {
  addToken(type, std::nullopt);
}

const void Scanner::addToken(const TokenType type,
                             const std::optional<literal_t> literal) {
  std::string text = source.substr(start, current - start);
  tokens.push_back(Token(type, text, 0, literal));
}

const bool Scanner::matches(const char expected) {
  if (isAtEnd())
    return false;
  if (source[current] != expected)
    return false;
  current++;
  return true;
}

const char Scanner::peek(int n) const {
  if (current + n >= source.length()) {
    return '\0';
  }
  return source[current + n];
}

const void Scanner::handleString() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') {
      line++;
    }
    current++;
  }

  if (isAtEnd()) {
    Meh::error(Error(line, "Unterminated string."));
    return;
  }

  // The closing ".
  current++;

  std::string value = source.substr(start + 1, current - start - 1);
  addToken(TokenType::STRING);
}

const void Scanner::handleNumber() {
  while (isdigit(peek())) {
    current++;
  }

  if (peek() == '.' && isdigit(peek(1))) {
    current++;

    while (isdigit(peek())) {
      current++;
    }
  }

  addToken(TokenType::NUMBER, std::stod(source.substr(start, current - start)));
}

const void Scanner::handleIdentifier() {
  while (isalpha(peek()) || isdigit(peek())) {
    current++;
  }

  std::string text = source.substr(start, current - start);
  auto type = identifiers.find(text);
  if (type != identifiers.end()) {
    addToken(type->second);
    return;
  }
  addToken(TokenType::IDENTIFIER);
  return;
}

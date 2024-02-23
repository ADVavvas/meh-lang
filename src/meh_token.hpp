#pragma once

#include "meh_token_type.hpp"
#include <optional>
#include <string>
#include <variant>

using Null = std::monostate;
using literal_t = std::variant<double, bool, std::string, Null>;

class Token {
public:
  Token(const TokenType type, const std::string &lexeme, const int line,
        std::optional<literal_t> literal = std::nullopt)
      : type(type), lexeme(lexeme), literal(literal), line(line) {}

  Token(const Token &other)
      : type(other.type), lexeme(other.lexeme), literal(other.literal),
        line(other.line) {}

  Token &operator=(const Token &other) {
    if (this != &other) {
      type = other.type;
      lexeme = other.lexeme;
      line = other.line;
      literal = other.literal;
    }
    return *this;
  }

  const TokenType getType() const { return type; }

  const std::string &getLexeme() const { return lexeme; }

  const int getLine() const { return line; }

  const std::optional<literal_t> getLiteral() const { return literal; }

  bool operator==(const Token &t) const {
    return (type == t.type && lexeme == t.lexeme && line == t.line);
    // && literal == t.literal);
  }

  bool operator!=(const Token &t) const {
    return (type != t.type || lexeme != t.lexeme || line != t.line);
    // || literal != t.literal);
  }

private:
  TokenType type;
  std::string lexeme;
  int line;
  std::optional<literal_t> literal;
};
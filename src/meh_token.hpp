#pragma once

#include "meh_token_type.hpp"
#include <optional>
#include <string>
#include <variant>

struct Null {};
using literal_t = std::variant<double, bool, std::string, Null>;

class Token {
public:
  Token(const TokenType type, const std::string &lexeme, const int line,
        std::optional<literal_t> literal = std::nullopt)
      : type(type), lexeme(lexeme), literal(literal), line(line) {}

  const TokenType getType() const { return type; }

  const std::string &getLexeme() const { return lexeme; }

  const int getLine() const { return line; }

  const std::optional<literal_t> getLiteral() const { return literal; }

private:
  TokenType type;
  std::string lexeme;
  int line;
  std::optional<literal_t> literal;
};
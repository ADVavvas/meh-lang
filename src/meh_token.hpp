#include "meh_token_type.hpp"
#include <optional>
#include <string>
#include <variant>

using Literal = const std::variant<double>;

class Token {
public:
  Token(const TokenType type, const std::string &lexeme, const int line,
        std::optional<Literal> literal = std::nullopt)
      : type(type), lexeme(lexeme), literal(literal), line(line) {}

  const TokenType getType() const { return type; }

  const std::string &getLexeme() const { return lexeme; }

  const int getLine() const { return line; }

private:
  const TokenType type;
  const std::string lexeme;
  const int line;
  const std::optional<Literal> literal;
};
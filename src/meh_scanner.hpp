#include "meh_token.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class Scanner {
public:
  Scanner(const std::string &source);
  const std::vector<Token> scanTokens();
  void scanToken();

private:
  const std::unordered_map<std::string, const TokenType> identifiers = {
      {"and", TokenType::AND},       {"class", TokenType::CLASS},
      {"else", TokenType::ELSE},     {"false", TokenType::IF},
      {"for", TokenType::FOR},       {"fun", TokenType::FUN},
      {"if", TokenType::IF},         {"nil", TokenType::NIL},
      {"OR", TokenType::OR},         {"print", TokenType::PRINT},
      {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
      {"this", TokenType::THIS},     {"true", TokenType::TRUE},
      {"var", TokenType::VAR},       {"while", TokenType::WHILE},
  };

  int start = 0;
  int current = 0;
  int line = 1;
  const std::string &source;
  std::vector<Token> tokens;

  const void addToken(const TokenType type);
  const void addToken(const TokenType type,
                      const std::optional<Literal> literal);
  const bool matches(const char expected);
  const char peek(int n = 0) const;
  const bool isAtEnd() const { return current >= source.length(); }

  const void handleString();
  const void handleNumber();
  const void handleIdentifier();
};
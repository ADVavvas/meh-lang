#include "meh_expr.hpp"
#include "meh_token.hpp"
#include "meh_token_type.hpp"
#include <string>

class Parser {

public:
  Parser(std::vector<Token> tokens) : tokens(tokens) {
    // TODO: Check non-empty
  }

  ExprT parse();

private:
  std::vector<Token> tokens;
  int current = 0;

  ExprT comma();
  ExprT expression();
  ExprT equality();
  ExprT comparison();
  ExprT term();
  ExprT factor();
  ExprT unary();
  ExprT primary();

  void synchronize();
  bool match(std::vector<TokenType> types);
  bool check(TokenType type);
  bool isAtEnd();
  const Token &advance();
  const Token &previous();
  const Token &peek();
  const Token &consume(TokenType type, std::string message);
};
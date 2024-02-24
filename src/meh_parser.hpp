#include "meh_expr.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"
#include "meh_token_type.hpp"
#include <string>
#include <vector>

class Parser {

public:
  Parser(std::vector<Token> tokens) : tokens(tokens) {
    // TODO: Check non-empty
  }

  std::vector<StmtT> parse();

private:
  std::vector<Token> tokens;
  int current = 0;

  StmtT declaration();
  StmtT varDeclaration();
  StmtT classDeclaration();
  StmtT function(std::string kind);
  StmtT statement();
  StmtT printStatement();
  StmtT expressionStatement();
  StmtT forStatement();
  StmtT ifStatement();
  StmtT whileStatement();
  StmtT returnStatement();
  std::vector<StmtT> block();
  ExprT comma();
  ExprT assignment();
  ExprT expression();
  ExprT logicOr();
  ExprT logicAnd();
  ExprT equality();
  ExprT comparison();
  ExprT term();
  ExprT factor();
  ExprT unary();
  ExprT call();
  ExprT primary();

  void synchronize();
  bool match(std::vector<TokenType> types);
  bool check(TokenType type);
  bool isAtEnd();
  const Token &advance();
  const Token &previous();
  const Token &peek();
  const Token &consume(TokenType type, std::string message);
  ExprT finishCall(ExprT callee);
};
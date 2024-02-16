#include "meh_parser.hpp"
#include "meh.hpp"
#include "meh_expr.hpp"
#include "meh_parse_error.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"
#include "meh_token_type.hpp"
#include <iostream>
#include <stdexcept>
#include <variant>
#include <vector>

std::vector<StmtT> Parser::parse() {
  std::vector<StmtT> statements;

  try {
    while (!isAtEnd()) {
      statements.push_back(declaration());
    }
  } catch (MehParseError &e) {
    return {};
  }
  return statements;
}

StmtT Parser::declaration() {
  try {
    if (match({TokenType::VAR})) {
      return varDeclaration();
    }
    return statement();
  } catch (MehParseError &e) {
    synchronize();
    return StmtT{Null{}};
  }
}

StmtT Parser::statement() {
  if (match({TokenType::FOR})) {
    return forStatement();
  }
  if (match({TokenType::WHILE})) {
    return whileStatement();
  }
  if (match({TokenType::IF})) {
    return ifStatement();
  }
  if (match({TokenType::PRINT})) {
    return printStatement();
  }
  if (match({TokenType::BRACE_LEFT})) {
    return Block{block()};
  }
  return expressionStatement();
}

StmtT Parser::printStatement() {
  ExprT value{expression()};
  consume(SEMICOLON, "Expect ';' after value.");
  return StmtT{Print{value}};
}

StmtT Parser::expressionStatement() {
  ExprT expr{expression()};
  consume(SEMICOLON, "Expect ';' after expression.");
  return StmtT{Expression{expr}};
}

StmtT Parser::forStatement() {
  consume(PAREN_LEFT, "Expect '(' after 'for'.");
  StmtT initializer{Null{}};
  if (match({TokenType::SEMICOLON})) {
    initializer = Null{};
  } else if (match({TokenType::VAR})) {
    initializer = varDeclaration();
  } else {
    initializer = expressionStatement();
  }

  ExprT condition{Null{}};
  if (!check(SEMICOLON)) {
    condition = expression();
  }
  consume(SEMICOLON, "Expect ';' after loop condition.");

  ExprT increment{Null{}};
  if (!check(PAREN_RIGHT)) {
    increment = expression();
  }
  consume(PAREN_RIGHT, "Expect ')' after for clauses.");

  StmtT body{statement()};

  if (!std::holds_alternative<Null>(increment)) {
    // Add increment to the end of the body.
    body = StmtT{Block{std::vector<StmtT>{body, StmtT{Expression{increment}}}}};
  }

  if (std::holds_alternative<Null>(condition)) {
    condition = ExprT{Literal{true}};
  }

  body = StmtT{While{condition, body}};

  if (!std::holds_alternative<box<Null>>(initializer)) {
    body = StmtT{Block{std::vector<StmtT>{initializer, body}}};
  }

  return body;
}

StmtT Parser::ifStatement() {
  consume(PAREN_LEFT, "Expect '(' after 'if'.");
  ExprT condition{expression()};
  consume(PAREN_RIGHT, "Expect ')' after if condition.");

  StmtT thenBranch{statement()};
  StmtT elseBranch{Null{}};
  if (match({TokenType::ELSE})) {
    elseBranch = statement();
  }

  return StmtT{If{condition, thenBranch, elseBranch}};
}

StmtT Parser::whileStatement() {
  consume(PAREN_LEFT, "Expect '(' after 'while'.");
  ExprT condition{expression()};
  consume(PAREN_RIGHT, "Expect ')' after while condition.");

  StmtT body{statement()};

  return StmtT{While{condition, body}};
}

std::vector<StmtT> Parser::block() {
  std::vector<StmtT> statements;

  while (!check(TokenType::BRACE_RIGHT) && !isAtEnd()) {
    statements.push_back(declaration());
  }

  consume(TokenType::BRACE_RIGHT, "Expect '}' after block.");
  return statements;
}

StmtT Parser::varDeclaration() {
  Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

  ExprT initializer{Null{}};
  if (match({TokenType::EQUAL})) {
    initializer = expression();
  }
  consume(SEMICOLON, "Expect ';' after variable declaration.");
  return StmtT{Var{name, initializer}};
}

ExprT Parser::expression() { return comma(); }

ExprT Parser::comma() {
  ExprT expr{assignment()};

  while (match({TokenType::COMMA})) {
    Token op = previous();
    ExprT right{assignment()};
    expr = ExprT{Binary{expr, op, right}};
  }

  return expr;
}

ExprT Parser::assignment() {
  ExprT expr{logicOr()};

  if (match({TokenType::EQUAL})) {
    Token equals = previous();
    ExprT value{assignment()};

    if (std::holds_alternative<box<Variable>>(expr)) {
      Token name = std::get<box<Variable>>(expr)->name;
      return ExprT{Assign{name, value}};
    }

    Meh::error(equals, "Invalid assignment target.");
  }

  return expr;
}

ExprT Parser::logicOr() {
  ExprT expr{logicAnd()};

  while (match({TokenType::OR})) {
    Token op{previous()};
    ExprT right{logicAnd()};
    expr = ExprT{Logical{expr, op, right}};
  }

  return expr;
}

ExprT Parser::logicAnd() {
  ExprT expr{equality()};

  while (match({TokenType::AND})) {
    Token op{previous()};
    ExprT right{equality()};
    expr = ExprT{Logical{expr, op, right}};
  }

  return expr;
}

ExprT Parser::equality() {
  ExprT expr{comparison()};

  while (match({TokenType::NOT_EQUAL, TokenType::EQUAL_EQUAL})) {
    Token op{previous()};
    ExprT right{comparison()};
    expr = ExprT{Binary{expr, op, right}};
  }

  return expr;
}

ExprT Parser::comparison() {
  ExprT expr{term()};
  while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                TokenType::LESS_EQUAL})) {
    Token op{previous()};
    ExprT right{term()};

    expr = ExprT{Binary{expr, op, right}};
  }
  return expr;
}

ExprT Parser::term() {
  ExprT expr{factor()};

  while (match({TokenType::MINUS, TokenType::PLUS})) {
    Token op{previous()};
    ExprT right{factor()};
    expr = ExprT{Binary{expr, op, right}};
  }

  return expr;
}

ExprT Parser::factor() {
  ExprT expr{unary()};

  while (match({TokenType::SLASH, TokenType::STAR})) {
    Token op{previous()};
    ExprT right{unary()};
    expr = ExprT{Binary{expr, op, right}};
  }
  return expr;
}

ExprT Parser::unary() {
  if (match({TokenType::NOT, TokenType::MINUS})) {
    Token op{previous()};
    ExprT right{unary()};
    return ExprT{Unary{op, right}};
  }
  return call();
}

ExprT Parser::call() {
  ExprT expr{primary()};

  while (true) {
    if (match({TokenType::PAREN_LEFT})) {
      expr = finishCall(expr);
    } else {
      break;
    }
  }
  return expr;
}

ExprT Parser::primary() {
  if (match({TokenType::FALSE})) {
    return ExprT{Literal{false}};
  }
  if (match({TokenType::TRUE})) {
    return ExprT{Literal{true}};
  }
  if (match({TokenType::NIL})) {
    return ExprT{Literal{Null{}}};
  }
  if (match({TokenType::IDENTIFIER})) {
    return ExprT{Variable{previous()}};
  }

  if (match({TokenType::NUMBER, TokenType::STRING})) {
    if (previous().getLiteral().has_value()) {
      return ExprT{Literal{previous().getLiteral().value()}};
    } else {
      return ExprT{Literal{Null{}}};
    }
  }

  if (match({TokenType::PAREN_LEFT})) {
    ExprT expr{expression()};
    consume((TokenType::PAREN_RIGHT), "Expect ')' after expression.");
    return ExprT{Grouping{expr}};
  }

  Meh::error(peek(), "Expect expression.");
  throw MehParseError(peek(), "Expect expression.");
}

bool Parser::match(std::vector<TokenType> types) {
  for (auto type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

bool Parser::check(TokenType type) {
  if (isAtEnd()) {
    return false;
  }
  return peek().getType() == type;
}

bool Parser::isAtEnd() { return peek().getType() == TokenType::FILE_END; }

const Token &Parser::peek() { return tokens.at(current); }

const Token &Parser::advance() {
  if (!isAtEnd()) {
    current++;
  }
  return previous();
}

const Token &Parser::previous() { return tokens.at(current - 1); }

const Token &Parser::consume(TokenType type, std::string message) {
  if (check(type)) {
    return advance();
  }

  // TODO: Replace with ParseError
  Meh::error(peek(), message);
  throw MehParseError(peek(), message);
}

void Parser::synchronize() {
  advance();

  while (!isAtEnd()) {
    if (previous().getType() == TokenType::SEMICOLON) {
      return;
    }

    switch (peek().getType()) {
    case TokenType::CLASS:
    case TokenType::FUN:
    case TokenType::VAR:
    case TokenType::FOR:
    case TokenType::IF:
    case TokenType::WHILE:
    case TokenType::PRINT:
    case TokenType::RETURN:
      return;
    default:
      break;
    }

    advance();
  }
}

ExprT Parser::finishCall(ExprT callee) {
  std::vector<ExprT> arguments;

  if (!check(TokenType::PAREN_RIGHT)) {
    do {
      if (arguments.size() >= 255) {
        Meh::error(peek(), "Cannot have more than 255 arguments.");
      }
      arguments.push_back(expression());
    } while (match({TokenType::COMMA}));
  }

  Token paren = consume(TokenType::PAREN_RIGHT, "Expect ')' after arguments.");

  return Call{callee, paren, arguments};
}
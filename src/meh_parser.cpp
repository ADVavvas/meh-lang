#include "meh_parser.hpp"
#include "meh.hpp"
#include "meh_expr.hpp"
#include "meh_token.hpp"
#include <stdexcept>

ExprT Parser::parse() {
  try {
    return expression();
  } catch (std::runtime_error &e) {
    return ExprT{Literal{Null{}}};
  }
}

ExprT Parser::expression() { return comma(); }

ExprT Parser::comma() {
  ExprT expr{equality()};

  while (match({TokenType::COMMA})) {
    Token op = previous();
    ExprT right{equality()};
    expr = ExprT{Binary{expr, op, right}};
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
  return primary();
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
  throw std::runtime_error("Expect expression.");
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

  Meh::error(peek(), message);
  throw std::runtime_error(message);
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
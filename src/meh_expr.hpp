#pragma once

#include <variant>

#include "meh_token.hpp"
#include "meh_util.hpp"

class Binary;
class Grouping;
class Literal;
class Unary;

using ExprT =
    std::variant<box<Literal>, box<Binary>, box<Grouping>, box<Unary>>;

class Binary {
public:
  Binary(ExprT left, Token op, ExprT right);

  const ExprT left;
  const Token op;
  const ExprT right;
};

class Grouping {
public:
  Grouping(ExprT expr);

  const ExprT expr;
};

// TODO: variant of what?
// using literal_t = const std::variant<double>;

class Literal {
public:
  Literal(literal_t value);

  literal_t value;
};

class Unary {
public:
  Unary(Token op, ExprT expr);

  const Token op;
  const ExprT expr;
};
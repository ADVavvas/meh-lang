#pragma once

#include <variant>
#include <vector>

#include "meh_token.hpp"
#include "meh_util.hpp"

class Binary;
class Call;
class Grouping;
class Literal;
class Unary;
class Variable;
class Assign;
class Logical;

// TODO: Rename
// An expression. Not to be confused with Expression, that represents an
// expression statement.
using ExprT =
    std::variant<box<Literal>, box<Binary>, box<Call>, box<Grouping>,
                 box<Unary>, box<Variable>, box<Assign>, box<Logical>, Null>;

class Binary {
public:
  Binary(ExprT left, Token op, ExprT right);

  ExprT left;
  Token op;
  ExprT right;
};

class Call {
public:
  Call(ExprT callee, Token paren, std::vector<ExprT> arguments);

  ExprT callee;
  Token paren;
  std::vector<ExprT> arguments;
};

class Grouping {
public:
  Grouping(ExprT expr);

  ExprT expr;
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

  Token op;
  ExprT expr;
};

class Variable {
public:
  Variable(Token name);
  Token name;
};

class Assign {
public:
  Assign(Token name, ExprT value);

  Token name;
  ExprT value;
};

class Logical {
public:
  Logical(ExprT left, Token op, ExprT right);

  ExprT left;
  ExprT right;
  Token op;
};
#pragma once

#include "meh_expr.hpp"
#include "meh_util.hpp"
#include <variant>
#include <vector>

class Block;
class Expression;
class Print;
class Var;

// TODO: Rename
using StmtT =
    std::variant<box<Block>, box<Expression>, box<Print>, box<Var>, box<Null>>;

// An expression statement. Not to be confused with ExprT, that represents an
// expression.
class Expression {
public:
  Expression(ExprT expr);

  ExprT expr;
};

class Print {
public:
  Print(ExprT expr);

  ExprT expr;
};

class Var {
public:
  Var(Token name, ExprT initializer);

  Token name;
  ExprT initializer;
};

class Block {
public:
  Block(std::vector<StmtT> statements);

  std::vector<StmtT> statements;
};

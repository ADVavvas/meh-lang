#pragma once

#include "meh_expr.hpp"
#include "meh_util.hpp"
#include <variant>
#include <vector>

class Block;
class Expression;
class If;
class Print;
class Var;
class While;

// TODO: Rename
using StmtT = std::variant<box<Block>, box<Expression>, box<If>, box<Print>,
                           box<Var>, box<While>, box<Null>>;

// An expression statement. Not to be confused with ExprT, that represents an
// expression.
class Expression {
public:
  Expression(ExprT expr);

  ExprT expr;
};

class If {
public:
  If(ExprT condition, StmtT thenBranch, StmtT elseBranch);

  ExprT condition;
  StmtT thenBranch;
  StmtT elseBranch;
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

class While {
public:
  While(ExprT condition, StmtT body);

  ExprT condition;
  StmtT body;
};
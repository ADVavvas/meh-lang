#pragma once
#include "meh_expr.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"
#include "meh_util.hpp"
#include "meh_value.hpp"
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

enum FunctionType { NONE, FUNCTION, METHOD };

class Resolver {
public:
  Resolver(Interpreter &interpreter);
  void resolve(std::vector<StmtT> const &statements);
  // ExprT visitor methods
  void operator()(box<Binary> const &expr);
  void operator()(box<Call> const &expr);
  void operator()(box<Grouping> const &expr);
  void operator()(box<Literal> const &expr);
  void operator()(box<Unary> const &expr);
  void operator()(box<Variable> const &expr);
  void operator()(box<Assign> const &expr);
  void operator()(box<Logical> const &expr);
  void operator()(box<Get> const &expr);
  void operator()(box<Set> const &expr);
  void operator()(Null const &expr);

  // StmT visitor methods
  void operator()(box<Block> const &stmt);
  void operator()(box<Expression> const &stmt);
  void operator()(box<Class> const &stmt);
  void operator()(box<Function> const &stmt);
  void operator()(box<If> const &stmt);
  void operator()(box<Print> const &stmt);
  void operator()(box<Var> const &stmt);
  void operator()(box<While> const &stmt);
  void operator()(box<Return> const &stmt);
  void operator()(box<Null> const &stmt);

private:
  Interpreter &interpreter;
  std::stack<std::unordered_map<std::string, bool>> scopes;
  FunctionType currentFunction = NONE;
  void beginScope();
  void endScope();
  void resolve(StmtT const &statement);
  void resolve(ExprT const &expression);
  void resolveLocal(ExprT const &expr, Token const &name);
  void resolveFunction(box<Function> const &function, FunctionType type);
  void define(Token const &name);
  void declare(Token const &name);
};
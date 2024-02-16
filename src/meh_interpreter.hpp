#pragma once
#include "meh_environment.hpp"
#include "meh_expr.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"
#include "meh_util.hpp"
#include "meh_value.hpp"
#include <memory>
#include <string>

class Interpreter {
public:
  Interpreter();
  // ExprT visitor methods
  MehValue operator()(box<Binary> const &expr);
  MehValue operator()(box<Call> const &expr);
  MehValue operator()(box<Grouping> const &expr);
  MehValue operator()(box<Literal> const &expr);
  MehValue operator()(box<Unary> const &expr);
  MehValue operator()(box<Variable> const &expr);
  MehValue operator()(box<Assign> const &expr);
  MehValue operator()(box<Logical> const &expr);
  MehValue operator()(Null const &expr);

  // StmT visitor methods
  void operator()(box<Block> const &stmt);
  void operator()(box<Expression> const &stmt);
  void operator()(box<If> const &stmt);
  void operator()(box<Print> const &stmt);
  void operator()(box<Var> const &stmt);
  void operator()(box<While> const &stmt);
  void operator()(box<Null> const &stmt);

  void interpret(std::vector<StmtT> const &stmts);

private:
  MehEnvironment globalEnvironment{};
  MehEnvironment *environment{&globalEnvironment};
  void execute(StmtT const &stmt);
  void executeBlock(std::vector<StmtT> const &statements,
                    MehEnvironment environment);
  MehValue evaluate(box<ExprT> const &expr);
  bool isTruthy(MehValue value) const;
  bool isTruthy(literal_t value) const;
  bool isEqual(literal_t a, literal_t b) const;
  void checkNumberOperand(const Token op, const literal_t operand) const;
  void checkNumberOperands(const Token op, const literal_t left,
                           const literal_t right) const;
  std::string stringify(MehValue value) const;
};
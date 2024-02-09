#pragma once
#include "meh_expr.hpp"
#include "meh_token.hpp"
#include <string>

struct Obj {};

using MehValue = std::variant<box<literal_t>, Obj>;

class Interpreter {
public:
  MehValue operator()(box<Binary> const &expr) const;
  MehValue operator()(box<Grouping> const &expr) const;
  MehValue operator()(box<Literal> const &expr) const;
  MehValue operator()(box<Unary> const &expr) const;

  void interpret(box<ExprT> const &expr) const;

private:
  MehValue evaluate(box<ExprT> const &expr) const;
  bool isTruthy(literal_t value) const;
  bool isEqual(literal_t a, literal_t b) const;
  void checkNumberOperand(const Token op, const literal_t operand) const;
  void checkNumberOperands(const Token op, const literal_t left,
                           const literal_t right) const;
  std::string stringify(MehValue value) const;
};
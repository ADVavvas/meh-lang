#include "meh_interpreter.hpp"
#include "meh.hpp"
#include "meh_expr.hpp"
#include "meh_runtime_error.hpp"
#include "meh_token.hpp"
#include "meh_token_type.hpp"
#include <iostream>
#include <variant>

void Interpreter::interpret(box<ExprT> const &expr) const {
  try {
    MehValue value{evaluate(expr)};
    std::cout << stringify(value) << std::endl;
  } catch (MehRuntimeError &e) {
    Meh::runtimeError(e);
  }
}

MehValue Interpreter::operator()(box<Binary> const &expr) const {
  Token op{expr->op};
  MehValue left{evaluate(expr->left)};
  MehValue right{evaluate(expr->right)};
  box<literal_t> left_val{std::get<box<literal_t>>(left)};
  box<literal_t> right_val{std::get<box<literal_t>>(right)};

  switch (op.getType()) {
  case MINUS:
    checkNumberOperands(op, *left_val, *right_val);
    return MehValue{
        literal_t{std::get<double>(*left_val) - std::get<double>(*right_val)}};
  case SLASH:
    checkNumberOperands(op, *left_val, *right_val);
    return MehValue{
        literal_t{std::get<double>(*left_val) / std::get<double>(*right_val)}};
  case STAR:
    checkNumberOperands(op, *left_val, *right_val);
    return MehValue{
        literal_t{std::get<double>(*left_val) * std::get<double>(*right_val)}};
  case PLUS:
    if (std::holds_alternative<double>(*left_val) &&
        std::holds_alternative<double>(*right_val)) {
      return MehValue{literal_t{std::get<double>(*left_val) +
                                std::get<double>(*right_val)}};
    } else if (std::holds_alternative<std::string>(*left_val) &&
               std::holds_alternative<std::string>(*right_val)) {
      return MehValue{literal_t{std::get<std::string>(*left_val) +
                                std::get<std::string>(*right_val)}};
    } else {
      throw MehRuntimeError{op, "Operands must be two numbers or two strings."};
    }
  case GREATER:
    checkNumberOperands(op, *left_val, *right_val);
    return MehValue{
        literal_t{std::get<double>(*left_val) > std::get<double>(*right_val)}};
  case GREATER_EQUAL:
    checkNumberOperands(op, *left_val, *right_val);
    return MehValue{
        literal_t{std::get<double>(*left_val) >= std::get<double>(*right_val)}};
  case LESS:
    checkNumberOperands(op, *left_val, *right_val);
    return MehValue{
        literal_t{std::get<double>(*left_val) < std::get<double>(*right_val)}};
  case LESS_EQUAL:
    checkNumberOperands(op, *left_val, *right_val);
    return MehValue{
        literal_t{std::get<double>(*left_val) <= std::get<double>(*right_val)}};
  case EQUAL_EQUAL:
    return MehValue{literal_t{isEqual(*left_val, *right_val)}};
  case NOT_EQUAL:
    return MehValue{literal_t{!isEqual(*left_val, *right_val)}};

  default:
    return MehValue{literal_t{Null{}}};
  }
}

MehValue Interpreter::operator()(box<Grouping> const &expr) const {
  return evaluate(expr->expr);
}

MehValue Interpreter::operator()(box<Literal> const &expr) const {
  return MehValue{expr->value};
}

MehValue Interpreter::operator()(box<Unary> const &expr) const {
  Token op{expr->op};
  MehValue right{evaluate(expr->expr)};
  box<literal_t> right_val{std::get<box<literal_t>>(right)};

  switch (op.getType()) {
  case MINUS:
    checkNumberOperand(op, *right_val);
    return MehValue{literal_t{-std::get<double>(*right_val)}};
  case NOT:
    return MehValue{literal_t{isTruthy(*right_val)}};
  default:
    return MehValue{literal_t{Null{}}};
  }
}

MehValue Interpreter::evaluate(box<ExprT> const &expr) const {
  return std::visit(*this, *expr);
}

bool Interpreter::isTruthy(literal_t value) const {
  if (std::holds_alternative<Null>(value)) {
    return false;
  }
  if (std::holds_alternative<bool>(value)) {
    return std::get<bool>(value);
  }
  if (std::holds_alternative<double>(value)) {
    return std::get<double>(value) != 0;
  }
  if (std::holds_alternative<std::string>(value))
    return !std::get<std::string>(value).empty();
  return false;
}

bool Interpreter::isEqual(literal_t a, literal_t b) const {
  if (std::holds_alternative<Null>(a) && std::holds_alternative<Null>(b)) {
    return true;
  }
  if (std::holds_alternative<Null>(a) || std::holds_alternative<Null>(b)) {
    return false;
  }
  if (std::holds_alternative<double>(a) && std::holds_alternative<double>(b)) {
    return std::get<double>(a) == std::get<double>(b);
  }
  if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
    return std::get<bool>(a) == std::get<bool>(b);
  }
  if (std::holds_alternative<std::string>(a) &&
      std::holds_alternative<std::string>(b)) {
    return std::get<std::string>(a) == std::get<std::string>(b);
  }
  return false;
}

void Interpreter::checkNumberOperand(const Token op,
                                     const literal_t operand) const {
  if (std::holds_alternative<double>(operand)) {
    return;
  }
  throw MehRuntimeError{op, "Operand must be a number."};
}

void Interpreter::checkNumberOperands(const Token op, const literal_t left,
                                      const literal_t right) const {
  if (std::holds_alternative<double>(left) &&
      std::holds_alternative<double>(right)) {
    return;
  }
  throw MehRuntimeError{op, "Operands must be numbers."};
}

std::string Interpreter::stringify(MehValue value) const {

  // Literal
  if (std::holds_alternative<box<literal_t>>(value)) {
    box<literal_t> val{std::get<box<literal_t>>(value)};
    if (std::holds_alternative<Null>(*val)) {
      return "nil";
    }
    if (std::holds_alternative<double>(*val)) {
      return std::to_string(std::get<double>(*val));
    }
    if (std::holds_alternative<std::string>(*val)) {
      return std::get<std::string>(*val);
    }
    if (std::holds_alternative<bool>(*val)) {
      return std::get<bool>(*val) ? "true" : "false";
    }
  }
  // Else it's Object type (or bad variant type)
  // TODO: Object definition required?
  return "Object";
}
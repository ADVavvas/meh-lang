#include "meh_interpreter.hpp"
#include "meh.hpp"
#include "meh_environment.hpp"
#include "meh_expr.hpp"
#include "meh_runtime_error.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"
#include "meh_token_type.hpp"
#include "meh_util.hpp"
#include "meh_value.hpp"
#include <exception>
#include <iostream>
#include <optional>
#include <variant>
#include <vector>

Interpreter::Interpreter() {
  MehNativeFunction clock{
      0,
      [](Interpreter &interpreter,
         std::vector<MehValue> arguments) -> MehValue {
        return MehValue{literal_t{static_cast<double>(std::time(nullptr))}};
      }};
  environment->define("clock", clock);
}

void Interpreter::interpret(std::vector<StmtT> const &stmts) {
  try {
    for (auto const &stmt : stmts) {
      execute(stmt);
    }
  } catch (MehRuntimeError &e) {
    Meh::runtimeError(e);
  }
}

MehValue Interpreter::operator()(box<Binary> const &expr) {
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

MehValue Interpreter::operator()(box<Call> const &expr) {
  MehValue callee{evaluate(expr->callee)};
  std::vector<MehValue> arguments{};
  for (auto const &arg : expr->arguments) {
    arguments.push_back(evaluate(arg));
  }

  // Not ideal design
  if (std::holds_alternative<box<MehNativeFunction>>(callee)) {
    MehNativeFunction function = *std::get<box<MehNativeFunction>>(callee);
    if (arguments.size() != function.getArity()) {
      throw MehRuntimeError{expr->paren,
                            "Expected " + std::to_string(function.getArity()) +
                                " arguments but got " +
                                std::to_string(arguments.size()) + "."};
    }
    return function.call(*this, arguments);
  } else if (std::holds_alternative<box<MehFunction>>(callee)) {
    MehFunction function = *std::get<box<MehFunction>>(callee);
    if (arguments.size() != function.getArity()) {
      throw MehRuntimeError{expr->paren,
                            "Expected " + std::to_string(function.getArity()) +
                                " arguments but got " +
                                std::to_string(arguments.size()) + "."};
    }
    return function.call(*this, arguments);
  } else {
    throw MehRuntimeError{expr->paren, "Can only call functions and classes."};
  }
}

MehValue Interpreter::operator()(box<Grouping> const &expr) {
  return evaluate(expr->expr);
}

MehValue Interpreter::operator()(box<Literal> const &expr) {
  return MehValue{expr->value};
}

MehValue Interpreter::operator()(box<Unary> const &expr) {
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

MehValue Interpreter::operator()(box<Variable> const &expr) {
  // TODO:
  return environment->get(expr->name);
}

MehValue Interpreter::operator()(box<Assign> const &expr) {
  MehValue value{evaluate(expr->value)};
  environment->assign(expr->name, value);
  return value;
}

MehValue Interpreter::operator()(box<Logical> const &expr) {
  MehValue left{evaluate(expr->left)};
  if (expr->op.getType() == OR) {
    if (isTruthy(left)) {
      // Alternative return the left value if it's truthy
      return MehValue{literal_t{true}};
    }
  } else {
    if (!isTruthy(left)) {
      return MehValue{literal_t{false}};
    }
  }

  return MehValue{literal_t{isTruthy(evaluate(expr->right))}};
}

MehValue Interpreter::operator()(Null const &expr) {
  // TODO: What to do?
  return MehValue{literal_t{Null{}}};
}

void Interpreter::operator()(box<Block> const &stmt) {
  executeBlock(stmt->statements, MehEnvironment{environment});
}

void Interpreter::operator()(box<Print> const &stmt) {
  MehValue value{evaluate(stmt->expr)};
  std::cout << stringify(value) << std::endl;
}

void Interpreter::operator()(box<Var> const &stmt) {
  MehValue value{literal_t{Null{}}};
  // Check type of variant
  if (!std::holds_alternative<Null>(stmt->initializer)) {
    value = evaluate(stmt->initializer);
  }
  environment->define(stmt->name.getLexeme(), value);
}

void Interpreter::operator()(box<Expression> const &stmt) {
  evaluate(stmt->expr);
}

void Interpreter::operator()(box<If> const &stmt) {
  if (isTruthy(evaluate(stmt->condition))) {
    execute(stmt->thenBranch);
  } else if (!std::holds_alternative<box<Null>>(stmt->elseBranch)) {
    execute(stmt->elseBranch);
  }
}

void Interpreter::operator()(box<While> const &stmt) {
  while (isTruthy(evaluate(stmt->condition))) {
    execute(stmt->body);
  }
}

void Interpreter::operator()(box<Function> const &stmt) {
  MehFunction function{*stmt};
  environment->define(stmt->name.getLexeme(), function);
}

void Interpreter::operator()(box<Null> const &stmt) {
  // TODO: What to do?
}

void Interpreter::execute(StmtT const &stmt) { std::visit(*this, stmt); }

void Interpreter::executeBlock(std::vector<StmtT> const &statements,
                               MehEnvironment environment) {
  MehEnvironment *previous{this->environment};
  try {
    this->environment = &environment;
    for (auto const &stmt : statements) {
      execute(stmt);
    }
  } catch (std::exception &e) {
    this->environment = previous;
  }
  this->environment = previous;
}

MehValue Interpreter::evaluate(box<ExprT> const &expr) {
  return std::visit(*this, *expr);
}

bool Interpreter::isTruthy(MehValue value) const {
  if (std::holds_alternative<box<literal_t>>(value)) {
    return isTruthy(*std::get<box<literal_t>>(value));
  }

  // If it's an object, it's not defined.
  return false;
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
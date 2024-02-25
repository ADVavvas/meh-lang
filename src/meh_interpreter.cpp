#include "meh_interpreter.hpp"
#include "meh.hpp"
#include "meh_environment.hpp"
#include "meh_expr.hpp"
#include "meh_return.hpp"
#include "meh_runtime_error.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"
#include "meh_token_type.hpp"
#include "meh_util.hpp"
#include "meh_value.hpp"
#include <exception>
#include <ios>
#include <iostream>
#include <memory>
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
  } else if (std::holds_alternative<box<MehClass>>(callee)) {
    MehClass klass = *std::get<box<MehClass>>(callee);
    if (arguments.size() != klass.getArity()) {
      throw MehRuntimeError{expr->paren,
                            "Expected " + std::to_string(klass.getArity()) +
                                " arguments but got " +
                                std::to_string(arguments.size()) + "."};
    }
    return klass.call(*this, arguments);
  } else {
    throw MehRuntimeError{expr->paren, "Can only call functions and classes."};
  }
  throw MehRuntimeError{expr->paren, "Can only call functions and classes."};
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
  return lookupVariable(expr->name, expr);
}

MehValue Interpreter::operator()(box<Assign> const &expr) {
  MehValue value{evaluate(expr->value)};
  // MyTest val{1, 2};
  // auto t = test.find(val);
  // std::cout << t->second << std::endl;
  auto distance{locals.find(expr)};
  if (distance != locals.end()) {
    environment->assignAt(distance->second, expr->name, value);
  } else {
    globalEnvironment->assign(expr->name, value);
  }
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

MehValue Interpreter::operator()(box<Get> const &expr) {
  MehValue object{evaluate(expr->obj)};
  if (std::holds_alternative<box<MehInstance>>(object)) {
    MehInstance instance = *std::get<box<MehInstance>>(object);
    return instance.get(expr->name);
  }
  throw MehRuntimeError(expr->name, "Only instances have properties.");
}

MehValue Interpreter::operator()(box<Set> const &expr) {
  MehValue object{evaluate(expr->obj)};

  if (std::holds_alternative<box<MehInstance>>(object)) {
    MehInstance instance = *std::get<box<MehInstance>>(object);
    MehValue value{evaluate(expr->value)};
    instance.set(expr->name, value);
    return value;
  }

  throw MehRuntimeError(expr->name, "Only instances have fields.");
}

MehValue Interpreter::operator()(Null const &expr) {
  // TODO: What to do?
  return MehValue{literal_t{Null{}}};
}

void Interpreter::operator()(box<Block> const &stmt) {
  executeBlock(stmt->statements,
               std::make_shared<MehEnvironment>(this->environment));
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

void Interpreter::operator()(box<Class> const &stmt) {
  environment->define(stmt->name.getLexeme(), MehValue{literal_t{Null{}}});

  std::unordered_map<std::string, MehFunction> methods{};
  for (auto const &method : stmt->methods) {
    MehFunction function{method, environment};
    methods.insert_or_assign(method.name.getLexeme(), function);
  }

  MehClass klass{stmt->name.getLexeme(), methods};
  environment->assign(stmt->name, klass);
}

void Interpreter::operator()(box<Function> const &stmt) {
  MehFunction function{*stmt, this->environment};
  environment->define(stmt->name.getLexeme(), function);
}

void Interpreter::operator()(box<Return> const &stmt) {
  MehValue value{literal_t{Null{}}};
  if (!std::holds_alternative<Null>(stmt->value)) {
    value = evaluate(stmt->value);
  }
  throw MehReturn{value};
}

void Interpreter::operator()(box<Null> const &stmt) {
  // TODO: What to do?
}

void Interpreter::execute(StmtT const &stmt) { std::visit(*this, stmt); }

void Interpreter::executeBlock(std::vector<StmtT> const &statements,
                               std::shared_ptr<MehEnvironment> environment) {
  std::shared_ptr<MehEnvironment> previous{this->environment};
  try {
    this->environment = environment;
    for (auto const &stmt : statements) {
      execute(stmt);
    }
  } catch (MehRuntimeError &e) {
    this->environment = previous;
  } catch (MehReturn &e) {
    this->environment = previous;
    throw e;
  } catch (std::exception &e) {
    this->environment = previous;
    throw e;
  }
  this->environment = previous;
}

void Interpreter::resolve(ExprT const &expression, int depth) {
  locals.insert_or_assign(expression, depth);
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
  } else if (std::holds_alternative<box<MehClass>>(value)) {
    return std::get<box<MehClass>>(value)->getName();
  } else if (std::holds_alternative<box<MehFunction>>(value)) {
    return std::get<box<MehFunction>>(value)->getFunction().name.getLexeme();
  } else if (std::holds_alternative<box<MehInstance>>(value)) {
    return std::get<box<MehInstance>>(value)->toString();
  }
  // Else it's Object type (or bad variant type)
  // TODO: Object definition required?
  return "Object";
}

MehValue Interpreter::lookupVariable(Token const &name, ExprT const &expr) {
  auto distance{locals.find(expr)};
  if (distance == locals.end()) {
    return globalEnvironment->get(name);
  }
  return environment->getAt(distance->second, name.getLexeme());
}

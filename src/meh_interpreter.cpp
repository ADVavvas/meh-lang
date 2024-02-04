#include "meh_interpreter.hpp"
#include "meh_token.hpp"
#include "meh_token_type.hpp"
MehValue Interpreter::operator()(box<Binary> const &expr) const {
  Token op{expr->op};
  MehValue left{evaluate(expr->left)};
  MehValue right{evaluate(expr->right)};
  box<literal_t> left_val{std::get<box<literal_t>>(left)};
  box<literal_t> right_val{std::get<box<literal_t>>(right)};

  switch (op.getType()) {
  case MINUS:
    return MehValue{
        literal_t{std::get<double>(*left_val) - std::get<double>(*right_val)}};
  case SLASH:
    return MehValue{
        literal_t{std::get<double>(*left_val) / std::get<double>(*right_val)}};
  case STAR:
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
      return MehValue{literal_t{Null{}}};
    }
  case GREATER:
    return MehValue{
        literal_t{std::get<double>(*left_val) > std::get<double>(*right_val)}};
  case GREATER_EQUAL:
    return MehValue{
        literal_t{std::get<double>(*left_val) >= std::get<double>(*right_val)}};
  case LESS:
    return MehValue{
        literal_t{std::get<double>(*left_val) < std::get<double>(*right_val)}};
  case LESS_EQUAL:
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
  if (std::holds_alternative<std::string>(a) &&
      std::holds_alternative<std::string>(b)) {
    return std::get<std::string>(a) == std::get<std::string>(b);
  }
  return false;
}
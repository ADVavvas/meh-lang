#include "meh_expr.hpp"
#include "meh_token.hpp"

Binary::Binary(ExprT left, Token op, ExprT right)
    : left{left}, op{op}, right{right} {}

Call::Call(ExprT callee, Token paren, std::vector<ExprT> arguments)
    : callee{callee}, paren{paren}, arguments{arguments} {}

Grouping::Grouping(ExprT expr) : expr{expr} {}

Literal::Literal(literal_t value) : value{value} {}

Unary::Unary(Token op, ExprT expr) : op{op}, expr{expr} {}

Variable::Variable(Token name) : name{name}, id{counter++} {}

Variable::Variable(const Variable &other) : name{other.name}, id{other.id} {}

Variable::Variable(Variable &&other) noexcept
    : name{std::move(other.name)}, id{std::exchange(other.id, 0)} {}

Variable &Variable::operator=(const Variable &other) {
  if (this != &other) {
    name = other.name;
    id = other.id;
  }
  return *this;
}

Variable &Variable::operator=(Variable &&other) noexcept {
  if (this != &other) {
    name = std::move(other.name);
    id = std::exchange(other.id, 0);
  }
  return *this;
}

Assign::Assign(Token name, ExprT value) : name{name}, value{value} {}

Logical::Logical(ExprT left, Token op, ExprT right)
    : left{left}, op{op}, right{right} {}

Get::Get(ExprT obj, Token name) : obj{obj}, name{name} {}

Set::Set(ExprT obj, Token name, ExprT value)
    : obj{obj}, name{name}, value{value} {}

This::This(Token keyword) : keyword{keyword} {}
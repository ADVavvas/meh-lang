#include "meh_expr.hpp"
#include "meh_token.hpp"

Binary::Binary(ExprT left, Token op, ExprT right)
    : left(left), op(op), right(right) {}

Grouping::Grouping(ExprT expr) : expr(expr) {}

Literal::Literal(literal_t value) : value(value) {}

Unary::Unary(Token op, ExprT expr) : op(op), expr(expr) {}

Variable::Variable(Token name) : name(name) {}

Assign::Assign(Token name, ExprT value) : name{name}, value{value} {};

Logical::Logical(ExprT left, Token op, ExprT right)
    : left{left}, op{op}, right{right} {};

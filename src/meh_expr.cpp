#include "meh_expr.hpp"
#include "meh_token.hpp"

Binary::Binary(ExprT left, Token op, ExprT right)
    : left(left), op(op), right(right) {}

Grouping::Grouping(ExprT expr) : expr(expr) {}

Literal::Literal(literal_t value) : value(value) {}

Unary::Unary(Token op, ExprT expr) : op(op), expr(expr) {}

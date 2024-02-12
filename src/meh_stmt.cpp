#include "meh_stmt.hpp"

Expression::Expression(ExprT expr) : expr{expr} {};

Print::Print(ExprT expr) : expr{expr} {};

Var::Var(Token name, ExprT initializer)
    : name{name}, initializer{initializer} {};

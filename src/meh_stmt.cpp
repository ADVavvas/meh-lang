#include "meh_stmt.hpp"

Expression::Expression(ExprT expr) : expr{expr} {};

If::If(ExprT condition, StmtT thenBranch, StmtT elseBranch)
    : condition{condition}, thenBranch{thenBranch}, elseBranch{elseBranch} {};

Print::Print(ExprT expr) : expr{expr} {};

Var::Var(Token name, ExprT initializer)
    : name{name}, initializer{initializer} {};

Block::Block(std::vector<StmtT> statements) : statements{statements} {};

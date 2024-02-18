#include "meh_stmt.hpp"

Expression::Expression(ExprT expr) : expr{expr} {};

If::If(ExprT condition, StmtT thenBranch, StmtT elseBranch)
    : condition{condition}, thenBranch{thenBranch}, elseBranch{elseBranch} {};

Print::Print(ExprT expr) : expr{expr} {};

Var::Var(Token name, ExprT initializer)
    : name{name}, initializer{initializer} {};

Block::Block(std::vector<StmtT> statements) : statements{statements} {};

While::While(ExprT condition, StmtT body) : condition{condition}, body{body} {};

Function::Function(Token name, std::vector<Token> params,
                   std::vector<StmtT> body)
    : name{name}, params{params}, body{body} {};

Return::Return(Token keyword, ExprT value) : keyword{keyword}, value{value} {};

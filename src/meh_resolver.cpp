#include "meh_resolver.hpp"
#include "meh.hpp"
#include "meh_expr.hpp"
#include <variant>

Resolver::Resolver(Interpreter &interpreter) : interpreter(interpreter) {}
// ExprT visitor methods
void Resolver::operator()(box<Binary> const &expr) {
  resolve(expr->left);
  resolve(expr->right);
}
void Resolver::operator()(box<Call> const &expr) {
  resolve(expr->callee);
  for (auto const &argument : expr->arguments) {
    resolve(argument);
  }
}
void Resolver::operator()(box<Grouping> const &expr) { resolve(expr->expr); }
void Resolver::operator()(box<Literal> const &expr) {
  // Nothing
}
void Resolver::operator()(box<Unary> const &expr) { resolve(expr->expr); }
void Resolver::operator()(box<Variable> const &expr) {
  if (!scopes.empty() &&
      scopes.top().find(expr->name.getLexeme()) != scopes.top().end() &&
      !scopes.top()[expr->name.getLexeme()]) {
    Meh::error(expr->name,
               "Cannot read local variable in its own initializer.");
  }
  resolveLocal(expr, expr->name);
}
void Resolver::operator()(box<Assign> const &expr) {
  resolve(expr->value);
  resolveLocal(expr, expr->name);
}
void Resolver::operator()(box<Logical> const &expr) {
  resolve(expr->left);
  resolve(expr->right);
}
void Resolver::operator()(box<Get> const &expr) { resolve(expr->obj); }
void Resolver::operator()(box<Set> const &expr) {
  resolve(expr->value);
  resolve(expr->obj);
}
void Resolver::operator()(Null const &expr) {
  // Nothing
}

// StmT visitor methods
void Resolver::operator()(box<Block> const &stmt) {
  beginScope();
  resolve(stmt->statements);
  endScope();
}
void Resolver::operator()(box<Expression> const &stmt) { resolve(stmt->expr); }
void Resolver::operator()(box<Class> const &stmt) {
  declare(stmt->name);
  define(stmt->name);

  for (auto const &method : stmt->methods) {
    FunctionType declaration = METHOD;
    resolveFunction(method, declaration);
  }
}
void Resolver::operator()(box<Function> const &stmt) {
  declare(stmt->name);
  define(stmt->name);
  resolveFunction(stmt, FUNCTION);
}
void Resolver::operator()(box<If> const &stmt) {
  resolve(stmt->condition);
  resolve(stmt->thenBranch);
  resolve(stmt->elseBranch);
}
void Resolver::operator()(box<Print> const &stmt) { resolve(stmt->expr); }
void Resolver::operator()(box<Var> const &stmt) {
  declare(stmt->name);
  if (!std::holds_alternative<Null>(stmt->initializer)) {
    resolve(stmt->initializer);
  }
  define(stmt->name);
}
void Resolver::operator()(box<While> const &stmt) {
  resolve(stmt->condition);
  resolve(stmt->body);
}
void Resolver::operator()(box<Return> const &stmt) {
  if (currentFunction == NONE) {
    Meh::error(stmt->keyword, "Cannot return from top-level code.");
  }
  resolve(stmt->value);
}
void Resolver::operator()(box<Null> const &stmt) {}

void Resolver::beginScope() {
  scopes.push(std::unordered_map<std::string, bool>());
}
void Resolver::endScope() { scopes.pop(); }
void Resolver::resolve(std::vector<StmtT> const &statements) {
  for (auto const &statement : statements) {
    resolve(statement);
  }
}
void Resolver::resolve(StmtT const &statement) { std::visit(*this, statement); }

void Resolver::resolve(ExprT const &expression) {
  std::visit(*this, expression);
}

void Resolver::resolveLocal(ExprT const &expr, Token const &name) {
  for (int i = scopes.size() - 1; i >= 0; --i) {
    if (scopes.top().find(name.getLexeme()) != scopes.top().end()) {
      interpreter.resolve(expr, scopes.size() - 1 - i);
      return;
    }
  }
}

void Resolver::resolveFunction(box<Function> const &function,
                               FunctionType type) {
  FunctionType enclosingFunction = currentFunction;
  currentFunction = type;
  beginScope();
  for (auto const &param : function->params) {
    declare(param);
    define(param);
  }
  resolve(function->body);
  endScope();
  currentFunction = enclosingFunction;
}

void Resolver::declare(Token const &name) {
  if (scopes.empty()) {
    return;
  }
  auto &scope = scopes.top();
  if (scope.count(name.getLexeme()) > 0) {
    Meh::error(name, "Variable with this name already declared in this scope.");
  }
  scope[name.getLexeme()] = false;
}

void Resolver::define(Token const &name) {
  if (scopes.empty()) {
    return;
  }
  auto &scope = scopes.top();
  scope[name.getLexeme()] = true;
}
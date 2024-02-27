#pragma once
#include "meh_environment.hpp"
#include "meh_expr.hpp"
#include "meh_runtime_error.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"
#include "meh_util.hpp"
#include "meh_value.hpp"
#include "meh_variant_hash.hpp" // This is a custom hash for std::variant
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

class Interpreter {
public:
  Interpreter();
  // ExprT visitor methods
  MehValue operator()(box<Binary> const &expr);
  MehValue operator()(box<Call> const &expr);
  MehValue operator()(box<Grouping> const &expr);
  MehValue operator()(box<Literal> const &expr);
  MehValue operator()(box<Unary> const &expr);
  MehValue operator()(box<Variable> const &expr);
  MehValue operator()(box<Assign> const &expr);
  MehValue operator()(box<Logical> const &expr);
  MehValue operator()(box<Get> const &expr);
  MehValue operator()(box<Set> const &expr);
  MehValue operator()(box<This> const &expr);
  MehValue operator()(Null const &expr);

  // StmT visitor methods
  void operator()(box<Block> const &stmt);
  void operator()(box<Expression> const &stmt);
  void operator()(box<Class> const &stmt);
  void operator()(box<Function> const &stmt);
  void operator()(box<If> const &stmt);
  void operator()(box<Print> const &stmt);
  void operator()(box<Var> const &stmt);
  void operator()(box<While> const &stmt);
  void operator()(box<Return> const &stmt);
  void operator()(box<Null> const &stmt);

  void interpret(std::vector<StmtT> const &stmts);

  std::shared_ptr<MehEnvironment> getGlobalEnvironment() {
    return globalEnvironment;
  }
  void executeBlock(std::vector<StmtT> const &statements,
                    std::shared_ptr<MehEnvironment> environment);
  void resolve(ExprT const &expression, int depth);

private:
  std::function<MehValue &(const ExprT &)> mutableVarVisitor =
      [this](const ExprT &expr) -> MehValue & {
    using T = std::decay_t<decltype(expr)>;
    if (std::holds_alternative<box<Variable>>(expr)) {
      auto &temp = std::get<box<Variable>>(expr);
      return lookupVariable(temp->name, expr);
    } else if (std::holds_alternative<box<Get>>(expr)) {
      auto &temp = std::get<box<Get>>(expr);
      MehValue &object{std::visit(mutableVarVisitor, temp->obj)};
      if (std::holds_alternative<box<MehInstance>>(object)) {
        box<MehInstance> &instance = std::get<box<MehInstance>>(object);
        return instance->getMut(temp->name);
      }
      throw MehRuntimeError(temp->name, "Only instances have properties.");
    } else {
      throw std::runtime_error("Invalid visit invocation");
    }
  };

  std::unordered_map<ExprT, int> locals;
  std::shared_ptr<MehEnvironment> globalEnvironment =
      std::make_shared<MehEnvironment>();
  std::shared_ptr<MehEnvironment> environment{globalEnvironment};
  void execute(StmtT const &stmt);
  MehValue evaluate(box<ExprT> const &expr);
  bool isTruthy(MehValue value) const;
  bool isTruthy(literal_t value) const;
  bool isEqual(literal_t a, literal_t b) const;
  void checkNumberOperand(const Token op, const literal_t operand) const;
  void checkNumberOperands(const Token op, const literal_t left,
                           const literal_t right) const;
  std::string stringify(MehValue value) const;
  MehValue &lookupVariable(Token const &name, ExprT const &expr);
};
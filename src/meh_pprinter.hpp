#include "meh_expr.hpp"
#include <string>
#include <variant>

class AstPrinter {

public:
  std::string operator()(box<Binary> const &expr) const {
    return parenthesize(expr->op.getLexeme(), {expr->left, expr->right});
  }
  std::string operator()(box<Grouping> const &expr) const {
    return parenthesize("group", {expr->expr});
  }
  std::string operator()(box<Literal> const &expr) const {
    if (std::holds_alternative<double>(expr->value)) {
      return std::to_string(std::get<double>(expr->value));
    } else if (std::holds_alternative<bool>(expr->value)) {
      return std::get<bool>(expr->value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(expr->value)) {
      return std::get<std::string>(expr->value);
    }
    return "nil";
  }
  std::string operator()(box<Unary> const &expr) const {
    return parenthesize(expr->op.getLexeme(), {expr->expr});
  }

private:
  std::string parenthesize(const std::string &name,
                           const std::vector<ExprT> &exprs) const {
    std::string builder = "(" + name;
    for (auto &expr : exprs) {
      builder += " ";
      builder += std::visit(*this, expr);
    }
    builder += ")";
    return builder;
  }
};
#include "meh_expr.hpp"
#include "meh_token.hpp"
#include <string>

struct Obj {};

using MehValue = std::variant<box<literal_t>, Obj>;

class Interpreter {
public:
  MehValue operator()(box<Binary> const &expr) const;
  MehValue operator()(box<Grouping> const &expr) const;
  MehValue operator()(box<Literal> const &expr) const;
  MehValue operator()(box<Unary> const &expr) const;

private:
  MehValue evaluate(box<ExprT> const &expr) const;
  bool isTruthy(literal_t value) const;
  bool isEqual(literal_t a, literal_t b) const;
};
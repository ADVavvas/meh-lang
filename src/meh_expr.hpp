#pragma once

#include <variant>
#include <vector>

#include "meh_token.hpp"
#include "meh_util.hpp"

class Binary;
class Call;
class Grouping;
class Literal;
class Unary;
class Variable;
class Assign;
class Logical;
class Get;
class Set;
class This;

// TODO: Rename
// An expression. Not to be confused with Expression, that represents an
// expression statement.
using ExprT = std::variant<box<Literal>, box<Binary>, box<Call>, box<Grouping>,
                           box<Unary>, box<Variable>, box<Assign>, box<Logical>,
                           box<Get>, box<Set>, box<This>, Null>;

class Binary {
public:
  Binary(ExprT left, Token op, ExprT right);

  ExprT left;
  Token op;
  ExprT right;

  bool operator==(const Binary &b) const {
    return (left == b.left && op == b.op && right == b.right);
  }

  bool operator!=(const Binary &b) const {
    return (left != b.left || op != b.op || right != b.right);
  }
};

class Call {
public:
  Call(ExprT callee, Token paren, std::vector<ExprT> arguments);

  ExprT callee;
  Token paren;
  std::vector<ExprT> arguments;

  bool operator==(const Call &c) const {
    return (callee == c.callee && arguments == c.arguments);
  }
  bool operator!=(const Call &c) const {
    return (callee != c.callee || arguments != c.arguments);
  }
};

class Grouping {
public:
  Grouping(ExprT expr);

  ExprT expr;

  bool operator==(const Grouping &g) const { return (expr == g.expr); }
  bool operator!=(const Grouping &g) const { return (expr != g.expr); }
};

// TODO: variant of what?
// using literal_t = const std::variant<double>;

class Literal {
public:
  Literal(literal_t value);

  literal_t value;

  bool operator==(const Literal &l) const { return (value == l.value); }
  bool operator!=(const Literal &l) const { return (value != l.value); }
};

class Unary {
public:
  Unary(Token op, ExprT expr);

  Token op;
  ExprT expr;

  bool operator==(const Unary &u) const {
    return (op == u.op && expr == u.expr);
  }
  bool operator!=(const Unary &u) const {
    return (op != u.op || expr != u.expr);
  }
};

class Variable {
public:
  Variable(Token name);
  Variable(const Variable &other);
  Variable(Variable &&other) noexcept;
  Variable &operator=(const Variable &other);
  Variable &operator=(Variable &&other) noexcept;
  Token name;
  int id;

  bool operator==(const Variable &v) const {
    return (name == v.name && id == v.id);
  }
  bool operator!=(const Variable &v) const {
    return (name != v.name || id != v.id);
  }

private:
  inline static int counter = 0;
};

class Assign {
public:
  Assign(Token name, ExprT value);

  Token name;
  ExprT value;

  bool operator==(const Assign &a) const {
    return (name == a.name && value == a.value);
  }
  bool operator!=(const Assign &a) const {
    return (name != a.name || value != a.value);
  }
};

class Logical {
public:
  Logical(ExprT left, Token op, ExprT right);

  ExprT left;
  ExprT right;
  Token op;

  bool operator==(const Logical &l) const {
    return (left == l.left && op == l.op && right == l.right);
  }
  bool operator!=(const Logical &l) const {
    return (left != l.left || op != l.op || right != l.right);
  }
};

class Get {
public:
  Get(ExprT obj, Token name);

  ExprT obj;
  Token name;

  bool operator==(const Get &l) const {
    return (obj == l.obj && name == l.name);
  }
  bool operator!=(const Get &l) const {
    return (obj != l.obj || name != l.name);
  }
};

class Set {
public:
  Set(ExprT obj, Token name, ExprT value);

  ExprT obj;
  Token name;
  ExprT value;

  bool operator==(const Set &l) const {
    return (obj == l.obj && name == l.name && value == l.value);
  }
  bool operator!=(const Set &l) const {
    return (obj != l.obj || name != l.name || value != l.value);
  }
};

class This {
public:
  This(Token keyword);

  Token keyword;

  bool operator==(const This &l) const { return (keyword == l.keyword); }
  bool operator!=(const This &l) const { return (keyword != l.keyword); }
};
#pragma once
#include "meh_expr.hpp"
#include "meh_token.hpp"
#include "meh_token_type.hpp"
#include <cmath>
#include <functional>
#include <variant>
#include <vector>

inline void hash_combine(std::size_t &seed) {}

template <typename T, typename... Rest>
inline void hash_combine(std::size_t &seed, const T &v, Rest... rest) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  hash_combine(seed, rest...);
}

namespace std {
template <typename T> struct hash<box<T>> {
  size_t operator()(const box<T> &x) const { return hash<T>()(*x); }
};
} // namespace std

namespace std {
template <> struct hash<TokenType> {
  size_t operator()(const TokenType &x) const { return hash<int>()(x); }
};
} // namespace std

namespace std {
template <> struct hash<Token> {
  size_t operator()(const Token &x) const {
    return hash<TokenType>()(x.getType());
  }
};
} // namespace std

namespace std {
template <> struct hash<Binary> {
  size_t operator()(const Binary &x) const {
    std::size_t h = 0;
    hash_combine(h, x.left, x.op, x.right);
    return h;
  }
};
} // namespace std

namespace std {
template <> struct hash<Grouping> {
  size_t operator()(const Grouping &x) const { return hash<ExprT>{}(x.expr); }
};
} // namespace std

namespace std {
template <> struct hash<vector<ExprT>> {
  size_t operator()(const vector<ExprT> &x) const {
    std::size_t h = 0;
    for (auto &e : x) {
      hash_combine(h, e);
    }
    return h;
  }
};
} // namespace std

namespace std {
template <> struct hash<Call> {
  size_t operator()(const Call &x) const {
    std::size_t h = 0;
    hash_combine(h, x.callee, x.arguments);
    return h;
  }
};
} // namespace std

namespace std {
template <> struct hash<Literal> {
  size_t operator()(const Literal &x) const {
    return hash<literal_t>{}(x.value);
  }
};
} // namespace std

namespace std {
template <> struct hash<Unary> {
  size_t operator()(const Unary &x) const {
    std::size_t h = 0;
    hash_combine(h, x.op, x.expr);
    return h;
  }
};
} // namespace std

namespace std {
template <> struct hash<Variable> {
  size_t operator()(const Variable &x) const {
    std::size_t h = 0;
    hash_combine(h, x.name, x.id);
    return h;
  }
};
} // namespace std

namespace std {
template <> struct hash<Assign> {
  size_t operator()(const Assign &x) const {
    std::size_t h = 0;
    hash_combine(h, x.name, x.value);
    return h;
  }
};
} // namespace std

namespace std {
template <> struct hash<Logical> {
  size_t operator()(const Logical &x) const {
    std::size_t h = 0;
    hash_combine(h, x.left, x.op, x.right);
    return h;
  }
};
} // namespace std

namespace std {
template <> struct hash<Get> {
  size_t operator()(const Get &x) const {
    std::size_t h = 0;
    hash_combine(h, x.name, x.obj);
    return h;
  }
};
} // namespace std

namespace std {
template <> struct hash<Set> {
  size_t operator()(const Set &x) const {
    std::size_t h = 0;
    hash_combine(h, x.name, x.obj, x.value);
    return h;
  }
};
} // namespace std

namespace std {
template <> struct hash<This> {
  size_t operator()(const This &x) const { return hash<Token>{}(x.keyword); }
};
} // namespace std
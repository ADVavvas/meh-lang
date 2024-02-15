#pragma once

#include "meh_util.hpp"
#include <optional>
#include <unordered_map>

class MehEnvironment {
private:
  std::unordered_map<std::string, MehValue> values;

  MehEnvironment *const enclosing;

public:
  MehEnvironment(MehEnvironment *enclosing = nullptr);
  void define(Token const &name, MehValue const &value);
  void assign(Token const &name, MehValue const &value);
  const MehValue get(Token const &name) const;
};
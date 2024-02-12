#pragma once

#include "meh_util.hpp"
#include <unordered_map>

class MehEnvironment {
private:
  std::unordered_map<std::string, MehValue> values;

public:
  const void define(Token const &name, MehValue const &value);
  const void assign(Token const &name, MehValue const &value);
  MehValue get(Token const &name) const;
};
#pragma once

#include "meh_value_fwd.hpp"
#include <memory>
#include <optional>
#include <unordered_map>

class MehEnvironment {
private:
  std::unordered_map<std::string, MehValue> values;

  std::shared_ptr<MehEnvironment> enclosing;

public:
  explicit MehEnvironment(std::shared_ptr<MehEnvironment> enclosing = nullptr);
  void define(std::string const &name, MehValue const &value);
  void assign(Token const &name, MehValue const &value);
  void assignAt(int distance, Token const &name, MehValue const &value);
  const MehValue get(Token const &name) const;
  const MehValue getAt(int distance, std::string const &name);
  MehEnvironment &ancestor(int distance);
};
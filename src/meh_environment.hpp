#pragma once

#include "meh_util.hpp"
#include <optional>
#include <unordered_map>

class MehEnvironment {
private:
  std::unordered_map<std::string, MehValue> values;

  // Probably just easier to use a raw pointer.
  std::optional<box<MehEnvironment>> enclosing;

public:
  MehEnvironment(std::optional<box<MehEnvironment>> enclosing = std::nullopt);
  const void define(Token const &name, MehValue const &value);
  const void assign(Token const &name, MehValue const &value);
  MehValue get(Token const &name) const;
};
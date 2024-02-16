#pragma once

#include "meh_interpreter_fwd.hpp"
#include "meh_token.hpp"
#include "meh_util.hpp"
#include <variant>

class MehFunction;

using MehValue = std::variant<box<literal_t>, box<MehFunction>>;

class MehFunction {
public:
  MehFunction(
      int arity,
      std::function<MehValue(Interpreter &, std::vector<MehValue>)> function);
  int getArity();
  MehValue call(Interpreter &interpreter, std::vector<MehValue> arguments);

private:
  int arity;
  std::function<MehValue(Interpreter &, std::vector<MehValue>)> function;
};
#pragma once

#include "meh_expr.hpp"
#include "meh_util.hpp"
#include <functional>
#include <vector>

class MehFunction;
using MehValue = std::variant<box<literal_t>, box<MehFunction>>;

class Interpreter;
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
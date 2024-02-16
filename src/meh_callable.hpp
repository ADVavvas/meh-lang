#pragma once

#include "meh_value.hpp"
#include <functional>
#include <vector>

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
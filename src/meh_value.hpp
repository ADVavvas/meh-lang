#pragma once

#include "meh_interpreter_fwd.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"
#include "meh_util.hpp"
#include <variant>

class MehNativeFunction;
class MehFunction;

using MehValue =
    std::variant<box<literal_t>, box<MehNativeFunction>, box<MehFunction>>;

class MehNativeFunction {
public:
  MehNativeFunction(
      int arity,
      std::function<MehValue(Interpreter &, std::vector<MehValue>)> function);
  int getArity();
  MehValue call(Interpreter &interpreter, std::vector<MehValue> arguments);

private:
  int arity;
  std::function<MehValue(Interpreter &, std::vector<MehValue>)> function;
};

class MehFunction {
public:
  MehFunction(Function function);
  MehValue call(Interpreter &interpreter, std::vector<MehValue> arguments);
  int getArity();

private:
  Function function;
};
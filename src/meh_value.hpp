#pragma once

#include "meh_environment.hpp"
#include "meh_interpreter_fwd.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"
#include "meh_util.hpp"
#include <memory>
#include <string>
#include <variant>

class MehNativeFunction;
class MehFunction;
class MehClass;

using MehValue = std::variant<box<literal_t>, box<MehNativeFunction>,
                              box<MehFunction>, box<MehClass>>;

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
  MehFunction(Function function, std::shared_ptr<MehEnvironment> closure);
  MehValue call(Interpreter &interpreter, std::vector<MehValue> arguments);
  int getArity();
  const Function getFunction() { return function; }

private:
  Function function;
  std::shared_ptr<MehEnvironment> closure;
  MehEnvironment local;
};

class MehClass {
public:
  MehClass(std::string name);
  const std::string getName() { return name; }

private:
  std::string name;
};
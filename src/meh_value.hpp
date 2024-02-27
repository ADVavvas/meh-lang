#pragma once

#include "meh_environment.hpp"
#include "meh_interpreter_fwd.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"
#include "meh_util.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

class MehNativeFunction;
class MehFunction;
class MehClass;
class MehInstance;

using MehValue =
    std::variant<box<literal_t>, box<MehNativeFunction>, box<MehFunction>,
                 box<MehClass>, box<MehInstance>>;

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
  const MehFunction bind(const MehInstance &instance) const;
  const Function getFunction() { return function; }

private:
  Function function;
  std::shared_ptr<MehEnvironment> closure;
  MehEnvironment local;
};

class MehClass {
public:
  MehClass(std::string name,
           std::unordered_map<std::string, MehFunction> methods);
  MehValue call(Interpreter &interpreter, std::vector<MehValue> arguments);
  const std::string getName() const { return name; }
  const int getArity() const;
  const std::optional<const MehFunction> getMethod(std::string name) const;

private:
  std::string name;
  std::unordered_map<std::string, MehFunction> methods;
};

class MehInstance {
public:
  MehInstance(MehClass klass);

  const MehClass &getKlass() const;
  const std::string toString() const;

  MehValue &getMut(Token name);
  MehValue get(Token name);
  void set(Token name, MehValue value);

private:
  std::unordered_map<std::string, MehValue> fields;
  MehClass klass;
};
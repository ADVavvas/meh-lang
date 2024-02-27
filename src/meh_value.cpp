#include "meh_value.hpp"
#include "meh_environment.hpp"
#include "meh_interpreter.hpp"
#include "meh_return.hpp"
#include "meh_runtime_error.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"
#include <memory>

MehNativeFunction::MehNativeFunction(
    int arity,
    std::function<MehValue(Interpreter &, std::vector<MehValue>)> function)
    : arity{arity}, function{function} {};

int MehNativeFunction::getArity() { return arity; }

MehValue MehNativeFunction::call(Interpreter &interpreter,
                                 std::vector<MehValue> arguments) {
  return function(interpreter, arguments);
}

MehFunction::MehFunction(Function function,
                         std::shared_ptr<MehEnvironment> closure)
    : function{function}, closure{closure} {};

int MehFunction::getArity() { return function.params.size(); }

const MehFunction MehFunction::bind(const MehInstance &instance) const {
  std::shared_ptr<MehEnvironment> local =
      std::make_shared<MehEnvironment>(closure);
  local->define("this", instance);
  return MehFunction{function, local};
}

MehValue MehFunction::call(Interpreter &interpreter,
                           std::vector<MehValue> arguments) {
  std::shared_ptr<MehEnvironment> local =
      std::make_shared<MehEnvironment>(closure);
  for (int i = 0; i < function.params.size(); i++) {
    local->define(function.params[i].getLexeme(), arguments[i]);
  }

  try {
    interpreter.executeBlock(function.body, local);
  } catch (MehReturn returnValue) {
    return returnValue.getValue();
  }
  return MehValue{literal_t{Null{}}};
}

MehClass::MehClass(std::string name,
                   std::unordered_map<std::string, MehFunction> methods)
    : name{name}, methods{methods} {}

MehValue MehClass::call(Interpreter &interpreter,
                        std::vector<MehValue> arguments) {
  MehInstance instance{*this};
  return instance;
}

const int MehClass::getArity() const { return 0; }
const std::optional<const MehFunction>
MehClass::getMethod(std::string name) const {
  if (methods.find(name) != methods.end()) {
    return methods.at(name);
  }
  return std::nullopt;
}

MehInstance::MehInstance(MehClass klass) : klass{klass} {}

const MehClass &MehInstance::getKlass() const { return klass; }
const std::string MehInstance::toString() const {
  return klass.getName() + " instance";
};

MehValue &MehInstance::getMut(Token name) {
  if (fields.find(name.getLexeme()) != fields.end()) {
    return fields.at(name.getLexeme());
  }

  throw MehRuntimeError{name, "Undefined property '" + name.getLexeme() + "'."};
}

MehValue MehInstance::get(Token name) {
  if (fields.find(name.getLexeme()) != fields.end()) {
    return fields.at(name.getLexeme());
  }

  if (auto method = klass.getMethod(name.getLexeme()); method.has_value()) {
    return method.value().bind(*this);
  }

  throw MehRuntimeError{name, "Undefined property '" + name.getLexeme() + "'."};
}

void MehInstance::set(Token name, MehValue value) {
  fields.insert_or_assign(name.getLexeme(), value);
}
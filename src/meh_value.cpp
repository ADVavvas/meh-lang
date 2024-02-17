#include "meh_value.hpp"
#include "meh_environment.hpp"
#include "meh_interpreter.hpp"
#include "meh_stmt.hpp"
#include "meh_token.hpp"

MehNativeFunction::MehNativeFunction(
    int arity,
    std::function<MehValue(Interpreter &, std::vector<MehValue>)> function)
    : arity{arity}, function{function} {};

int MehNativeFunction::getArity() { return arity; }

MehValue MehNativeFunction::call(Interpreter &interpreter,
                                 std::vector<MehValue> arguments) {
  return function(interpreter, arguments);
}

MehFunction::MehFunction(Function function) : function{function} {};

int MehFunction::getArity() { return function.params.size(); }

MehValue MehFunction::call(Interpreter &interpreter,
                           std::vector<MehValue> arguments) {
  MehEnvironment environment{interpreter.getGlobalEnvironment()};
  for (int i = 0; i < function.params.size(); i++) {
    environment.define(function.params[i].getLexeme(), arguments[i]);
  }

  interpreter.executeBlock(function.body, &environment);
  return MehValue{literal_t{Null{}}};
}

#include "meh_value.hpp"
#include "meh_environment.hpp"
#include "meh_interpreter.hpp"
#include "meh_return.hpp"
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

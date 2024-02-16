#include "meh_value.hpp"

MehFunction::MehFunction(
    int arity,
    std::function<MehValue(Interpreter &, std::vector<MehValue>)> function)
    : arity{arity}, function{function} {};

int MehFunction::getArity() { return arity; }

MehValue MehFunction::call(Interpreter &interpreter,
                           std::vector<MehValue> arguments) {
  return function(interpreter, arguments);
}
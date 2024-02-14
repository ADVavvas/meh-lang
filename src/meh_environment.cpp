#include "meh_environment.hpp"
#include "meh_runtime_error.hpp"

MehEnvironment::MehEnvironment(std::optional<box<MehEnvironment>> enclosing)
    : enclosing{enclosing} {}

const void MehEnvironment::define(Token const &name, MehValue const &value) {
  values.insert_or_assign(name.getLexeme(), value);
}

const void MehEnvironment::assign(Token const &name, MehValue const &value) {
  if (values.find(name.getLexeme()) != values.end()) {
    values.insert_or_assign(name.getLexeme(), value);
    return;
  }

  if (enclosing.has_value()) {
    enclosing.value()->assign(name, value);
  }

  throw MehRuntimeError{name, "Undefined variable '" + name.getLexeme() + "'."};
}

MehValue MehEnvironment::get(Token const &name) const {
  if (values.find(name.getLexeme()) != values.end()) {
    return values.at(name.getLexeme());
  }

  if (enclosing.has_value()) {
    return enclosing.value()->get(name);
  }
  throw MehRuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
}
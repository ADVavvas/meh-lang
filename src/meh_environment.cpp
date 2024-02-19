#include "meh_environment.hpp"
#include "meh_runtime_error.hpp"
#include "meh_value.hpp"

MehEnvironment::MehEnvironment(std::shared_ptr<MehEnvironment> enclosing)
    : enclosing{enclosing} {}

void MehEnvironment::define(std::string const &name, MehValue const &value) {
  values.insert_or_assign(name, value);
}

void MehEnvironment::assign(Token const &name, MehValue const &value) {
  if (values.find(name.getLexeme()) != values.end()) {
    values.insert_or_assign(name.getLexeme(), value);
    return;
  }

  if (enclosing != nullptr) {
    enclosing->assign(name, value);
    return;
  }

  throw MehRuntimeError{name, "Undefined variable '" + name.getLexeme() + "'."};
}

const MehValue MehEnvironment::get(Token const &name) const {
  if (values.find(name.getLexeme()) != values.end()) {
    return values.at(name.getLexeme());
  }

  if (enclosing != nullptr) {
    return enclosing->get(name);
  }
  throw MehRuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
}
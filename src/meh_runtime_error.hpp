#pragma once

#include "meh_token.hpp"
#include <stdexcept>

class MehRuntimeError : public std::runtime_error {
public:
  explicit MehRuntimeError(Token token, const std::string &message)
      : std::runtime_error{message}, token{token} {}

private:
  const Token token;
};

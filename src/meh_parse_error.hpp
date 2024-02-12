#pragma once

#include "meh_token.hpp"
#include <exception>
#include <stdexcept>

class MehParseError : public std::runtime_error {
public:
  explicit MehParseError(Token token, const std::string &message)
      : std::runtime_error{message}, token{token} {}

private:
  const Token token;
};
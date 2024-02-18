#pragma once

#include "meh_value.hpp"
#include <stdexcept>

class MehReturn : public std::runtime_error {
public:
  explicit MehReturn(MehValue value) : std::runtime_error{""}, value{value} {}
  const MehValue getValue() const { return value; }

private:
  const MehValue value;
};
#pragma once
#include <string>

class Error {
public:
  Error(int line, const std::string &message) : line(line), message(message) {}

  int getLine() const { return line; }

  const std::string &getMessage() const { return message; }

private:
  int line;
  std::string message;
};

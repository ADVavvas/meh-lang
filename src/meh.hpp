#pragma once

#include "meh_error.hpp"
#include "meh_token.hpp"
#include <string>
class Meh {
public:
  static void run(const std::string &source);
  static void runFile(const std::string &path);
  static void runInteractive();
  static void error(Error error);
  static void error(Token token, std::string message);
  static void report(Error error);

private:
  inline static bool hadError = false;
};

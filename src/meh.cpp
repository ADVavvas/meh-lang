#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <variant>

#include "meh.hpp"
#include "meh_expr.hpp"
#include "meh_parser.hpp"
#include "meh_pprinter.hpp"
#include "meh_scanner.hpp"
#include "meh_token.hpp"

#include "iostream"

void Meh::run(const std::string &source) {
  Scanner scanner(source);
  std::vector<Token> tokens = scanner.scanTokens();

  Parser parser{tokens};
  ExprT expr = parser.parse();

  if (hadError) {
    return;
  }

  AstPrinter printer{};
  std::cout << std::visit(printer, expr) << std::endl;
}

void Meh::runFile(const std::string &path) {
  std::ifstream file(path);

  if (!file.is_open()) {
    std::cerr << "Could not open file: " << path << std::endl;
    return;
  }
  std::string line;

  while (getline(file, line)) {
    std::cout << line << '\n';
  }

  if (hadError) // Throw exception

    file.close();
}

void Meh::runInteractive() {
  std::string line;

  std::cout << "\n> ";
  while (getline(std::cin, line)) {
    run(line);
    std::cout << "> ";
    // After each loop
    hadError = false;
  }
}

void Meh::error(Error error) {
  hadError = true;
  report(error);
}

void Meh::error(Token token, std::string message) {
  if (token.getType() == TokenType::FILE_END) {
    report(Error(token.getLine(), " at end " + message));
  } else {
    report(
        Error(token.getLine(), " at '" + token.getLexeme() + "': " + message));
  }
}

void Meh::report(Error error) {
  std::cout << "Error line: " << error.getLine() << ": " << error.getMessage()
            << std::endl;
}

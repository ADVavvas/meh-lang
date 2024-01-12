#include <fstream>
#include <iostream>
#include <string>

#include "meh.hpp"
#include "meh_scanner.hpp"

void Meh::run(const std::string &source) {
  Scanner scanner(source);
  scanner.scanTokens();
  // Scanner scanner = new Scanner(source);
  //   List<Token> tokens = scanner.scanTokens();

  //   // For now, just print the tokens.
  //   for (Token token : tokens) {
  //     System.out.println(token);
  //   }
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

void Meh::report(Error error) {
  std::cout << "Error line: " << error.getLine() << ": " << error.getMessage()
            << std::endl;
}

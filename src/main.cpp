#include "meh.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  // Print the number of arguments
  std::cout << "Number of arguments: " << argc << std::endl;
  Meh meh;
  if (argc > 2) {
    std::cout << "Usage: meh [script]" << std::endl;
    return 1;
  } else if (argc == 2) {
    std::cout << "Running script: " << argv[1] << std::endl;
    meh.runFile(argv[1]);
  } else {
    std::cout << "Running interactive mode" << std::endl;
    meh.runInteractive();
  }

  return 0;
}

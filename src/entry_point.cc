/**
 * @file entry_point.cc
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <exception>
#include <iostream>

#include "application.h"

int main(int argc, char* argv[]) {
  for (int i = 0; i < argc; ++i) {
    std::clog << i + 1 << "/" << argc << ": " << argv[i] << '\n';
  }

  try {
    rt::Application app{};
    app.Run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }

  return EXIT_SUCCESS;
}
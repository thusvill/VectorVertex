#include "src/Core/VectorVertex.hpp"
#include <Log.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
  VectorVertex::Log::Init();
  VectorVertex::ProjectInfo info{};
  info.width = 1600;
  info.height = 900;
  info.title = "VectorVertex";
  VectorVertex::VectorVetrex app{info};
  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

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
  info.path = "/home/bios/CLionProjects/VectorVertex/3DEngine/build/assets/scene/Example.vscene";
  VectorVertex::Application app{info};
  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

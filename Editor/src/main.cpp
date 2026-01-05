#include <Core/Application.hpp>
#include <Core/Log.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <Layers/Editor_Layer.hpp>

int main() {
  VectorVertex::Log::Init();
  VectorVertex::ProjectInfo info{};
  info.width = 1600;
  info.height = 900;
  info.title = "VectorVertex";
  //info.path = "/home/bios/VectorVertex/build/assets/scene/Example.vscene";
  VectorVertex::Application app{info};

  VectorVertex::EditorLayer* editor_layer = new VectorVertex::EditorLayer(info);
  app.PushLayer(editor_layer);
  editor_layer->SetupImgui();

  
  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
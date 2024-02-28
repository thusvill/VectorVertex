#include "src/Core/VectorVertex.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <Log.h>

int main()
{
    VectorVertex::Log::Init();
    VectorVertex::ProjectInfo info{};
    info.height = 800;
    info.width = 800;
    info.title = "VectorVertex";
    VectorVertex::VectorVetrex app{info};
    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
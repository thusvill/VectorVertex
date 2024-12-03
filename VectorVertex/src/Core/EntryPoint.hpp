#pragma once
#include <Core/Application.hpp>

extern VectorVertex::Application *VectorVertex::CreateApplication();

int main(int argc, char **argv)
{
    auto app = VectorVertex::CreateApplication();
    app->Run();
    delete app;
    return 0;
}

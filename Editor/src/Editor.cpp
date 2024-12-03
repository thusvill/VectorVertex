#include <VectorVertex.h>
#include <Core/EntryPoint.hpp>

namespace VectorVertex
{

    class VVEditor : public Application
    {
    public:
        VVEditor(ProjectInfo &info) : Application(info)
        {
            // Create a new window
        }
    };

    Application *CreateApplication()
    {
        ProjectInfo info{};
        info.width = 1600;
        info.height = 900;
        info.title = "VectorVertex";

        return new VVEditor(info);
    }
}

//   ProjectInfo info{};
//   info.width = 1600;
//   info.height = 900;
//   info.title = "VectorVertex";
//   //info.path = "/home/bios/CLionProjects/VectorVertex/VectorVertex/build/assets/scene/Example.vscene";

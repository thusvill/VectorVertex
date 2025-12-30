#include "Window.hpp"
#include <Core/Base.h>

// #if VV_PLATTFORM == LINUX
#include <Plattform/Linux/LinuxWindow.hpp>
// #endif
namespace VectorVertex
{

	Scope<Window> Window::Create(const WindowProps &props)
	{
//#if VV_PLATTFORM == LINUX
		return CreateScope<LinuxWindow>(props);
//#else
		// VV_CORE_ASSERT(false, "Unknown platform!");
		// return nullptr;
// #endif
	}

}
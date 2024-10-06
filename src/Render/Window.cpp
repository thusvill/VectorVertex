#include "Window.hpp"
#include <Base.h>

#ifdef VV_PLATTFORM_LINUX
    #include <Plattform/Linux/LinuxWindow.hpp>
#endif
namespace VectorVertex
{

   Scope<Window> Window::Create(const WindowProps& props)
	{
	#ifdef VV_PLATTFORM_LINUX
		return CreateScope<LinuxWindow>(props);
	#else
		VV_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
	}

}
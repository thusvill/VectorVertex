#include <Input.hpp>
#include <MouseCodes.hpp>
#include <KeyCodes.hpp>
#include <Application.hpp>
#include <GraphicsContext.hpp>
#include <GLFW/glfw3.h>

namespace VectorVertex {

	bool Input::IsKeyPressed(const KeyCode key)
	{
		auto* window = Application::Get().GetNativeWindow();
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		auto* window = Application::Get().GetNativeWindow();
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		auto* window = Application::Get().GetNativeWindow();
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

}
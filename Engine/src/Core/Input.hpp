#pragma once

#include <KeyCodes.hpp>
#include <MouseCodes.hpp>

#include <glm/glm.hpp>

namespace VectorVertex {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
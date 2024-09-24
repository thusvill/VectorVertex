#include "vv_game_object.hpp"

namespace VectorVertex
{
    
    VVGameObject VVGameObject::MakePointLight(float intensity, float radius, glm::vec3 color)
    {
        VVGameObject _object = VVGameObject::CreateGameObject("point_light");
        _object.color = color;
        _object.transform.scale.x = radius;
        _object.point_light = std::make_unique<PointLightComponent>();
        _object.point_light->light_intensity = intensity;

        return _object;
    }
} // namespace VectorVertex

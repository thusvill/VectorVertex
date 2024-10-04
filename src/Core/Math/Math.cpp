#include "Math.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace VectorVertex::Math
{
    bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &position, glm::vec3 &rotation, glm::vec3 &scale)
    {
        using namespace glm;

        // Extract translation from the last column
        position = vec3(transform[3]);

        // Extract scale from the basis vectors (first three columns)
        scale.x = length(vec3(transform[0]));
        scale.y = length(vec3(transform[1]));
        scale.z = length(vec3(transform[2]));

        // Remove scale from the matrix to isolate the rotation matrix
        mat4 rotationMatrix = transform;

        if (scale.x != 0)
            rotationMatrix[0] /= scale.x;
        if (scale.y != 0)
            rotationMatrix[1] /= scale.y;
        if (scale.z != 0)
            rotationMatrix[2] /= scale.z;

        // Extract rotation (in radians) from the rotation matrix
        rotation.x = atan2(rotationMatrix[2][1], rotationMatrix[2][2]);                                                                             // Pitch (X axis)
        rotation.y = atan2(-rotationMatrix[2][0], sqrt(rotationMatrix[2][1] * rotationMatrix[2][1] + rotationMatrix[2][2] * rotationMatrix[2][2])); // Yaw (Y axis)
        rotation.z = atan2(rotationMatrix[1][0], rotationMatrix[0][0]);                                                                             // Roll (Z axis)

        return true;
    }

} // namespace VectorVertex

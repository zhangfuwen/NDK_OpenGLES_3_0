//
// Created by zhangfuwen on 2022/2/24.
//

#include <GLUtils.h>
#include <mesh.h>
#include <model.h>
#include "Transform.h"
#include "../glm/glm.hpp"

glm::mat4 Transform::GetModel() const {
    float radiansX = static_cast<float>(MATH_PI / 180.0f * rotation[0]);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * rotation[1]);
    float radiansZ = static_cast<float>(MATH_PI / 180.0f * rotation[2]);

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, scale);
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::rotate(Model, radiansZ, glm::vec3(0.0f, 0.0f, 1.0f));
    Model = glm::translate(Model, translation);
    return Model;
}
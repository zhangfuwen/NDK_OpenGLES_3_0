//
// Created by zhangfuwen on 2022/2/24.
//

#include <GLUtils.h>
#include <mesh.h>
#include <model.h>
#include "Transform.h"
#include "../glm/glm.hpp"
void Transform::GetMVPMatrix(glm::mat4 &mvpMatrix) const
{

    float radiansX = static_cast<float>(MATH_PI / 180.0f * rotation[0]);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * rotation[1]);
    float radiansZ = static_cast<float>(MATH_PI / 180.0f * rotation[2]);

    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.7f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, 100);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
           glm::vec3(0.0f, 0.0f, 0.8f), // Camera is at (1,1,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, scale);
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::rotate(Model, radiansZ, glm::vec3(0.0f, 0.0f, 1.0f));
    Model = glm::translate(Model, translation);
    mvpMatrix = Projection * View * Model;
}
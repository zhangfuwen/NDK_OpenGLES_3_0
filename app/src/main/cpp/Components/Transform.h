//
// Created by zhangfuwen on 2022/2/24.
//

#ifndef NDK_OPENGLES_3_0_TRANSFORM_H
#define NDK_OPENGLES_3_0_TRANSFORM_H


#include "../glm/glm.hpp"
class Transform {
public:
    glm::vec3 translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f}; // by degrees


    void GetMVPMatrix(glm::mat4 &mvpMatrix) const;
};


#endif //NDK_OPENGLES_3_0_TRANSFORM_H

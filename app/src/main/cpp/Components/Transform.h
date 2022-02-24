//
// Created by zhangfuwen on 2022/2/24.
//

#ifndef NDK_OPENGLES_3_0_TRANSFORM_H
#define NDK_OPENGLES_3_0_TRANSFORM_H


#include "../glm/glm.hpp"
class Transform {
public:
    glm::vec3 translation;
    glm::vec3 scale;
    glm::vec3 rotation; // by degrees


    void GetMVPMatrix(glm::mat4 &mvpMatrix) const;
};


#endif //NDK_OPENGLES_3_0_TRANSFORM_H

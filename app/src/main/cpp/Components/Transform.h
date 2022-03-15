//
// Created by zhangfuwen on 2022/2/24.
//

#ifndef NDK_OPENGLES_3_0_TRANSFORM_H
#define NDK_OPENGLES_3_0_TRANSFORM_H


#include "model/model.h"
#include "../glm/glm.hpp"
class Transform {
public:
    glm::vec3 translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f}; // by degrees

    glm::mat4 GetModel() const;

    inline static glm::vec3 front{ 0.0f, 0.0f, 1.0f };
    static inline glm::vec3 right{ 1.0f, 0.0f, 0.0f };
    static inline glm::vec3 up{ 0.0f, 1.0f, 0.0f };

};


#endif //NDK_OPENGLES_3_0_TRANSFORM_H

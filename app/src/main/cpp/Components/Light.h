//
// Created by zhangfuwen on 2022/3/7.
//

#ifndef NDK_OPENGLES_3_0_LIGHT_H
#define NDK_OPENGLES_3_0_LIGHT_H

#include <glm/glm.hpp>

class Light {
public:
    Light(glm::vec3 pos, glm::vec3 ka, glm::vec3 kd, glm::vec3 ks) {
        m_lightPos = pos;
        m_ambientColor = ka;
        m_diffuseColor = ka;
        m_specularColor = ks;
    }
    glm::vec3 getLightPos() const {
        return m_lightPos;
    }
    glm::vec3 getAmbientColor() const {
        return m_ambientColor;
    }

    glm::vec3 getDiffuseColor() const {
        return m_diffuseColor;
    }

    glm::vec3 getSpecularColor() const {
        return m_specularColor;
    }
private:
    glm::vec3 m_lightPos;
    glm::vec3 m_ambientColor;
    glm::vec3 m_diffuseColor;
    glm::vec3 m_specularColor;
};


#endif //NDK_OPENGLES_3_0_LIGHT_H
